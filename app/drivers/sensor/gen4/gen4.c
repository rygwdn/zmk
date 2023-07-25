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

static uint8_t gen4_checksum(uint8_t *buffer, uint8_t length) {
    uint16_t temp;
    uint8_t checksum = 0;
    for (temp = 0; temp < length; temp++)
        checksum += (*(buffer + temp) & 0xFF);
    return checksum;
}

static int gen4_ext_read(const struct device *dev, const uint16_t addr, const uint8_t len,
                         uint8_t *buffer) {
    const struct gen4_config *cfg = dev->config;
    uint8_t request[8] = {(uint8_t)(GEN4_EXT_ACCESS_READ & 0xFF),
                          (uint8_t)((GEN4_EXT_ACCESS_READ >> 8) & 0xFF),
                          (uint8_t)(addr & 0xFF),
                          (uint8_t)((addr >> 8) & 0xFF),
                          0,
                          0,
                          (uint8_t)(len & 0xFF),
                          (uint8_t)((len >> 8) & 0xFF)};
    int ret = i2c_write_read_dt(&cfg->bus, request, 8, buffer, (len + 3));
    if (ret < 0) {
        LOG_ERR("ext read status: %d", ret);
        return ret;
    }
    LOG_DBG("received value %x", buffer[2]);
    uint8_t checksum = gen4_checksum(buffer, len + 2);
    if (checksum != *(buffer + len + 2)) {
        LOG_ERR("checksum failed: %d", (int)checksum);
        // return 1;
    }
    return 0;
}

