/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sensor_squid_driver.h"
#include <stdio.h>
#include <zephyr/kernel.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 25

const struct device *dev;

void main(void)
{
	int ret;
	bool state = false;
	int frame = 0;
	int speed = 1;
	int loop = 0;
	printk("Hello World from the app!\n");

	dev = device_get_binding("SENSOR_SQUID");

	__ASSERT(dev, "Failed to get device binding");

	printk("device is %p, name is %s\n", dev, dev->name);


	while (1)
	{
		if ((loop % speed) == 0) {
			sensor_squid_update(dev, (frame % 4));
			frame++;
		}
		int8_t buttons = sensor_squid_state(dev);
		if ((buttons & 1) != 0) {
			speed++;
			printk("Delay is now %d\n", speed);
		} else if ((buttons & 2) != 0) {
			speed = (speed > 1) ? speed - 1 : speed;
			printk("Delay is now %d\n", speed);
		}
		k_msleep(SLEEP_TIME_MS);
		loop++;
	}
}
