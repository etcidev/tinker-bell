# Installation guide

Tinker Bell runs locally on Linux, without cloud services.

## 1. Requirements

Hardware:
- ESP32 development board
- CC1101 433 MHz receiver and antenna
- Compatible EV1527 wireless button
- Linux computer and speaker

Software:
- Python 3
- Arduino CLI with ESP32 board support
- SmartRC CC1101 library
- ALSA audio utilities

## 2. Install server dependencies

On Ubuntu or Debian:

    sudo apt update
    sudo apt install git python3 python3-venv alsa-utils

Clone the project:

    mkdir -p ~/projects
    cd ~/projects
    git clone https://github.com/etcidev/tinker-bell.git
    cd tinker-bell

Create a Python virtual environment:

    python3 -m venv .venv
    .venv/bin/pip install -r server/requirements.txt

## 3. Configure the ESP32

Connect CC1101 according to hardware.md.

Copy the example button configuration:

    cp firmware/DoorbellReceiver/button_config.example.h \
       firmware/DoorbellReceiver/button_config.h

Edit button_config.h and set BUTTON_CODE to the
24-bit identifier of your own wireless button.

The project does not currently include an automatic
button-code discovery utility.

Compile the firmware:

    arduino-cli compile --fqbn esp32:esp32:esp32 \
      firmware/DoorbellReceiver

After confirming the serial port, upload with Arduino CLI:

    arduino-cli upload --fqbn esp32:esp32:esp32 \
      --port /dev/ttyUSB0 firmware/DoorbellReceiver

The ESP32 board package and SmartRC CC1101 library
must be installed in Arduino CLI beforehand.

## 4. Configure audio

Create a directory for sound files:

    mkdir -p ~/tinker-bell-sounds

Place at least one WAV file in this directory.

Find the Linux audio device:

    aplay -l

The default audio device is plughw:0,0.
Other computers may use a different device.

## 5. Serial port access

Check the serial device:

    ls -l /dev/ttyUSB0

The user running Tinker Bell must have access to
the serial port, commonly through the dialout group.

Only one application should open the serial port
at a time.

## 6. Start manually

From the repository directory:

    TINKER_BELL_SOUND_DIR="$HOME/tinker-bell-sounds" \
    TINKER_BELL_AUDIO_DEVICE="plughw:0,0" \
    .venv/bin/python server/doorbell.py

Press the wireless button to test playback.

Stop the application with Ctrl+C.

## 7. Run as a systemd service

See systemd/tinker-bell.service.

Edit the example service to match your username,
installation paths, sound directory, and audio device.

Install the configured service:

    sudo cp systemd/tinker-bell.service \
      /etc/systemd/system/tinker-bell.service

    sudo systemctl daemon-reload
    sudo systemctl enable --now tinker-bell.service

Check its status:

    systemctl status tinker-bell.service

View logs:

    journalctl -u tinker-bell.service -f

## Troubleshooting

No sound:
- Check WAV files and ALSA output device.
- Test audio with aplay.

No serial connection:
- Check the USB cable and port permissions.
- Ensure no other program is using the port.

No button events:
- Check CC1101 wiring and the antenna.
- Check the radio protocol and button identifier.

## Security

EV1527 uses fixed radio codes and is not suitable
for secure access control.
