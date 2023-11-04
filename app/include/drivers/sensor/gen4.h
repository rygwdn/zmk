#ifndef ZEPHYR_INCLUDE_DRIVERS_SENSOR_GEN4_H_
#define ZEPHYR_INCLUDE_DRIVERS_SENSOR_GEN4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/drivers/sensor.h>

enum sensor_channel_gen4 {
    SENSOR_CHAN_BUTTONS = SENSOR_CHAN_PRIV_START,
    SENSOR_CHAN_WHEEL,
    SENSOR_CHAN_XDELTA,
    SENSOR_CHAN_YDELTA,
};

#ifdef __cplusplus
}
#endif

#endif