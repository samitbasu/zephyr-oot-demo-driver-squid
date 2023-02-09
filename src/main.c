/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sensor_squid_driver.h"
#include <stdio.h>
#include <zephyr/kernel.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 500

const struct device *dev;

void main(void)
{
	int ret;
	bool state = false;
	printk("Hello World from the app!\n");

	dev = device_get_binding("SENSOR_SQUID");

	__ASSERT(dev, "Failed to get device binding");

	printk("device is %p, name is %s\n", dev, dev->name);

	while (1)
	{
		sensor_squid_led(dev, state);
		state = !state;
		k_msleep(SLEEP_TIME_MS);
	}
}
