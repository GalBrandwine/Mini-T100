/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <app/drivers/motor.h>
#include <zephyr/app_version.h>

LOG_MODULE_REGISTER(app, CONFIG_APP_LOG_LEVEL);

#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

const struct device *const left_motor = DEVICE_DT_GET(DT_NODELABEL(left_l298n_motor));
const struct device *const right_motor = DEVICE_DT_GET(DT_NODELABEL(right_l298n_motor));

static const struct device *check_l298n_motor_device(const struct device *motor)
{
	if (motor == NULL)
	{
		/* No such node, or the node does not have status "okay". */
		LOG_ERR("\nError: no device found.\n");
		return NULL;
	}

	if (!device_is_ready(motor))
	{
		LOG_ERR("\nError: Device \"%s\" is not ready; "
				"check the driver initialization logs for errors.\n",
				motor->name);
		return NULL;
	}

	LOG_INF("Found device \"%s\", getting sensor data\n", motor->name);
	return motor;
}

bool forward = true;
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("button_pressed. dev %s\n", dev->name);
	printk("dev label: %s\n", DT_PROP(LED0_NODE, label));
	gpio_pin_toggle_dt(&led);

	const struct motor_driver_api *motor_api =
		(const struct motor_driver_api *)left_motor->api;

	if (forward) // Drive Forward
	{
		motor_api->set_direction_speed(left_motor, RIGHT, 50);
		motor_api->set_direction_speed(right_motor, LEFT, 50);
		forward = false;
	}
	else // Drive Backward
	{
		forward = true;
		motor_api->set_direction_speed(left_motor, LEFT, 50);
		motor_api->set_direction_speed(right_motor, RIGHT, 50);
	}
}

static struct gpio_callback button_cb_data;

int main(void)
{
	int ret;
	printk("Version %s\n", APP_VERSION_EXTENDED_STRING);

	const struct device *ready_left_motor = check_l298n_motor_device(left_motor);
	if (ready_left_motor == NULL)
	{
		return -1;
	}

	const struct device *ready_right_motor = check_l298n_motor_device(right_motor);
	if (ready_right_motor == NULL)
	{
		return -1;
	}
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
	if (ret < 0)
	{
		return -1;
	}
	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	return 0;
}
