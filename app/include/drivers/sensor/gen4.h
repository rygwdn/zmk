#ifndef ZEPHYR_INCLUDE_DRIVERS_SENSOR_GEN4_H_
#define ZEPHYR_INCLUDE_DRIVERS_SENSOR_GEN4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/drivers/sensor.h>

enum sensor_channel_gen4 {
    SENSOR_CHAN_FINGER = SENSOR_CHAN_PRIV_START,
    SENSOR_CHAN_X,
    SENSOR_CHAN_Y,
    SENSOR_CHAN_CONFIDENCE_TIP,
    SENSOR_CHAN_CONTACTS,
    SENSOR_CHAN_BUTTONS,
};

#ifdef __cplusplus
}
#endif

#endif