#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "zmk/hid.h"
#include "zmk/endpoints.h"
#include "drivers/sensor/gen4.h"

LOG_MODULE_REGISTER(trackpad, CONFIG_SENSOR_LOG_LEVEL);

const struct device *trackpad = DEVICE_DT_GET(DT_INST(0, cirque_gen4));

static void handle_trackpad(const struct device *dev, const struct sensor_trigger *trig) {
    static uint8_t last_pressed = 0;
    int ret = sensor_sample_fetch(dev);
    if (ret < 0) {
        LOG_ERR("fetch: %d", ret);
        return;
    }
    LOG_DBG("Trackpad handler trigd %d", 0);
}

static int trackpad_init() {
    struct sensor_trigger trigger = {
        .type = SENSOR_TRIG_DATA_READY,
        .chan = SENSOR_CHAN_ALL,
    };
    printk("trackpad");
    if (sensor_trigger_set(trackpad, &trigger, handle_trackpad) < 0) {
        LOG_ERR("can't set trigger");
        return -EIO;
    };
    return 0;
}

SYS_INIT(trackpad_init, APPLICATION, CONFIG_ZMK_KSCAN_INIT_PRIORITY);