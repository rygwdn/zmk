#define DT_DRV_COMPAT cirque_gen4

#include <zephyr/init.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <drivers/sensor/gen4.h>
#include "gen4.h"

LOG_MODULE_REGISTER(gen4, CONFIG_SENSOR_LOG_LEVEL);

static int gen4_normal_read(const struct device *dev, uint8_t *buf, const uint8_t len) {
    const struct gen4_config *config = dev->config;
    return i2c_read_dt(&config->bus, buf, len);
}

static int gen4_i2c_init(const struct device *dev) {
    const struct gen4_config *cfg = dev->config;
    uint8_t request[2] = {0x20, 0x00};
    uint8_t buffer[30];
    int ret = i2c_write_read_dt(&cfg->bus, request, 2, buffer, 30);
    if (ret < 0) {
        LOG_ERR("ext read status: %d", ret);
        return ret;
    }
    LOG_DBG("received value %x", buffer[2]);
    // enable absolute mode
    uint8_t request2[10] = {0x05, 0x00, 0x34, 0x03, 0x06, 0x00, 0x04, 0x00, 0x04, 0x03};
    ret = i2c_write_dt(&cfg->bus, request2, 10);
    if (ret < 0) {
        LOG_ERR("ext read status: %d", ret);
        return ret;
    }
    return 0;
}

static int gen4_channel_get(const struct device *dev, enum sensor_channel chan,
                            struct sensor_value *val) {
    const struct gen4_data *data = dev->data;
    switch ((enum sensor_channel_gen4)chan) {
    case SENSOR_CHAN_FINGERS:
        val->val1 = data->contacts;
        break;
    case SENSOR_CHAN_X_0:
        val->val1 = data->fingers[0].x;
        break;
    case SENSOR_CHAN_X_1:
        val->val1 = data->fingers[1].x;
        break;
    case SENSOR_CHAN_X_2:
        val->val1 = data->fingers[2].x;
        break;
    case SENSOR_CHAN_X_3:
        val->val1 = data->fingers[3].x;
        break;
    case SENSOR_CHAN_X_4:
        val->val1 = data->fingers[4].x;
        break;
    case SENSOR_CHAN_Y_0:
        val->val1 = data->fingers[0].y;
        break;
    case SENSOR_CHAN_Y_1:
        val->val1 = data->fingers[1].y;
        break;
    case SENSOR_CHAN_Y_2:
        val->val1 = data->fingers[2].y;
        break;
    case SENSOR_CHAN_Y_3:
        val->val1 = data->fingers[3].y;
        break;
    case SENSOR_CHAN_Y_4:
        val->val1 = data->fingers[4].y;
        break;
    case SENSOR_CHAN_PRESENT_0:
        val->val1 = data->fingers[0].present;
        break;
    case SENSOR_CHAN_PRESENT_1:
        val->val1 = data->fingers[1].present;
        break;
    case SENSOR_CHAN_PRESENT_2:
        val->val1 = data->fingers[2].present;
        break;
    case SENSOR_CHAN_PRESENT_3:
        val->val1 = data->fingers[3].present;
        break;
    case SENSOR_CHAN_PRESENT_4:
        val->val1 = data->fingers[4].present;
        break;
    case SENSOR_CHAN_PALM_0:
        val->val1 = data->fingers[0].palm;
        break;
    case SENSOR_CHAN_PALM_1:
        val->val1 = data->fingers[1].palm;
        break;
    case SENSOR_CHAN_PALM_2:
        val->val1 = data->fingers[2].palm;
        break;
    case SENSOR_CHAN_PALM_3:
        val->val1 = data->fingers[3].palm;
        break;
    case SENSOR_CHAN_PALM_4:
        val->val1 = data->fingers[4].palm;
        break;
    case SENSOR_CHAN_BUTTONS:
        val->val1 = data->btns;
        break;
    default:
        return -ENOTSUP;
    }
    return 0;
}

static int gen4_sample_fetch(const struct device *dev, enum sensor_channel) {
    uint8_t packet[53];
    int ret;
    ret = gen4_normal_read(dev, packet, 52);
    if (ret < 0) {
        LOG_ERR("read status: %d", ret);
        return ret;
    }
    if (!(packet[REPORT_ID_SHIFT] == PTP_REPORT_ID)) {
        return -EAGAIN;
    }

    uint16_t report_length = packet[LENGTH_LOWBYTE_SHIFT] | (packet[LENGTH_HIGHBYTE_SHIFT] << 8);

    struct gen4_data *data = dev->data;

    if (report_length == 12) {
        data->contacts = packet[10];
        data->btns = packet[11];
    } else {
        data->contacts = packet[11];
        data->btns = packet[12];
    }

    uint8_t finger_id = (packet[3] & 0xFC) >> 2;
    LOG_DBG("FINGER ID: %d", finger_id);
    // Finger data
    data->fingers[finger_id].present = (packet[3] & 0x02) >> 1;
    data->fingers[finger_id].palm = packet[3] & 0x01;
    data->fingers[finger_id].x = (uint16_t)packet[4] | (uint16_t)(packet[5] << 8);
    data->fingers[finger_id].y = (uint16_t)packet[6] | (uint16_t)(packet[7] << 8);

    LOG_DBG("Finger detected: %d", data->fingers[finger_id].present);
    LOG_DBG("Finger palm: %d", data->fingers[finger_id].palm);
    LOG_DBG("Finger x: %d", data->fingers[finger_id].x);
    LOG_DBG("Finger y: %d", data->fingers[finger_id].y);

    return 0;
}

