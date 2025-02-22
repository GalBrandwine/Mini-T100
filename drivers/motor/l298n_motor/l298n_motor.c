/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT gabra_motor_l298n

#include <zephyr/device.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app/drivers/motor.h>

LOG_MODULE_REGISTER(l298n_motor, CONFIG_MOTOR_LOG_LEVEL);

struct motor_l298n_data
{
	struct k_timer timer;
};

/**
 * @brief A PWM motor config expects to have 3 pins
 *
 */
struct motor_l298n_config
{
	struct pwm_dt_spec pin_pwm_b;
	struct gpio_dt_spec pin_in4;
	struct gpio_dt_spec pin_in3;
	// unsigned int period_ms;
};

static void motor_direction_toggle(struct k_timer *timer)
{
	const struct device *dev = k_timer_user_data_get(timer);
	const struct motor_l298n_config *config = dev->config;
	int ret;

	ret = pwm_set_pulse_dt(&config->pin_pwm_b, 21500);
	if (ret < 0)
	{
		printk("Error %d: failed to set pulse width\n", ret);
		return;
	}

	// if (dir == DOWN)
	// {
	// 	if (pulse_width <= min_pulse)
	// 	{
	// 		dir = UP;
	// 		pulse_width = min_pulse;
	// 	}
	// 	else
	// 	{
	// 		pulse_width -= STEP;
	// 	}
	// }
	// else
	// {
	// 	pulse_width += STEP;

	// 	if (pulse_width >= max_pulse)
	// 	{
	// 		dir = DOWN;
	// 		pulse_width = max_pulse;
	// 	}
	// }

	LOG_INF("toggling pin %s", &config->pin_in4.port->name);
	ret = gpio_pin_toggle_dt(&config->pin_in4);
	if (ret < 0)
	{
		LOG_ERR("Could not toggle l298n GPIO (%d)", ret);
	}

	LOG_INF("toggling pin %s", &config->pin_in3.port->name);
	ret = gpio_pin_toggle_dt(&config->pin_in3);
	if (ret < 0)
	{
		LOG_ERR("Could not toggle l298n GPIO (%d)", ret);
	}
}

static int motor_l928n_set_speed(const struct device *dev,
								 char speed)
{
	const struct motor_l298n_config *config = dev->config;
	struct motor_l298n_data *data = dev->data;

	if (speed == 0)
	{
		k_timer_stop(&data->timer);
		return gpio_pin_set_dt(&config->pin_pwm_b, 0) &&
			   gpio_pin_set_dt(&config->pin_in4, 0) &&
			   gpio_pin_set_dt(&config->pin_in3, 0);
		;
	}

	k_timer_start(&data->timer, K_MSEC(speed * 100), K_MSEC(speed * 100));

	return 0;
}

static const struct motor_driver_api motor_l298n_api = {
	.set_speed = &motor_l928n_set_speed,
};

static int motor_l298n_init(const struct device *dev)
{
	const struct motor_l298n_config *config = dev->config;
	struct motor_l298n_data *data = dev->data;
	int ret;

	// struct gpio_dt_spec pin_pwm_b;
	// struct gpio_dt_spec pin_in4;
	// struct gpio_dt_spec pin_in3;
	if (!pwm_is_ready_dt(&config->pin_pwm_b))
	{
		LOG_ERR("l298n %s PWM not ready", &config->pin_pwm_b.dev->name);
		return -ENODEV;
	}

	if (!gpio_is_ready_dt(&config->pin_in4))
	{
		LOG_ERR("l298n %s GPIO not ready", &config->pin_in4.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&config->pin_in4, GPIO_OUTPUT_HIGH);
	if (ret < 0)
	{
		LOG_ERR("l298n %s GPIO not ready", &config->pin_in4.port->name);
		return ret;
	}

	if (!gpio_is_ready_dt(&config->pin_in3))
	{
		LOG_ERR("l298n %s GPIO not ready", &config->pin_in3.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&config->pin_in3, GPIO_OUTPUT_LOW);
	if (ret < 0)
	{
		LOG_ERR("l298n %s GPIO not ready", &config->pin_in3.port->name);
		return ret;
	}

	k_timer_init(&data->timer, motor_direction_toggle, NULL);
	k_timer_user_data_set(&data->timer, (void *)dev);

	k_timer_start(&data->timer, K_MSEC(500),
				  K_MSEC(500));
	// if (config->period_ms > 0)
	// {
	// 	k_timer_start(&data->timer, K_MSEC(config->period_ms),
	// 				  K_MSEC(config->period_ms));
	// }

	return 0;
}

#define MOTOR_L298N_DEFINE(inst)                                     \
	static struct motor_l298n_data data##inst;                       \
	static const struct motor_l298n_config config##inst = {          \
		.pin_pwm_b = PWM_DT_SPEC_INST_GET(inst),                     \
		.pin_in4 = GPIO_DT_SPEC_INST_GET_BY_IDX(inst, gpios, 0),     \
		.pin_in3 = GPIO_DT_SPEC_INST_GET_BY_IDX(inst, gpios, 1),     \
	};                                                               \
	DEVICE_DT_INST_DEFINE(inst, motor_l298n_init, NULL, &data##inst, \
						  &config##inst, POST_KERNEL,                \
						  CONFIG_MOTOR_INIT_PRIORITY,                \
						  &motor_l298n_api);

DT_INST_FOREACH_STATUS_OKAY(MOTOR_L298N_DEFINE)
