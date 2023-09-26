/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zmk/keys.h>
#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>

#define ZMK_HID_KEYBOARD_NKRO_MAX_USAGE HID_USAGE_KEY_KEYPAD_EQUAL

#define COLLECTION_REPORT 0x03

#define ZMK_REPORT_ID_KEYBOARD 0x01
#define ZMK_REPORT_ID_CONSUMER 0x02

#define ZMK_REPORT_ID_TRACKPAD 0x05
#define ZMK_REPORT_ID_FEATURE_PTP_CAPABILITIES 0x06
#define ZMK_REPORT_ID_FEATURE_PTPHQA 0x07
#define ZMK_REPORT_ID_FEATURE_PTP_CONFIGURATION 0x08
#define ZMK_REPORT_ID_FEATURE_PTP_SELECTIVE 0x09

static const uint8_t zmk_hid_report_desc[] = {
    HID_USAGE_PAGE(HID_USAGE_GEN_DESKTOP),
    HID_USAGE(HID_USAGE_GD_KEYBOARD),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    HID_REPORT_ID(ZMK_REPORT_ID_KEYBOARD),
    HID_USAGE_PAGE(HID_USAGE_KEY),
    HID_USAGE_MIN8(HID_USAGE_KEY_KEYBOARD_LEFTCONTROL),
    HID_USAGE_MAX8(HID_USAGE_KEY_KEYBOARD_RIGHT_GUI),
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX8(0x01),

    HID_REPORT_SIZE(0x01),
    HID_REPORT_COUNT(0x08),
    /* INPUT (Data,Var,Abs) */
    HID_INPUT(0x02),

    HID_USAGE_PAGE(HID_USAGE_KEY),
    HID_REPORT_SIZE(0x08),
    HID_REPORT_COUNT(0x01),
    /* INPUT (Cnst,Var,Abs) */
    HID_INPUT(0x03),

    HID_USAGE_PAGE(HID_USAGE_KEY),

#if IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_NKRO)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX8(0x01),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX8(ZMK_HID_KEYBOARD_NKRO_MAX_USAGE),
    HID_REPORT_SIZE(0x01),
    HID_REPORT_COUNT(ZMK_HID_KEYBOARD_NKRO_MAX_USAGE + 1),
    /* INPUT (Data,Ary,Abs) */
    HID_INPUT(0x02),
#elif IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_HKRO)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x00),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX8(0xFF),
    HID_REPORT_SIZE(0x08),
    HID_REPORT_COUNT(CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE),
    /* INPUT (Data,Ary,Abs) */
    HID_INPUT(0x00),
#else
#error "A proper HID report type must be selected"
#endif

    HID_END_COLLECTION,
    HID_USAGE_PAGE(HID_USAGE_CONSUMER),
    HID_USAGE(HID_USAGE_CONSUMER_CONSUMER_CONTROL),
    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    HID_REPORT_ID(ZMK_REPORT_ID_CONSUMER),
    HID_USAGE_PAGE(HID_USAGE_CONSUMER),

#if IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x00),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX8(0xFF),
    HID_REPORT_SIZE(0x08),
#elif IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL)
    HID_LOGICAL_MIN8(0x00),
    HID_LOGICAL_MAX16(0xFF, 0x0F),
    HID_USAGE_MIN8(0x00),
    HID_USAGE_MAX16(0xFF, 0x0F),
    HID_REPORT_SIZE(0x10),
#else
#error "A proper consumer HID report usage range must be selected"
#endif
    HID_REPORT_COUNT(CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE),
    /* INPUT (Data,Ary,Abs) */
    HID_INPUT(0x00),
    HID_END_COLLECTION,
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
    //  PTP Touchpad HID with inspiration from osmakari
    /* USAGE_PAGE (Digitizers) */
    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),
    /* USAGE (Touch Pad) */
    HID_USAGE(HID_USAGE_DIGITIZERS_TOUCH_PAD),
    /* COLLECTION (Application) */
    HID_COLLECTION(HID_COLLECTION_APPLICATION),

    /* Windows Precision Touchpad Input Reports */

    /* REPORT_ID (0x05) */
    HID_REPORT_ID(ZMK_REPORT_ID_TRACKPAD),
    /* USAGE (Finger) */
    HID_USAGE(HID_USAGE_DIGITIZERS_FINGER),
    /* COLLECTION (Logical) */
    HID_COLLECTION(HID_COLLECTION_LOGICAL),
    /* LOGICAL_MINIMUM (0) */
    HID_LOGICAL_MIN8(0),
    /* LOGICAL_MAXIMUM (1) */
    HID_LOGICAL_MAX8(1),
    /* USAGE (Confidence) */
    HID_USAGE(0x47),
    /* USAGE (Tip switch) */
    HID_USAGE(0x42),
    /* REPORT_COUNT (2) */
    HID_REPORT_COUNT(2),
    /* REPORT_SIZE (1) */
    HID_REPORT_SIZE(1),
    /* INPUT (Data, Var, Abs) */
    HID_INPUT(0x02),
    // Padding for bytes
    HID_REPORT_COUNT(6),
    /* INPUT (Cnst,Var,Abs) */
    HID_INPUT(0x03),
    /* REPORT_COUNT (1) */
    HID_REPORT_COUNT(1),
    /* REPORT_SIZE (3) */
    HID_REPORT_SIZE(3),
    /* LOGICAL_MAXIMUM (24 */
    HID_LOGICAL_MAX8(5),
    /* USAGE (Contact Identifier) */
    HID_USAGE(HID_USAGE_DIGITIZERS_CONTACT_IDENTIFIER),
    /* INPUT (Data, Var, Abs) */
    HID_INPUT(0x02),
    /* REPORT_SIZE (1) */
    HID_REPORT_SIZE(1),
    /* REPORT_COUNT (byte padding) */
    HID_REPORT_COUNT(5),
    /* INPUT (Cnst,Var,Abs) */
    HID_INPUT(0x03),
    /* USAGE_PAGE(Generic Desktop) */
    HID_USAGE_PAGE(HID_USAGE_GD),
    /* LOGICAL_MINIMUM (0) */
    HID_LOGICAL_MIN8(0),
    /* LOGICAL_MAXIMUM (defined inkconfic) */
    HID_LOGICAL_MAX16((CONFIG_ZMK_TRACKPAD_LOGICAL_X & 0xFF),
                      ((CONFIG_ZMK_TRACKPAD_LOGICAL_X >> 8) & 0xFF)),
    /* REPORT_SIZE (16) */
    HID_REPORT_SIZE(16),
    /* UNIT_EXPONENT (-2) */
    0x55,
    0x0e,
    /* UNIT (CM, EngLinear) */
    0x65,
    0x11,
    /* USAGE (X) */
    HID_USAGE(HID_USAGE_GD_X),
    /* PHYSICAL_MINIMUM (0) */
    0x35,
    0x00,
    /* PHYSICAL_MAXIMUM (defined in config) */
    0x46,
    (CONFIG_ZMK_TRACKPAD_PHYSICAL_X & 0xFF),
    ((CONFIG_ZMK_TRACKPAD_PHYSICAL_X >> 8) & 0xFF),
    /* REPORT_COUNT (1) */
    HID_REPORT_COUNT(1),
    /* INPUT (Data, Var, Abs) */
    HID_INPUT(0x02),
    // Logimax
    HID_LOGICAL_MAX16((CONFIG_ZMK_TRACKPAD_LOGICAL_Y & 0xFF),
                      ((CONFIG_ZMK_TRACKPAD_LOGICAL_Y >> 8) & 0xFF)),
    /* PHYSICAL_MAXIMUM (Defined in config) */
    0x46,
    (CONFIG_ZMK_TRACKPAD_PHYSICAL_Y & 0xFF),
    ((CONFIG_ZMK_TRACKPAD_PHYSICAL_Y >> 8) & 0xFF),
    /* USAGE (Y) */
    HID_USAGE(HID_USAGE_GD_Y),
    /* INPUT (Data, Var, Abs) */
    HID_INPUT(0x02),
    /* END_COLLECTION */
    HID_END_COLLECTION,
    /* USAGE_PAGE (Digitizers) */
    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),
    // Usage scan time
    HID_USAGE(HID_USAGE_DIGITIZERS_SCAN_TIME),
    //
    0x55,
    0x0C, //    UNIT_EXPONENT (-4)
    0x66,
    0x01,
    0x10, //    UNIT (Seconds)
    0x47,
    0xff,
    0xff,
    0x00,
    0x00, //     PHYSICAL_MAXIMUM (65535)
    0x27,
    0xff,
    0xff,
    0x00,
    0x00, // Logical Maximum
    HID_REPORT_SIZE(16),
    HID_REPORT_COUNT(1),

    HID_INPUT(0x02),
    // Physmax 0
    0x45,
    0x00,
    /* USAGE (Contact count) */
    HID_USAGE(HID_USAGE_DIGITIZERS_CONTACT_COUNT),
    /* LOGICAL_MAXIMUM (5) */
    HID_LOGICAL_MAX8(0x05),
    /* REPORT_COUNT (1) */
    HID_REPORT_COUNT(1),
    /* REPORT_SIZE (8) */
    HID_REPORT_SIZE(8),
    /* INPUT(Data, Var, Abs) */
    HID_INPUT(0x02),

    // Button report
    HID_USAGE_PAGE(HID_USAGE_GEN_BUTTON),
    /* USAGE (Button 1) */
    HID_USAGE(0x01),
    HID_USAGE(0x02),
    HID_USAGE(0x03), /* LOGICAL_MAXIMUM (1) */
    HID_LOGICAL_MAX8(1),
    /* REPORT_SIZE (1) */
    HID_REPORT_SIZE(1),
    /* REPORT_COUNT (1) */
    HID_REPORT_COUNT(3),
    /* INPUT (Data, Var, Abs) */
    HID_INPUT(0x02),
    /* REPORT_SIZE (1) */
    HID_REPORT_SIZE(1),
    /* REPORT_COUNT (byte padding) */
    HID_REPORT_COUNT(5),
    /* INPUT (Cnst,Var,Abs) */
    HID_INPUT(0x03),

    /* Device Capabilities Feature Report */

    /* USAGE_PAGE (Digitizer) */
    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),
    /* REPORT_ID (0x07) */
    HID_REPORT_ID(ZMK_REPORT_ID_FEATURE_PTP_CAPABILITIES),
    /* USAGE (Contact Count Maximum) */
    HID_USAGE(HID_USAGE_DIGITIZERS_CONTACT_COUNT_MAXIMUM),
    HID_REPORT_SIZE(8),
    HID_REPORT_COUNT(1),
    HID_LOGICAL_MAX8(0x05),
    HID_FEATURE(0x02),
    /* USAGE (Pad Type) */
    HID_USAGE(HID_USAGE_DIGITIZERS_PAD_TYPE),
    /* FEATURE (Data, Var, Abs) */
    HID_FEATURE(0x02),

    /* PTPHQA Blob: Necessary for < Windows 10 */

    /* USAGE_PAGE (Vendor Defined) */
    0x06,
    0x00,
    0xff,
    /* REPORT_ID (0x08) */
    HID_REPORT_ID(ZMK_REPORT_ID_FEATURE_PTPHQA),
    /* HID_USAGE (Vendor Usage 0xC5) */
    HID_USAGE(0xC5),
    /* LOGICAL_MINIMUM (0) */
    HID_LOGICAL_MIN8(0),
    /* LOGICAL_MAXIMUM (0xFF) */
    HID_LOGICAL_MAX16(0xFF, 0x00),
    /* REPORT_SIZE (8) */
    HID_REPORT_SIZE(8),
    /* REPORT_COUNT (256) */
    0x96,
    0x00,
    0x01,
    /* FEATURE (Data, Var, Abs) */
    HID_FEATURE(0x02),
    /* END_COLLECTION */
    HID_END_COLLECTION,

    HID_USAGE_PAGE(HID_USAGE_DIGITIZERS),

    HID_USAGE(HID_USAGE_DIGITIZERS_DEVICE_CONFIGURATION),

    HID_COLLECTION(HID_COLLECTION_APPLICATION),
    /* USAGE (Finger) */
    HID_USAGE(HID_USAGE_DIGITIZERS_FINGER),
    /* COLLECTION (Physical) */
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),
    /* REPORT_ID (Feature, 0x0A) */
    HID_REPORT_ID(ZMK_REPORT_ID_FEATURE_PTP_SELECTIVE),
    /* USAGE (Surface switch) */
    HID_USAGE(HID_USAGE_DIGITIZERS_SURFACE_SWITCH),
    /* USAGE (Button switch) */
    HID_USAGE(HID_USAGE_DIGITIZERS_BUTTON_SWITCH),
    /* REPORT_SIZE (1) */
    HID_REPORT_SIZE(1),
    /* REPORT_COUNT (2) */
    HID_REPORT_COUNT(2),
    /* LOGICAL_MAXIMUM (1) */
    HID_LOGICAL_MIN8(0),
    HID_LOGICAL_MAX8(1),
    /* FEATURE (Data, Var, Abs) */
    HID_FEATURE(0x02),
    /* REPORT_COUNT (6) */
    HID_REPORT_COUNT(6),
    /* FEATURE (Cnst, Var, Abs) */
    HID_FEATURE(0x03),
    /* END_COLLECTION */
    HID_END_COLLECTION,
    HID_END_COLLECTION,
