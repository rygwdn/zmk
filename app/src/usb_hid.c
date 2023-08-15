/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/device.h>
#include <zephyr/init.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/class/usb_hid.h>

#include <zmk/usb.h>
#include <zmk/usb_hid.h>
#include <zmk/hid.h>
#include <zmk/keymap.h>
#include <zmk/event_manager.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static const struct device *hid_dev;

static K_SEM_DEFINE(hid_sem, 1, 1);

static void in_ready_cb(const struct device *dev) { k_sem_give(&hid_sem); }

#define HID_GET_REPORT_TYPE_MASK 0xff00
#define HID_GET_REPORT_ID_MASK 0x00ff

#define HID_REPORT_TYPE_INPUT 0x100
#define HID_REPORT_TYPE_OUTPUT 0x200
#define HID_REPORT_TYPE_FEATURE 0x300

#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
static int set_report_cb(const struct device *dev, struct usb_setup_packet *setup, int32_t *len,
                         uint8_t **data) {
    if ((setup->wValue & HID_GET_REPORT_TYPE_MASK) != HID_REPORT_TYPE_FEATURE) {
        LOG_ERR("Unsupported report type %d requested",
                (setup->wValue & HID_GET_REPORT_TYPE_MASK) >> 8);
        return -ENOTSUP;
    }

    switch (setup->wValue & HID_GET_REPORT_ID_MASK) {
    case ZMK_REPORT_ID_FEATURE_PTP_SELECTIVE:
        if (*len != sizeof(struct zmk_hid_ptp_feature_selective_report)) {
            LOG_ERR("LED set report is malformed: length=%d", *len);
            return -EINVAL;
        } else {
            struct zmk_hid_ptp_feature_selective_report *report =
                (struct zmk_hid_ptp_feature_selective_report *)*data;
            zmk_hid_ptp_set_feature_selective_report(report->selective_reporting);
        }
        break;
    default:
        LOG_ERR("Invalid report ID %d requested", setup->wValue & HID_GET_REPORT_ID_MASK);
        return -EINVAL;
    }

    return 0;
}

static int get_report_cb(const struct device *dev, struct usb_setup_packet *setup, int32_t *len,
                         uint8_t **data) {
    if ((setup->wValue & HID_GET_REPORT_TYPE_MASK) != HID_REPORT_TYPE_FEATURE) {
        LOG_ERR("Unsupported report type %d requested",
                (setup->wValue & HID_GET_REPORT_TYPE_MASK) >> 8);
        return -ENOTSUP;
    }

    switch (setup->wValue & HID_GET_REPORT_ID_MASK) {
    case ZMK_REPORT_ID_FEATURE_PTP_SELECTIVE:

        struct zmk_hid_ptp_feature_selective_report *report1 =
            zmk_hid_ptp_get_feature_selective_report();
        int err1 = zmk_usb_hid_send_report((uint8_t *)report1, sizeof(*report1));
        if (err1) {
            LOG_ERR("FAILED TO SEND SELECTIVE OVER USB: %d", err1);
        }
        return err1;

        break;
    case ZMK_REPORT_ID_FEATURE_PTPHQA:

        struct zmk_hid_ptp_feature_certification_report *report2 =
            zmk_hid_ptp_get_feature_certification_report();
        int err2 = zmk_usb_hid_send_report((uint8_t *)report2, sizeof(*report2));
        if (err2) {
            LOG_ERR("FAILED TO SEND CERTIFICATION OVER USB: %d", err2);
        }
        return err2;

        break;
    case ZMK_REPORT_ID_FEATURE_PTP_CAPABILITIES:

        struct zmk_hid_ptp_feature_capabilities_report *report3 =
            zmk_hid_ptp_get_feature_capabilities_report();
        int err3 = zmk_usb_hid_send_report((uint8_t *)report3, sizeof(*report3));
        if (err3) {
            LOG_ERR("FAILED TO SEND CAPABILITIES OVER USB: %d", err3);
        }
        return err3;

        break;
    default:
        LOG_ERR("Invalid report ID %d requested", setup->wValue & HID_GET_REPORT_ID_MASK);
        return -EINVAL;
    }

    return 0;
}
#endif

static const struct hid_ops ops = {
    .int_in_ready = in_ready_cb,
#if IS_ENABLED(CONFIG_ZMK_TRACKPAD)
    .set_report = set_report_cb,
    .get_report = get_report_cb,
#endif
};

int zmk_usb_hid_send_report(const uint8_t *report, size_t len) {
    switch (zmk_usb_get_status()) {
    case USB_DC_SUSPEND:
        return usb_wakeup_request();
    case USB_DC_ERROR:
    case USB_DC_RESET:
    case USB_DC_DISCONNECTED:
    case USB_DC_UNKNOWN:
        return -ENODEV;
    default:
        k_sem_take(&hid_sem, K_MSEC(30));
        int err = hid_int_ep_write(hid_dev, report, len, NULL);

        if (err) {
            k_sem_give(&hid_sem);
        }

        return err;
    }
}

static int zmk_usb_hid_init(const struct device *_arg) {
    hid_dev = device_get_binding("HID_0");
    if (hid_dev == NULL) {
        LOG_ERR("Unable to locate HID device");
        return -EINVAL;
    }

    usb_hid_register_device(hid_dev, zmk_hid_report_desc, sizeof(zmk_hid_report_desc), &ops);
    usb_hid_init(hid_dev);

    return 0;
}

SYS_INIT(zmk_usb_hid_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
