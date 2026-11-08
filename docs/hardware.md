# Hardware and wiring

## Components

- ESP32 ESP-WROOM-32 DevKit
- CC1101 433 MHz radio transceiver
- 433 MHz wireless doorbell button
- CC1101-compatible antenna
- Dupont jumper wires
- USB data cable
- Linux computer with audio output

## CC1101 to ESP32 wiring

| CC1101 | ESP32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SCK | GPIO18 |
| MISO | GPIO19 |
| MOSI | GPIO23 |
| CSN / SS | GPIO5 |
| GDO0 | GPIO27 |

Important: use 3.3V for the CC1101 module.
Do not connect its VCC to 5V.

## Radio configuration

The tested firmware uses:

- Frequency: 433.92 MHz
- Modulation setting: 2 (ASK/OOK in the CC1101 library)
- RX bandwidth: 325 kHz
- Data rate setting: 4.8 kBaud
- Protocol: EV1527-compatible, 24-bit code

## Button configuration

Copy the example configuration:

    cp firmware/DoorbellReceiver/button_config.example.h \
       firmware/DoorbellReceiver/button_config.h

Edit `button_config.h` and replace the example
BUTTON_CODE with the 24-bit code of your button.

The personal configuration file is excluded from Git.

## USB connection

ESP32 connects to the Linux computer through USB.

The firmware sends:

    DOORBELL

when the configured button is detected.

It also sends periodic status messages:

    STAT frames=100 decoded=10

Serial speed: 115200 baud.

## Compatibility

Not every 433 MHz wireless button uses EV1527.

Other transmitters may require firmware changes.

This project has been tested with the Spreest B100-DB button.