#endif

};

// struct zmk_hid_boot_report
// {
//     uint8_t modifiers;
//     uint8_t _unused;
//     uint8_t keys[6];
// } __packed;

struct zmk_hid_keyboard_report_body {
    zmk_mod_flags_t modifiers;
    uint8_t _reserved;
#if IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_NKRO)
    uint8_t keys[(ZMK_HID_KEYBOARD_NKRO_MAX_USAGE + 1) / 8];
#elif IS_ENABLED(CONFIG_ZMK_HID_REPORT_TYPE_HKRO)
    uint8_t keys[CONFIG_ZMK_HID_KEYBOARD_REPORT_SIZE];
#endif
} __packed;

struct zmk_hid_keyboard_report {
    uint8_t report_id;
    struct zmk_hid_keyboard_report_body body;
} __packed;

struct zmk_hid_consumer_report_body {
#if IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_BASIC)
    uint8_t keys[CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE];
#elif IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL)
    uint16_t keys[CONFIG_ZMK_HID_CONSUMER_REPORT_SIZE];
#endif
} __packed;

struct zmk_hid_consumer_report {
    uint8_t report_id;
    struct zmk_hid_consumer_report_body body;
} __packed;

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
// Reporting structure from osmakari
// Report for single finger
struct zmk_ptp_finger {
    // Confidence (bit 0) and tip switch (bit 1)
    uint8_t confidence_tip;
    // Contact ID
    uint8_t contact_id;
    // X
    uint16_t x;
    // Y
    uint16_t y;
} __packed;

