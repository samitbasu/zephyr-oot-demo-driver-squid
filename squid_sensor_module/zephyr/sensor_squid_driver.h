/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __SENSOR_SQUID_DRIVER_H__
#define __SENSOR_SQUID_DRIVER_H__

#include <stdint.h>

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * Turn on the LED... Very sparkles!
	 */
	void sensor_squid_led(const struct device *dev, bool state);

	/**
	 * Retrieve the state of the LED...
	 */
	//	bool sensor_squid_state(const struct device *dev);

	/**
	 * Send a greeting...
	 */
	//	void sensor_squid_greet(const struct device *dev, int number);

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_SQUID_DRIVER_H__ */
