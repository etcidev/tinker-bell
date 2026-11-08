# Tinker Bell v0.1.0

Initial standalone source release:

- ESP32 + CC1101 decoder for compatible 433 MHz EV1527-style signals.
- Python Linux service with random WAV playback, 2-second cooldown and a serial heartbeat watchdog.
- Configurable serial port, sound paths and ALSA device via environment variables.
- MIT license, English/Russian documentation, hardware wiring and systemd example.
- Optional **button-code discovery mode**. It must be temporarily flashed onto an ESP32; the mode disables normal doorbell events until restored.
- Isolated Python smoke-test script. Full clean-machine installation and radio/audio hardware integration must be validated separately.

Audio recordings and individual radio codes are not included.