struct zmk_hid_ptp_report_body {
    // Finger reporting
    struct zmk_ptp_finger finger;
    // scantime
    uint16_t scan_time;
    // Contact count
    uint8_t contact_count;
    // Buttons /surfaceswitch
    uint8_t buttons;

} __packed;

// Report containing finger data
struct zmk_hid_ptp_report {
    // 0x05
    uint8_t report_id;
    struct zmk_hid_ptp_report_body body;

} __packed;

// Feature report for configuration
struct zmk_hid_ptp_feature_selective_report {
    uint8_t report_id;
    // Selective reporting: Surface switch (bit 0), Button switch (bit 1)
    uint8_t selective_reporting;
} __packed;

// Feature report for certification
struct zmk_hid_ptp_feature_certification_report {
    uint8_t report_id;

    uint8_t ptphqa_blob[256];
} __packed;

#define PTP_PAD_TYPE_DEPRESSIBLE 0x00
#define PTP_PAD_TYPE_PRESSURE 0x01
#define PTP_PAD_TYPE_NON_CLICKABLE 0x02

// Feature report for device capabilities
struct zmk_hid_ptp_feature_capabilities_report {
    uint8_t report_id;
    // Max touches (L 4bit) and pad type (H 4bit):
    // Max touches: number 3-5
    // Pad type:    0 for Depressible, 1 for Non-depressible, 2 for Non-clickable
    uint8_t max_touches;
    uint8_t pad_type;

} __packed;

