/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//#include "sensor_squid_driver.h"
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   500

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

const struct device *dev;

static void user_entry(void *p1, void *p2, void *p3)
{
	hello_world_print(dev);
}

void main(void)
{
	int ret;
	printk("Hello World from the app!\n");

	dev = device_get_binding("CUSTOM_DRIVER");

	__ASSERT(dev, "Failed to get device binding");

	printk("device is %p, name is %s\n", dev, dev->name);

	if (!gpio_is_ready_dt(&led)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}
		k_msleep(SLEEP_TIME_MS);
	}

//	k_object_access_grant(dev, k_current_get());
//	k_thread_user_mode_enter(user_entry, NULL, NULL, NULL);
}
