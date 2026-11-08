#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
TEST_DIR=$(mktemp -d)
trap 'rm -rf "$TEST_DIR"' EXIT
python3 -m venv "$TEST_DIR/.venv"
"$TEST_DIR/.venv/bin/python" -m pip install --disable-pip-version-check -r server/requirements.txt
"$TEST_DIR/.venv/bin/python" - <<'PY'
import ast
from pathlib import Path
import serial
p = Path('server/doorbell.py')
ast.parse(p.read_text())
assert 'Serial' in dir(serial)
assert Path('firmware/DoorbellReceiver/DoorbellReceiver.ino').is_file()
assert Path('firmware/DoorbellReceiver/button_config.example.h').is_file()
print('PASS: isolated Python environment, dependency import, syntax and expected files')
PY
if command -v aplay >/dev/null 2>&1; then
  echo 'PASS: ALSA aplay is present'
else
  echo 'NOTE: ALSA aplay is absent; install alsa-utils before playback'
fi
if command -v arduino-cli >/dev/null 2>&1 && test -f firmware/DoorbellReceiver/button_config.h; then
  echo 'Testing normal firmware build...'
  arduino-cli compile --fqbn esp32:esp32:esp32 firmware/DoorbellReceiver
  echo 'Testing learn-mode firmware build in a temporary sketch copy...'
  mkdir -p "$TEST_DIR/firmware"
  cp -a firmware/DoorbellReceiver "$TEST_DIR/firmware/"
  printf '
#define TINKER_BELL_LEARN_MODE
' >> "$TEST_DIR/firmware/DoorbellReceiver/button_config.h"
  arduino-cli compile --fqbn esp32:esp32:esp32 "$TEST_DIR/firmware/DoorbellReceiver"
  echo 'PASS: normal and learn-mode firmware compile'
else
  echo 'NOTE: ESP32 compilation skipped (arduino-cli or local button_config.h unavailable)'
fi
