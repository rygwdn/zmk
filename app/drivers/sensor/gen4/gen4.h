#pragma once

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>

#define LENGTH_LOWBYTE_SHIFT 0
#define LENGTH_HIGHBYTE_SHIFT 1
#define REPORT_ID_SHIFT 2

#define PTP_REPORT_ID (0x01)

#define GEN4_ADDRESS 0x2C

struct gen4_finger_data {
    uint8_t confidence_tip;
    uint16_t x, y;
};

struct gen4_data {
    uint8_t contacts, btns, finger_id;
    struct gen4_finger_data finger;
    bool in_int;
#ifdef CONFIG_GEN4_TRIGGER
    const struct device *dev;
    const struct sensor_trigger *data_ready_trigger;
    struct gpio_callback gpio_cb;
    sensor_trigger_handler_t data_ready_handler;
#if defined(CONFIG_GEN4_TRIGGER_OWN_THREAD)
    K_THREAD_STACK_MEMBER(thread_stack, CONFIG_GEN4_THREAD_STACK_SIZE);
    struct k_sem gpio_sem;
    struct k_thread thread;
#elif defined(CONFIG_GEN4_TRIGGER_GLOBAL_THREAD)
    struct k_work work;
#endif
#endif
};

struct gen4_config {
#if DT_INST_ON_BUS(0, i2c)
    const struct i2c_dt_spec bus;
#endif
    bool rotate_90;
#ifdef CONFIG_GEN4_TRIGGER
    const struct gpio_dt_spec dr;
#endif
};
