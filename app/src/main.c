/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/app_version.h>

#define SLEEP_TIME_MS 10 * 60 * 1000

#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("button_pressed. dev %s\n", dev->name);
	printk("dev label: %s\n", DT_PROP(LED0_NODE, label));
	gpio_pin_toggle_dt(&led);
}

static struct gpio_callback button_cb_data;

int main(void)
{
	int ret;
	printk("Zephyr Example Application %s\n", APP_VERSION_EXTENDED_STRING);

	/* Only checking one since led.port and button.port point to the same device, &gpio0 */
	if (!device_is_ready(led.port))
	{
		return -1;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return -1;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret < 0)
	{
		return -1;
	}

	ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));

	gpio_add_callback(button.port, &button_cb_data);
	while (1)
	{
		k_msleep(SLEEP_TIME_MS);
	}
	// while (true)
	// {
	// 	printk("looping\n");
	// 	k_msleep(1000);
	// }

	return 0;
}
