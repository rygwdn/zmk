#pragma once

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>

#define GEN4_REG_CHIP_ID 0xC2C0
#define GEN4_REG_FW_VER 0xC2C1
#define GEN4_REG_SYSCONFIG_1 0xC2C2
#define GEN4_REG_SYSCONFIG_2 0xC2C3
#define GEN4_REG_FEEDCONFIG_1 0xC2C4
#define GEN4_REG_FEEDCONFIG_2 0xC2C5
#define GEN4_REG_FEEDCONFIG_3 0xC2C6
#define GEN4_REG_COMPCONFIG_1 0xC2C7
#define GEN4_REG_PS2_AUXCONTROL 0xC2C8
#define GEN4_REG_SAMPLERATE 0xC2C9
#define GEN4_REG_ZIDLE 0xC2CA
#define GEN4_REG_FILTERCONTROL 0xC2CB
#define GEN4_REG_POWERCONTROL 0xC2CE
#define GEN4_REG_RESETCONTROL 0xC2CF
#define GEN4_REG_SECURITYSTATUS 0xC418

// Normal read packet
#define GEN4_LEN_LO_SHIFT 0
#define GEN4_LEN_HI_SHIFT 1
#define GEN4_REPORT_ID_SHIFT 2
#define GEN4_NUM_CONTACT_SHIFT 3
#define GEN4_F0_PALM_SHIFT 4
#define GEN4_F0_X_LO_SHIFT 5
#define GEN4_F0_X_HI_SHIFT 6
#define GEN4_F0_Y_LO_SHIFT 7
#define GEN4_F0_Y_HI_SHIFT 8
#define GEN4_F1_PALM_SHIFT 9
#define GEN4_F1_X_LO_SHIFT 10
#define GEN4_F1_X_HI_SHIFT 11
#define GEN4_F1_Y_LO_SHIFT 12
#define GEN4_F1_Y_HI_SHIFT 13
#define GEN4_F2_PALM_SHIFT 14
#define GEN4_F2_X_LO_SHIFT 15
#define GEN4_F2_X_HI_SHIFT 16
#define GEN4_F2_Y_LO_SHIFT 17
#define GEN4_F2_Y_HI_SHIFT 18
#define GEN4_F3_PALM_SHIFT 19
#define GEN4_F3_X_LO_SHIFT 20
#define GEN4_F3_X_HI_SHIFT 21
#define GEN4_F3_Y_LO_SHIFT 22
#define GEN4_F3_Y_HI_SHIFT 23
#define GEN4_F4_PALM_SHIFT 24
#define GEN4_F4_X_LO_SHIFT 25
#define GEN4_F4_X_HI_SHIFT 26
#define GEN4_F4_Y_LO_SHIFT 27
#define GEN4_F4_Y_HI_SHIFT 28
#define GEN4_BTNS_SHIFT 29

// Sysconfig 1
#define GEN4_PS2_MODE BIT(0)
#define GEN4_TRACKING_EN BIT(1)
#define GEN4_ANY_MEAS_EN BIT(2)
#define GEN4_ASSERT_DR_POR BIT(3)
#define GEN4_INTERFACE_PRIORITY_PRI BIT(4)
#define GEN4_INTERFACE_PRIORITY_SEC BIT(5)
#define GEN4_RANDOMISE_DRIVE BIT(7)

// Sysconfig 2
#define GEN4_NO_INTELLIMOUSE BIT(0)
#define GEN4_UPSET_REC_EN BIT(1)
#define GEN4_DOUBLE_MEAS_RES BIT(2)
#define GEN4_RES_SCALER_EN BIT(3)
#define GEN4_GEN_PEAK_DATA BIT(4)
#define GEN4_DISABLE_WATCHDOG BIT(5)
#define GEN4_TRISTATE_UNMUXED_SENSE BIT(6)
#define GEN4_EXECUTE_EF_AUTOTUNE BIT(7)

// Feedconfig 1
#define GEN4_PRI_FEED_EN BIT(0)
#define GEN4_PRI_FEED_TYPE BIT(1)
#define GEN4_ABS_XYZZ_EN BIT(2)
#define GEN4_SEC_FEED_TYPE BIT(4)
#define GEN4_LOCK_DATA_PORT BIT(5)
#define GEN4_LOCK_DATA_PORT_SEL BIT(6)
#define GEN4_CAL BIT(7)

// Feedconfig 2
#define GEN4_INV_X BIT(0)
#define GEN4_INV_Y BIT(1)
#define GEN4_SWAP_XY BIT(2)
#define GEN4_Y_PAN_INV BIT(3)
#define GEN4_DISABLE_COORD BIT(4)
#define GEN4_DISABLE_BUTTONS BIT(5)
#define GEN4_KEYPAD_AVAILABLE BIT(6)
#define GEN4_KEYPAD_EN BIT(7)

// Feedconfig 3
#define GEN4_LINEAR_CORR_EN BIT(0)
#define GEN4_EDGE_STAB_DISABLE BIT(1)
#define GEN4_DELTA_ACCELERATION_EN BIT(2)
#define GEN4_NOISE_AVOIDANCE_EN BIT(3)
#define GEN4_MULTI_FINGER_FRAME BIT(4)
#define GEN4_CLIP_SCALAR_BOUNDARIES BIT(5)
#define GEN4_DISCARD_NOISY_DATA BIT(6)
#define GEN4_EN_INTELLIMOUSE BIT(7)

// Compconfig 1
#define GEN4_BIST_RUN BIT(0)
#define GEN4_OFFSET_COMP_EN BIT(1)
#define GEN4_BACKGROUND_COMP_EN BIT(2)
#define GEN4_SIG_SIGN_COMP_EN BIT(3)
#define GEN4_CONGRUENCE_COMP_EN BIT(4)
#define GEN4_POS_COMP_EN BIT(5)
#define GEN4_BIST_PASS BIT(6)
#define GEN4_NFC_ACTIVE BIT(7)

// Filtercontrol
#define GEN4_FILTER_JITTER_EN BIT(0)
#define GEN4_FILTER_LEAST_CHANGE_EN BIT(1)
#define GEN4_FILTER_SMOOTHING_EN BIT(2)
#define GEN4_FILTER_ADAPTIVE_EXP_EN BIT(3)
#define GEN4_FILTER_ONLY_MULTI_FINGER_EN BIT(4)
#define GEN4_FILTER_HOVER_EN BIT(5)
#define GEN4_FILTER_SWARM_EN BIT(6)

// Powercontrol
#define GEN4_DEEP_SLEEP_EN BIT(0)
#define GEN4_BUTTON_WAKE_EN BIT(1)
#define GEN4_SLEEP_WHEN_DISABLED BIT(2)
#define GEN4_COMP_ON_WAKE BIT(3)
#define GEN4_DISABLE_ST2_SLEEP BIT(4)
#define GEN4_DISABLE_PS2_IN_SLEEP BIT(5)

// Resetcontrol
#define GEN4_RESET_CONTROL BIT(7)

#define GEN4_EXT_ACCESS_WRITE 0x0900
#define GEN4_EXT_ACCESS_READ 0x0901

#define GEN4_ADDRESS 0x2A

struct gen4_finger_data {
    bool present;
    bool palm;
    uint16_t x, y;
};

struct gen4_data {
    uint8_t contacts;
    uint8_t btns;
    struct gen4_finger_data fingers[5];
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
    bool rotate_90, sleep_en, no_taps;
#ifdef CONFIG_GEN4_TRIGGER
    const struct gpio_dt_spec dr;
#endif
};
