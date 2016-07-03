all:
	pio run
clean:
	pio run --target=clean

upload: all
	pio run --target=upload
	pio serialports monitor -b 115200

monitor:
	pio serialports monitor -b 115200