static int gen4_ext_write_1byte(const struct device *dev, const uint16_t addr, const uint8_t val) {
    const struct gen4_config *cfg = dev->config;

    uint8_t sendbuf[10];

    sendbuf[0] = (uint8_t)(GEN4_EXT_ACCESS_WRITE & 0xFF);
    sendbuf[1] = (uint8_t)((GEN4_EXT_ACCESS_WRITE >> 8) & 0xFF);
    sendbuf[2] = (uint8_t)(addr & 0xFF);
    sendbuf[3] = (uint8_t)((addr >> 8) & 0xFF);
    sendbuf[4] = 0;
    sendbuf[5] = 0;
    sendbuf[6] = (uint8_t)(1 & 0xFF);
    sendbuf[7] = 0;
    sendbuf[8] = val;
    sendbuf[9] = gen4_checksum(sendbuf, 9);

    int ret = i2c_write_dt(&cfg->bus, sendbuf, 10);
    if (ret < 0) {
        LOG_ERR("ext read status: %d", ret);
        return ret;
    }
    return ret;
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
    uint8_t packet[52];
    int ret;
    ret = gen4_normal_read(dev, packet, 52);
    if (ret < 0) {
        LOG_ERR("read status: %d", ret);
        return ret;
    }
    if (!(packet[GEN4_REPORT_ID_SHIFT] == 9)) {
        return -EAGAIN;
    }
    struct gen4_data *data = dev->data;
    data->btns = packet[GEN4_BTNS_SHIFT];
    data->contacts = packet[GEN4_NUM_CONTACT_SHIFT];
    // Finger 0
    if (data->contacts & BIT(0)) {
        data->fingers[0].present = packet[GEN4_F0_PALM_SHIFT] & BIT(1);
        data->fingers[0].palm = packet[GEN4_F0_PALM_SHIFT] & BIT(7);
        data->fingers[0].x =
            (uint16_t)(packet[GEN4_F0_X_HI_SHIFT] << 8) + packet[GEN4_F0_X_LO_SHIFT];
        data->fingers[0].y =
            (uint16_t)(packet[GEN4_F0_Y_HI_SHIFT] << 8) + packet[GEN4_F0_Y_LO_SHIFT];
        LOG_DBG("Finger 0 detected: %d", data->fingers[0].present);
        LOG_DBG("Finger 0 palm: %d", data->fingers[0].palm);
        LOG_DBG("Finger 0 x: %d", data->fingers[0].x);
        LOG_DBG("Finger 0 y: %d", data->fingers[0].y);

    } else {
        data->fingers[0].present = false;
        LOG_DBG("Finger 0 not detected: %d", data->fingers[0].present);
    }
    // Finger 1
    if (data->contacts & BIT(1)) {
        data->fingers[1].present = packet[GEN4_F1_PALM_SHIFT] & BIT(1);
        data->fingers[1].palm = packet[GEN4_F1_PALM_SHIFT] & BIT(7);
        data->fingers[1].x =
            (uint16_t)(packet[GEN4_F1_X_HI_SHIFT] << 8) + packet[GEN4_F1_X_LO_SHIFT];
        data->fingers[1].y =
            (uint16_t)(packet[GEN4_F1_Y_HI_SHIFT] << 8) + packet[GEN4_F1_Y_LO_SHIFT];
    } else {
        data->fingers[1].present = false;
    }
    // Finger 2
    if (data->contacts & BIT(2)) {
        data->fingers[2].present = packet[GEN4_F2_PALM_SHIFT] & BIT(1);
        data->fingers[2].palm = packet[GEN4_F2_PALM_SHIFT] & BIT(7);
        data->fingers[2].x =
            (uint16_t)(packet[GEN4_F2_X_HI_SHIFT] << 8) + packet[GEN4_F2_X_LO_SHIFT];
        data->fingers[2].y =
            (uint16_t)(packet[GEN4_F2_Y_HI_SHIFT] << 8) + packet[GEN4_F2_Y_LO_SHIFT];
    } else {
        data->fingers[2].present = false;
    }
    // Finger 3
    if (data->contacts & BIT(3)) {
        data->fingers[3].present = packet[GEN4_F3_PALM_SHIFT] & BIT(1);
        data->fingers[3].palm = packet[GEN4_F3_PALM_SHIFT] & BIT(7);
        data->fingers[3].x =
            (uint16_t)(packet[GEN4_F3_X_HI_SHIFT] << 8) + packet[GEN4_F3_X_LO_SHIFT];
        data->fingers[3].y =
            (uint16_t)(packet[GEN4_F3_Y_HI_SHIFT] << 8) + packet[GEN4_F3_Y_LO_SHIFT];
    } else {
        data->fingers[3].present = false;
    }
    // Finger 4
    if (data->contacts & BIT(4)) {
        data->fingers[4].present = packet[GEN4_F4_PALM_SHIFT] & BIT(1);
        data->fingers[4].palm = packet[GEN4_F4_PALM_SHIFT] & BIT(7);
        data->fingers[4].x =
            (uint16_t)(packet[GEN4_F4_X_HI_SHIFT] << 8) + packet[GEN4_F4_X_LO_SHIFT];
        data->fingers[4].y =
            (uint16_t)(packet[GEN4_F4_Y_HI_SHIFT] << 8) + packet[GEN4_F4_Y_LO_SHIFT];
    } else {
        data->fingers[4].present = false;
    }
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

    LOG_WRN("gen4 start");
    data->in_int = false;
    int ret;
    ret = gen4_ext_write_1byte(dev, GEN4_REG_RESETCONTROL, GEN4_RESET_CONTROL);
    if (ret < 0) {
        LOG_ERR("can't reset %d", ret);
        return ret;
    }
    k_msleep(60);
    uint8_t extcache[4] = {0};

    ret = gen4_ext_read(dev, GEN4_REG_SECURITYSTATUS, 1, extcache); // Read and pritn chip ID
    if (ret < 0) {
        LOG_ERR("can't read chip id %d", ret);
        return ret;
    } else
        LOG_DBG("Chip ID: %x", extcache[2]);

    ret = gen4_ext_read(dev, GEN4_REG_FEEDCONFIG_1, 1, extcache); // Read feedconfig 1
    if (ret < 0) {
        LOG_ERR("can't reaf feedconfig 1 %d", ret);
        return ret;
    }
    // Configure abs reporting
    extcache[2] |= GEN4_PRI_FEED_EN;
    extcache[2] |= GEN4_PRI_FEED_TYPE;
    k_usleep(50);

    ret = gen4_ext_write_1byte(dev, GEN4_REG_FEEDCONFIG_1, extcache[2]); // Write feedconfig 1
    if (ret < 0) {
        LOG_ERR("can't Write feedconfig 1 %d", ret);
        return ret;
    }
    k_usleep(50);
    ret = gen4_ext_read(dev, GEN4_REG_FEEDCONFIG_1, 1, extcache); // Read feedconfig 1
    if (ret < 0) {
        LOG_ERR("can't reaf feedconfig 1 %d", ret);
        return ret;
    }

    /*if (config->no_taps || config->rotate_90) {
        ret = gen4_ext_read(dev, GEN4_REG_FEEDCONFIG_2, 1, extcache); // Read feedconfig 2
        if (ret < 0) {
            LOG_ERR("can't reaf feedconfig 2 %d", ret);
            return ret;
        }
        // Configure abs reporting
        extcache[2] |= config->rotate_90 ? GEN4_SWAP_XY : 0;
        extcache[2] |= config->no_taps ? GEN4_DISABLE_BUTTONS : 0;
        k_usleep(50);

        ret = gen4_ext_write_1byte(dev, GEN4_REG_FEEDCONFIG_2, extcache[2]); // Write feedconfig
    2 if (ret < 0) { LOG_ERR("can't Write feedconfig 2 %d", ret); return ret;
        }
        k_usleep(50);
    }
    ret = gen4_ext_write_1byte(dev, GEN4_REG_ZIDLE, 1); // one Z-Idle packet
    if (ret < 0) {
        LOG_ERR("can't write %d", ret);
        return ret;
    }

    ret = gen4_ext_read(dev, GEN4_REG_POWERCONTROL, 1, extcache); // Read Powercontrol
    if (ret < 0) {
        LOG_ERR("can't read Powercontrol %d", ret);
        return ret;
    }
    // Configure deep sleep
    extcache[2] |= config->sleep_en ? GEN4_DEEP_SLEEP_EN : 0;
    k_usleep(50);

    ret = gen4_ext_write_1byte(dev, GEN4_REG_POWERCONTROL, extcache[2]); // Write Powercontrol
    if (ret < 0) {
        LOG_ERR("can't Write Powercontrol %d", ret);
        return ret;
    }
    k_usleep(50);
    */
    gen4_sample_fetch(dev, 0);
#ifdef CONFIG_GEN4_TRIGGER
    data->dev = dev;
    gpio_pin_configure_dt(&config->dr, GPIO_INPUT);
    gpio_init_callback(&data->gpio_cb, gen4_gpio_cb, BIT(config->dr.pin));
    ret = gpio_add_callback(config->dr.port, &data->gpio_cb);
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