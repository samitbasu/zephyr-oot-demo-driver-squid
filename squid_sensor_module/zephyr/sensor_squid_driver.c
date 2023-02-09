/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sensor_squid_driver.h"
#include <zephyr/types.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#define LED0_NODE DT_ALIAS(led0)

/**
 * This struct contains data needed by the driver that needs to be mutable.
 */
struct sensor_squid_data
{
	bool led_state;
};

/**
 * This struct contains the config data for the driver.
 */
struct sensor_squid_config
{
	struct gpio_dt_spec pin;
};

static int
sensor_squid_init(const struct device *dev)
{
	const struct sensor_squid_config *config = dev->config;
	struct sensor_squid_data *data = dev->data;
	printk("INIT");
	if (!gpio_is_ready_dt(&config->pin))
	{
		return -ENODEV;
	}
	if (gpio_pin_configure_dt(&config->pin, GPIO_OUTPUT_ACTIVE) < 0)
	{
		return -ENODEV;
	}
	data->led_state = false;
	return 0;
}

void sensor_squid_led(const struct device *dev, bool state)
{
	const struct sensor_squid_config *config = dev->config;
	struct sensor_squid_data *data = dev->data;
	gpio_pin_set_dt(&config->pin, state);
	data->led_state = state;
}

bool sensor_squid_state(const struct device *dev)
{
	struct sensor_squid_data *data = dev->data;
	return data->led_state;
}

// The config struct is static and initialized at compile time.
static const struct sensor_squid_config sensor_squid_config_inst = {
	.pin = GPIO_DT_SPEC_GET(LED0_NODE, gpios)};

// The data struct is static, but initialized at run time
static struct sensor_squid_data sensor_squid_data_inst;

// We won't rely on the device tree to load the driver.
DEVICE_DEFINE(sensor_squid,						  // dev name unique token as a C identifier
			  "SENSOR_SQUID",					  // String name for the device.
			  sensor_squid_init,				  // initialization function
			  NULL,								  // power management resources...
			  &sensor_squid_data_inst,			  // pointer to the data struct
			  &sensor_squid_config_inst,		  // pointer to the config struct
			  POST_KERNEL,						  // Initialize the kernel first
			  CONFIG_KERNEL_INIT_PRIORITY_DEVICE, // initialization priority
			  NULL);							  // No API pointer
