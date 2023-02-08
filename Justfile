build:
	cmake -B build -G Ninja
	ninja -C build


flash:
	west flash
