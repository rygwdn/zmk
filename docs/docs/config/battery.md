---
title: Battery Level
sidebar_label: Battery Level
---

See the [battery level feature page](../features/battery.md) for more details on configuring a battery sensor.

See [Configuration Overview](index.md) for instructions on how to change these settings.

### Kconfig

Definition file: [zmk/app/Kconfig](https://github.com/zmkfirmware/zmk/blob/main/app/Kconfig)

| Config                               | Type | Description                                            | Default |
| ------------------------------------ | ---- | ------------------------------------------------------ | ------- |
| `CONFIG_ZMK_BATTERY_REPORTING`       | bool | Enables/disables all battery level detection/reporting | n       |
| `CONFIG_ZMK_BATTERY_REPORT_INTERVAL` | int  | Battery level report interval in seconds               | 60      |

:::note Default setting

While `CONFIG_ZMK_BATTERY_REPORTING` is disabled by default it is implied by `CONFIG_ZMK_BLE`, thus any board with BLE enabled will have this automatically enabled unless explicitly overriden.

:::

### Devicetree

Applies to: [`/chosen` node](https://docs.zephyrproject.org/latest/guides/dts/intro.html#aliases-and-chosen-nodes)

| Property      | Type | Description                                   |
| ------------- | ---- | --------------------------------------------- |
| `zmk,battery` | path | The node for the battery sensor driver to use |

## Battery Voltage Divider Sensor

Driver for reading the voltage of a battery using an ADC connected to a voltage divider. This driver can also read a GPIO pin to detect whether the battery is charging or not if the hardware implementation includes a battery charging IC with an output to indicate charging status. This functionality is optional, the `chg-gpios` devicetree configuration does not have to be set.

### Devicetree

Applies to: `compatible = "zmk,battery-voltage-divider"`

Definition file: [zmk/app/module/dts/bindings/sensor/zmk,battery-voltage-divider.yaml](https://github.com/zmkfirmware/zmk/blob/main/app/module/dts/bindings/sensor/zmk,battery-voltage-divider.yaml)

The ZMK battery voltage divider includes the [Zephyr voltage divider](https://docs.zephyrproject.org/latest/build/dts/api/bindings/adc/voltage-divider.html) and adds on additional functionality.

| Property    | Type       | Description                                      | Default |
| ----------- | ---------- | ------------------------------------------------ | ------- |
| `chg-gpios` | GPIO array | GPIO connected to the charging IC's charging pin |         |

:::note Charging indication

The battery charging status is not currently consumed by any indicators and cannot be conveyed to the host over BLE. The battery charging status is updated every `CONFIG_ZMK_BATTERY_REPORT_INTERVAL` seconds.

:::

## nRF VDDH Battery Sensor

Driver for reading the voltage of a battery using a Nordic nRF52's VDDH pin. This driver has no configuration except for the required `label` property.

### Devicetree

Applies to: `compatible = "zmk,battery-nrf-vddh"`

Definition file: [zmk/app/drivers/zephyr/dts/bindings/sensor/zmk,battery-nrf-vddh.yaml](https://github.com/zmkfirmware/zmk/blob/main/app/drivers/zephyr/dts/bindings/sensor/zmk%2Cbattery-nrf-vddh.yaml)

| Property | Type   | Description               |
| -------- | ------ | ------------------------- |
| `label`  | string | Unique label for the node |
