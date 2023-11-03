#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

#include <zmk/trackpad.h>
#include <zmk/hid.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/sensor_event.h>
#include "drivers/sensor/gen4.h"

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

const struct device *trackpad = DEVICE_DT_GET(DT_INST(0, cirque_gen4));

static zmk_trackpad_finger_contacts_t present_contacts = 0;
static zmk_trackpad_finger_contacts_t contacts_to_send = 0;

static uint8_t btns;
static uint16_t scantime;

static struct zmk_ptp_finger fingers[CONFIG_ZMK_TRACKPAD_MAX_FINGERS];

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD_WORK_QUEUE_DEDICATED)
K_THREAD_STACK_DEFINE(trackpad_work_stack_area, CONFIG_ZMK_TRACKPAD_DEDICATED_THREAD_STACK_SIZE);
static struct k_work_q trackpad_work_q;
#endif

struct k_work_q *zmk_trackpad_work_q() {
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD_WORK_QUEUE_DEDICATED)
    return &trackpad_work_q;
#else
    return &k_sys_work_q;
#endif
}

static void handle_trackpad(const struct device *dev, const struct sensor_trigger *trig) {
    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        LOG_ERR("fetch: %d", ret);
        return;
    }
    LOG_DBG("Trackpad handler trigd %d", 0);

    struct sensor_value contacts, confidence_tip, id, x, y, buttons, scan_time;
    sensor_channel_get(dev, SENSOR_CHAN_CONTACTS, &contacts);
    sensor_channel_get(dev, SENSOR_CHAN_BUTTONS, &buttons);
    sensor_channel_get(dev, SENSOR_CHAN_SCAN_TIME, &scan_time);
    // expects bitmap format
    present_contacts = contacts.val1;
    // Buttons and scan time
    btns = buttons.val1;
    scantime = scan_time.val1;
    // released Fingers
    sensor_channel_get(dev, SENSOR_CHAN_X, &x);
    sensor_channel_get(dev, SENSOR_CHAN_Y, &y);
    sensor_channel_get(dev, SENSOR_CHAN_CONFIDENCE_TIP, &confidence_tip);
    sensor_channel_get(dev, SENSOR_CHAN_FINGER, &id);
    // If finger has changed
    fingers[id.val1].confidence_tip = confidence_tip.val1;
    fingers[id.val1].contact_id = id.val1;
    fingers[id.val1].x = x.val1;
    fingers[id.val1].y = y.val1;
    contacts_to_send |= BIT(id.val1);

    ZMK_EVENT_RAISE(new_zmk_sensor_event(
        (struct zmk_sensor_event){.sensor_index = 0,
                                  .channel_data_size = 1,
                                  .channel_data = {(struct zmk_sensor_channel_data){
                                      .value = btns, .channel = SENSOR_CHAN_BUTTONS}},
                                  .timestamp = k_uptime_get()}));
}

static void zmk_trackpad_tick(struct k_work *work) {
    if (contacts_to_send) {
        // LOG_DBG("Trackpad sendy thing trigd %d", 0);
        for (int i = 0; i < CONFIG_ZMK_TRACKPAD_MAX_FINGERS; i++)
            if (contacts_to_send & BIT(i)) {
                LOG_DBG("Trackpad sendy thing trigd %d", i);
                zmk_hid_ptp_set(fingers[i], present_contacts, scantime, btns);
                zmk_endpoints_send_ptp_report();
                contacts_to_send &= !BIT(i);
                return;
            }
    }
}

K_WORK_DEFINE(trackpad_work, zmk_trackpad_tick);

static void zmk_trackpad_tick_handler(struct k_timer *timer) {
    k_work_submit_to_queue(zmk_trackpad_work_q(), &trackpad_work);
}

K_TIMER_DEFINE(trackpad_tick, zmk_trackpad_tick_handler, NULL);

static int trackpad_init() {
    struct sensor_trigger trigger = {
        .type = SENSOR_TRIG_DATA_READY,
        .chan = SENSOR_CHAN_ALL,
    };
    if (sensor_trigger_set(trackpad, &trigger, handle_trackpad) < 0) {
        LOG_ERR("can't set trigger");
        return -EIO;
    };
    k_timer_start(&trackpad_tick, K_NO_WAIT, K_MSEC(CONFIG_ZMK_TRACKPAD_TICK_DURATION));
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD_WORK_QUEUE_DEDICATED)
    k_work_queue_start(&trackpad_work_q, trackpad_work_stack_area,
                       K_THREAD_STACK_SIZEOF(trackpad_work_stack_area),
                       CONFIG_ZMK_TRACKPAD_DEDICATED_THREAD_PRIORITY, NULL);
#endif
    return 0;
}

SYS_INIT(trackpad_init, APPLICATION, CONFIG_KSCAN_INIT_PRIORITY);