build:
	rm -rf build
	cmake -B build -G Ninja
	ninja -C build


flash:
	nrfjprog --program build/zephyr/zephyr.hex --sectoranduicrerase --verify -f NRF52


clean:
	rm -rf build
	
	
