# Tinker Bell 🔔

A standalone, self-hosted wireless doorbell built with ESP32, CC1101 and Python.

Tinker Bell receives a 433 MHz signal from a wireless doorbell button and plays a randomly selected WAV sound through a Linux computer.

No cloud services, internet connection or home automation platform required.

## How it works

Wireless button (433 MHz)
        |
        v
    CC1101 receiver
        |
        v
       ESP32
        |
   USB Serial (115200)
        |
        v
   Python on Linux
        |
        v
   WAV audio playback
        |
        v
      Speaker

## Features

- 433 MHz wireless doorbell reception
- EV1527-compatible signal decoding
- Random WAV sound playback
- Custom sound collection
- Protection against repeated triggers
- Automatic serial reconnection
- ESP32 heartbeat monitoring
- Linux systemd service support
- Fully local operation

## Hardware

- ESP32 ESP-WROOM-32 development board
- CC1101 433 MHz transceiver with antenna
- Compatible wireless doorbell button
- USB data cable
- Linux computer
- Speaker or soundbar

Tested with the Spreest B100-DB wireless doorbell button.

See [Hardware and wiring](docs/hardware.md).

## Project structure

    firmware/
      DoorbellReceiver/
        DoorbellReceiver.ino
        button_config.example.h

    server/
      doorbell.py
      requirements.txt

    docs/
      hardware.md
      installation.md
      README.ru.md

    systemd/
      tinker-bell.service

## Quick start

1. Connect the CC1101 module to ESP32.
2. Create `button_config.h` from `button_config.example.h`.
3. Set your wireless button code.
4. Compile and flash the ESP32 firmware.
5. Connect ESP32 to a Linux computer using USB.
6. Install the Python dependencies.
7. Add WAV audio files and start the doorbell service.

See [Installation guide](docs/installation.md).

## Configuration

The Python application supports environment variables:

| Variable | Default |
|---|---|
| TINKER_BELL_SERIAL_PORT | /dev/ttyUSB0 |
| TINKER_BELL_BAUD_RATE | 115200 |
| TINKER_BELL_SOUND_DIR | ./sounds |
| TINKER_BELL_FALLBACK_SOUND | ./sounds/fallback/dingdong.wav |
| TINKER_BELL_AUDIO_DEVICE | plughw:0,0 |

Default sound paths are relative to the project directory.

## Audio

Place WAV files in the configured sounds directory.

Tinker Bell randomly selects one file each time the doorbell is activated.

Audio collections are not included in the repository. Use sounds that you have permission to use.

## Documentation

- [Hardware and wiring](docs/hardware.md)
- [Installation](docs/installation.md)
- [Русская документация](docs/README.ru.md)

## Security

EV1527 uses fixed radio codes. Tinker Bell is intended for doorbell notifications, not for unlocking doors or controlling security-critical access.

## License

License to be selected before the first public release.
