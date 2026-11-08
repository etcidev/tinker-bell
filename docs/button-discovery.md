# Finding the code of a 433 MHz button

**Applies to buttons whose signal is compatible with the current EV1527-style decoder.** Frequency alone does not guarantee compatibility. Some transmitters use other formats.

## 1. Enable temporary discovery mode

Copy the example if you do not already have your local config:

```sh
cp firmware/DoorbellReceiver/button_config.example.h firmware/DoorbellReceiver/button_config.h
```

In `firmware/DoorbellReceiver/button_config.h`, uncomment:

```cpp
#define TINKER_BELL_LEARN_MODE
```

This is a **local** file excluded from Git. In discovery mode, the firmware **does not send `DOORBELL`**: normal doorbell operation is temporarily suspended on the ESP32 flashed with this build.

## 2. Build and upload

```sh
arduino-cli compile --fqbn esp32:esp32:esp32 firmware/DoorbellReceiver
arduino-cli upload --fqbn esp32:esp32:esp32 --port /dev/ttyUSB0 firmware/DoorbellReceiver
```

**Important:** Stop the Linux doorbell service before accessing the serial port, and keep it stopped while using the serial monitor. If this is your live doorbell, expect downtime until normal firmware is restored.

```sh
arduino-cli monitor --port /dev/ttyUSB0 --config baudrate=115200
```

Press the candidate button several times. Look for repeated lines such as:

```text
BUTTON_CODE=0x123456
```

The printed code is the decoded 24-bit value. Nearby transmitters and noise may also produce decoded values, so confirm the value by repeated presses and by testing the normal mode. Keep in mind that 433 MHz EV1527 is fixed-code and not secure.

## 3. Restore normal operation

In `button_config.h`, update `BUTTON_CODE` to the observed value and comment out the `#define TINKER_BELL_LEARN_MODE` line. Recompile and re-upload the normal firmware, close the serial monitor and restart your normal Linux service. Check that a button press plays a sound and periodic `STAT` messages arrive.

**Do not use the discovered code as a security credential for a lock or access-control system.**
