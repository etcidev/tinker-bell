import os
import random
import serial
import subprocess
import time
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parent.parent

PORT = os.getenv("TINKER_BELL_SERIAL_PORT", "/dev/ttyUSB0")
BAUD_RATE = int(os.getenv("TINKER_BELL_BAUD_RATE", "115200"))

SOUND_DIR = Path(
    os.getenv("TINKER_BELL_SOUND_DIR", str(PROJECT_DIR / "sounds"))
)

DEFAULT_SOUND = Path(
    os.getenv(
        "TINKER_BELL_FALLBACK_SOUND",
        str(SOUND_DIR / "fallback" / "dingdong.wav")
    )
)

AUDIO_DEVICE = os.getenv("TINKER_BELL_AUDIO_DEVICE", "plughw:0,0")

COOLDOWN = 2.0
WATCHDOG_TIMEOUT = 15.0

last_ring = 0.0
player = None


def pick_sound():
    sounds = [
        path for path in SOUND_DIR.iterdir()
        if path.is_file() and path.suffix.lower() == ".wav"
    ] if SOUND_DIR.is_dir() else []
    return random.choice(sounds) if sounds else DEFAULT_SOUND


while True:
    try:
        print("Подключение к ESP32...", flush=True)

        with serial.Serial(PORT, BAUD_RATE, timeout=1) as esp32:
            print("Doorbell активен. Ожидание звонка.", flush=True)

            last_stat = time.monotonic()
            last_report = 0.0

            while True:
                line = esp32.readline().decode(
                    "utf-8", errors="replace"
                ).strip()

                now = time.monotonic()

                if line.startswith("STAT "):
                    last_stat = now

                    if now - last_report >= 60:
                        print(f"ESP32 отвечает: {line}", flush=True)
                        last_report = now

                elif line == "DOORBELL":
                    if now - last_ring < COOLDOWN:
                        continue

                    last_ring = now
                    sound = pick_sound()

                    if not sound.is_file():
                        print(f"Файл не найден: {sound}", flush=True)
                        continue

                    print(f"ЗВОНОК: {sound.name}", flush=True)

                    # Do not let long audio block serial monitoring.
                    if player is not None and player.poll() is None:
                        player.terminate()
                        try:
                            player.wait(timeout=1)
                        except subprocess.TimeoutExpired:
                            player.kill()
                            player.wait()

                    player = subprocess.Popen([
                        "aplay", "-q", "-D", AUDIO_DEVICE, str(sound)
                    ])

                if time.monotonic() - last_stat > WATCHDOG_TIMEOUT:
                    raise serial.SerialException(
                        "Нет сообщений STAT более 15 секунд"
                    )

    except (serial.SerialException, OSError) as error:
        print(f"Ошибка: {error}", flush=True)
        print("Повторное подключение через 3 секунды...", flush=True)
        time.sleep(3)

    except KeyboardInterrupt:
        print("Doorbell остановлен", flush=True)
        break
