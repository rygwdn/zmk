/*
 * Copyright (c) 2023 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>

struct zmk_trackpad_mouse_data_t {
    uint8_t buttons;
    int8_t xDelta;
    int8_t yDelta;
    int8_t scrollDelta;
};

struct k_work_q *zmk_trackpad_work_q();