#ifdef CONFIG_GEN4_TRIGGER
static void set_int(const struct device *dev, const bool en) {
    const struct gen4_config *config = dev->config;
    int ret = gpio_pin_interrupt_configure_dt(&config->dr,
                                              en ? GPIO_INT_EDGE_TO_ACTIVE : GPIO_INT_DISABLE);
    if (ret < 0) {
        LOG_ERR("can't set interrupt");
    }
}

static int gen4_trigger_set(const struct device *dev, const struct sensor_trigger *trig,
                            sensor_trigger_handler_t handler) {
    struct gen4_data *data = dev->data;

    set_int(dev, false);
    if (trig->type != SENSOR_TRIG_DATA_READY) {
        return -ENOTSUP;
    }
    data->data_ready_trigger = trig;
    data->data_ready_handler = handler;
    set_int(dev, true);
    return 0;
}

static void gen4_int_cb(const struct device *dev) {
    struct gen4_data *data = dev->data;
    LOG_DBG("Gen4 interrupt trigd: %d", 0);
    gen4_sample_fetch(dev, 0);
    data->data_ready_handler(dev, data->data_ready_trigger);
    set_int(dev, true);
    data->in_int = false;
}

#ifdef CONFIG_GEN4_TRIGGER_OWN_THREAD
static void gen4_thread(void *arg) {
    const struct device *dev = arg;
    struct gen4_data *data = dev->data;

    while (1) {
        k_sem_take(&data->gpio_sem, K_FOREVER);
        gen4_int_cb(dev);
    }
}
#elif defined(CONFIG_GEN4_TRIGGER_GLOBAL_THREAD)
static void gen4_work_cb(struct k_work *work) {
    struct gen4_data *data = CONTAINER_OF(work, struct gen4_data, work);
    gen4_int_cb(data->dev);
}
#endif

static void gen4_gpio_cb(const struct device *port, struct gpio_callback *cb, uint32_t pins) {
    struct gen4_data *data = CONTAINER_OF(cb, struct gen4_data, gpio_cb);
    data->in_int = true;
#if defined(CONFIG_GEN4_TRIGGER_OWN_THREAD)
    k_sem_give(&data->gpio_sem);
#elif defined(CONFIG_GEN4_TRIGGER_GLOBAL_THREAD)
    k_work_submit(&data->work);
#endif
}
#endif

static int gen4_init(const struct device *dev) {
    struct gen4_data *data = dev->data;
    const struct gen4_config *config = dev->config;

    gen4_i2c_init(dev);
    gen4_sample_fetch(dev, 0);
#ifdef CONFIG_GEN4_TRIGGER
    data->dev = dev;
    gpio_pin_configure_dt(&config->dr, GPIO_INPUT);
    gpio_init_callback(&data->gpio_cb, gen4_gpio_cb, BIT(config->dr.pin));
    int ret = gpio_add_callback(config->dr.port, &data->gpio_cb);
    if (ret < 0) {
        LOG_ERR("Failed to set DR callback: %d", ret);
        return -EIO;
    }

#if defined(CONFIG_GEN4_TRIGGER_OWN_THREAD)
    k_sem_init(&data->gpio_sem, 0, UINT_MAX);

    k_thread_create(&data->thread, data->thread_stack, CONFIG_GEN4_THREAD_STACK_SIZE,
                    (k_thread_entry_t)gen4_thread, (void *)dev, 0, NULL,
                    K_PRIO_COOP(CONFIG_GEN4_THREAD_PRIORITY), 0, K_NO_WAIT);
#elif defined(CONFIG_GEN4_TRIGGER_GLOBAL_THREAD)
    k_work_init(&data->work, gen4_work_cb);
#endif
#endif
    LOG_WRN("inited");
    return 0;
}

static const struct sensor_driver_api gen4_driver_api = {
#if CONFIG_GEN4_TRIGGER
    .trigger_set = gen4_trigger_set,
#endif
    .sample_fetch = gen4_sample_fetch,
    .channel_get = gen4_channel_get,
};

#define GEN4_INST(n)                                                                               \
    static struct gen4_data gen4_data_##n;                                                         \
    static const struct gen4_config gen4_config_##n = {                                            \
        .bus = I2C_DT_SPEC_INST_GET(n),                                                            \
        .rotate_90 = DT_INST_PROP(0, rotate_90),                                                   \
        .sleep_en = DT_INST_PROP(0, sleep),                                                        \
        .no_taps = DT_INST_PROP(0, no_taps),                                                       \
        COND_CODE_1(CONFIG_GEN4_TRIGGER, (.dr = GPIO_DT_SPEC_GET(DT_DRV_INST(0), dr_gpios), ),     \
                    ())};                                                                          \
    DEVICE_DT_INST_DEFINE(n, gen4_init, NULL, &gen4_data_##n, &gen4_config_##n, POST_KERNEL,       \
                          CONFIG_SENSOR_INIT_PRIORITY, &gen4_driver_api);

DT_INST_FOREACH_STATUS_OKAY(GEN4_INST)