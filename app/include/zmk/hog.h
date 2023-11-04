/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zmk/keys.h>
#include <zmk/hid.h>

int zmk_hog_init();

int zmk_hog_send_keyboard_report(struct zmk_hid_keyboard_report_body *body);
int zmk_hog_send_consumer_report(struct zmk_hid_consumer_report_body *body);
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
int zmk_hog_send_touchpad_mouse_report(struct zmk_hid_touchpad_mouse_report_body *body);
int zmk_hog_send_touchpad_mouse_report_direct(struct zmk_hid_touchpad_mouse_report_body *report);
#endif
