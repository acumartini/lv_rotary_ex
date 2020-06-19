.PHONY: init init-clion init-vscode install update check test clean build upload

NAME := lv_rotary_ex

init:
	platformio init --board adafruit_feather_m4

init-clion:
	platformio init --ide clion --board adafruit_feather_m4

init-vscode:
	platformio init --ide vscode --board adafruit_feather_m4

install:
	platformio lib install "Adafruit ZeroDMA=git@github.com:adafruit/Adafruit_ZeroDMA.git"
	platformio lib install "Adafruit HX8357 Library"
	platformio lib install "Adafruit STMPE610"
	platformio lib install "EncoderStepCounter"
	platformio lib install "git@github.com:adafruit/Adafruit_NeoPixel.git"
	platformio lib install "git@github.com:acumartini/lvgl.git"

update:
	platformio platform update
	platformio lib update "Adafruit ZeroDMA=git@github.com:adafruit/Adafruit_ZeroDMA.git"
	platformio lib update "Adafruit HX8357 Library"
	platformio lib update "Adafruit STMPE610"
	platformio lib update "EncoderStepCounter"
	platformio lib update "git@github.com:adafruit/Adafruit_NeoPixel.git"
	platformio lib update "git@github.com:acumartini/lvgl.git"

check:
	platformio check

test:
	platformio test

clean:
	pio run --target clean

build:
	pio run

upload:
	platformio run -t upload

