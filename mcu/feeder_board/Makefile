.PHONY: build upload clean clangd

build:
	pio run

upload:
	pio run --target upload

clean:
	pio run --target clean

clangd:
	pio run -t compiledb
	cp .pio/build/esp32dev/compile_commands.json .

serial:
	LANG=C minicom -D /dev/ttyUSB0
