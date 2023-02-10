/*
 * Copyright (c) 2019 Nordic Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT zephyr,squid

#include "sensor_squid_driver.h"
#include <zephyr/types.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#define BUTTON_FASTER_NODE DT_ALIAS(sw0)
#define BUTTON_SLOWER_NODE DT_ALIAS(sw2)
#define SQUID_NODE DT_NODELABEL(squid)

#define MAX7219_SEGMENTS_PER_DIGIT 8
#define MAX7219_DIGITS_PER_DEVICE 8

/* clang-format off */

/* MAX7219 registers and fields */
#define MAX7219_REG_NOOP		0x00
#define MAX7219_NOOP			0x00

#define MAX7219_REG_DECODE_MODE		0x09
#define MAX7219_NO_DECODE		0x00

#define MAX7219_REG_INTENSITY		0x0A

#define MAX7219_REG_SCAN_LIMIT		0x0B

#define MAX7219_REG_SHUTDOWN		0x0C
#define MAX7219_SHUTDOWN_MODE		0x00
#define MAX7219_LEAVE_SHUTDOWN_MODE	0x01

#define MAX7219_REG_DISPLAY_TEST	0x0F
#define MAX7219_LEAVE_DISPLAY_TEST_MODE	0x00
#define MAX7219_DISPLAY_TEST_MODE	0x01

/**
 * This struct contains data needed by the driver that needs to be mutable.
 */
struct sensor_squid_data
{
	uint8_t tx_buf[2];
};

/**
 * This struct contains the config data for the driver.
 */
struct sensor_squid_config
{
	struct gpio_dt_spec pin_faster;
	struct gpio_dt_spec pin_slower;
	struct spi_dt_spec spi;
	uint16_t frame_bufs[4][4];
};

static int max7219_transmit(const struct device *dev, const uint8_t addr, const uint8_t value)
{
	struct sensor_squid_data *dev_data = dev->data;
	const struct sensor_squid_config *dev_config = dev->config;
	const struct spi_buf tx_buf = {
		.buf = dev_data->tx_buf,
		.len = 2,
	};
	const struct spi_buf_set tx_bufs = {
		.buffers = &tx_buf,
		.count = 1U,
	};
	dev_data->tx_buf[0] = addr;
	dev_data->tx_buf[1] = value;

	return spi_write_dt(&dev_config->spi, &tx_bufs);
}

static int
sensor_squid_init(const struct device *dev)
{
	const struct sensor_squid_config *config = dev->config;
	struct sensor_squid_data *data = dev->data;
	printk("INIT\n");
	if (!gpio_is_ready_dt(&config->pin_faster))
	{
		return -ENODEV;
	}
	if (!gpio_is_ready_dt(&config->pin_slower))
	{
		return -ENODEV;
	}
	if (gpio_pin_configure_dt(&config->pin_faster, GPIO_INPUT) < 0)
	{
		return -ENODEV;
	}
	if (gpio_pin_configure_dt(&config->pin_slower, GPIO_INPUT) < 0)
	{
		return -ENODEV;
	}
	if (!spi_is_ready_dt(&config->spi))
	{
		printk("SPI device is not ready");
		return -ENODEV;
	}
	printk("SPI INITIALIZED\n");
	// Exit test mode
	
	max7219_transmit(dev, MAX7219_REG_DISPLAY_TEST, MAX7219_LEAVE_DISPLAY_TEST_MODE);
	max7219_transmit(dev, MAX7219_REG_DECODE_MODE, MAX7219_NO_DECODE);
	max7219_transmit(dev, MAX7219_REG_INTENSITY, 0x3);
	max7219_transmit(dev, MAX7219_REG_SCAN_LIMIT, 7);
/*	max7219_transmit(dev, 1, 0x3E);
	max7219_transmit(dev, 2, 0x41);
	max7219_transmit(dev, 3, 0x55);
	max7219_transmit(dev, 4, 0x41);
	max7219_transmit(dev, 5, 0x32);
	max7219_transmit(dev, 6, 0x2E);
	max7219_transmit(dev, 7, 0x49);
	max7219_transmit(dev, 8, 0x91);*/
	max7219_transmit(dev, MAX7219_REG_SHUTDOWN, MAX7219_LEAVE_SHUTDOWN_MODE);
	return 0;
}

void sensor_squid_update(const struct device *dev, int frame) {
	const struct sensor_squid_config *config = dev->config;
	struct sensor_squid_data *data = dev->data;
	const uint16_t *frame_ptr = config->frame_bufs[frame];
	for (int i=0;i<4;i++) {
		max7219_transmit(dev, 2*i+1, frame_ptr[i] & 0xFF);
		max7219_transmit(dev, 2*i+2, (frame_ptr[i] & 0xFF00) >> 8);
	}
}

uint8_t sensor_squid_state(const struct device *dev)
{
	const struct sensor_squid_config *config = dev->config;
	struct sensor_squid_data *data = dev->data;
	uint8_t buttons = 0;
	if (gpio_pin_get_dt(&config->pin_faster)) {
		buttons |= 1;
	}
	if (gpio_pin_get_dt(&config->pin_slower)) {
		buttons |= 2;
	}
	return buttons;
}

// The config struct is static and initialized at compile time.
static const struct sensor_squid_config sensor_squid_config_inst = {
	.pin_faster = GPIO_DT_SPEC_GET(BUTTON_FASTER_NODE, gpios),
	.pin_slower = GPIO_DT_SPEC_GET(BUTTON_SLOWER_NODE, gpios),
	.spi = SPI_DT_SPEC_GET(SQUID_NODE, SPI_WORD_SET(8) | SPI_MODE_GET(0), 0),
	.frame_bufs = {{0x413e,0x4155,0x2e32,0x9149},{0x3e00,0x5541,0x3e41,0x9149},{0x7c00,0xaa82,0x7c82,0xa354},{0x827c,0x82aa,0x927c,0x498a}},
};

// The data struct is static, but initialized at run time
static struct sensor_squid_data sensor_squid_data_inst;

// We won't rely on the device tree to load the driver.
DEVICE_DEFINE(sensor_squid,				 // dev name unique token as a C identifier
			  "SENSOR_SQUID",			 // String name for the device.
			  sensor_squid_init,		 // initialization function
			  NULL,						 // power management resources...
			  &sensor_squid_data_inst,	 // pointer to the data struct
			  &sensor_squid_config_inst, // pointer to the config struct
			  APPLICATION,				 // Initialize the kernel first
			  32,						 // initialization priority
			  NULL);					 // No API pointer