#endif

zmk_mod_flags_t zmk_hid_get_explicit_mods();
int zmk_hid_register_mod(zmk_mod_t modifier);
int zmk_hid_unregister_mod(zmk_mod_t modifier);
bool zmk_hid_mod_is_pressed(zmk_mod_t modifier);

int zmk_hid_register_mods(zmk_mod_flags_t explicit_modifiers);
int zmk_hid_unregister_mods(zmk_mod_flags_t explicit_modifiers);
int zmk_hid_implicit_modifiers_press(zmk_mod_flags_t implicit_modifiers);
int zmk_hid_implicit_modifiers_release();
int zmk_hid_masked_modifiers_set(zmk_mod_flags_t masked_modifiers);
int zmk_hid_masked_modifiers_clear();

int zmk_hid_keyboard_press(zmk_key_t key);
int zmk_hid_keyboard_release(zmk_key_t key);
void zmk_hid_keyboard_clear();
bool zmk_hid_keyboard_is_pressed(zmk_key_t key);

int zmk_hid_consumer_press(zmk_key_t key);
int zmk_hid_consumer_release(zmk_key_t key);
void zmk_hid_consumer_clear();
bool zmk_hid_consumer_is_pressed(zmk_key_t key);

int zmk_hid_press(uint32_t usage);
int zmk_hid_release(uint32_t usage);
bool zmk_hid_is_pressed(uint32_t usage);

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
void zmk_hid_ptp_set(struct zmk_ptp_finger finger, uint8_t contact_count, uint16_t scan_time,
                     uint8_t buttons);
#endif

struct zmk_hid_keyboard_report *zmk_hid_get_keyboard_report();
struct zmk_hid_consumer_report *zmk_hid_get_consumer_report();
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
struct zmk_hid_ptp_report *zmk_hid_get_ptp_report();
struct zmk_hid_ptp_feature_selective_report *zmk_hid_ptp_get_feature_selective_report();
void zmk_hid_ptp_set_feature_selective_report(uint8_t selective_report);
struct zmk_hid_ptp_feature_certification_report *zmk_hid_ptp_get_feature_certification_report();
struct zmk_hid_ptp_feature_capabilities_report *zmk_hid_ptp_get_feature_capabilities_report();
#endif
