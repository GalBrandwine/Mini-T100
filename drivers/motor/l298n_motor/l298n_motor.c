/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT gal_motor_l298n

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <app/drivers/motor.h>

LOG_MODULE_REGISTER(l298n_motor, CONFIG_MOTOR_LOG_LEVEL);

#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

struct motor_l298n_data
{
	struct k_timer timer;
	uint32_t max_period, current_period;
};

/**
 * @brief A PWM motor config expects to have 3 pins
 *
 */
struct motor_l298n_config
{
	struct gpio_dt_spec pin_in4;
	struct gpio_dt_spec pin_in3;
	struct pwm_dt_spec pin_pwm_b;
};

static void motor_direction_toggle(struct k_timer *timer)
{
	const struct device *dev = k_timer_user_data_get(timer);
	const struct motor_l298n_config *config = dev->config;
	int ret;

	// ret = pwm_set_pulse_dt(&config->pin_pwm_b, 21500);
	// if (ret < 0)
	// {
	// 	LOG_ERR("Error %d: failed to set pulse width\n", ret);
	// 	return;
	// }

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

	LOG_DBG("toggling pin %d", &config->pin_in4.pin);
	ret = gpio_pin_toggle_dt(&config->pin_in4);
	if (ret < 0)
	{
		LOG_ERR("Could not toggle l298n GPIO (%d)", ret);
		return;
	}

	LOG_DBG("toggling pin %d", &config->pin_in3.pin);
	ret = gpio_pin_toggle_dt(&config->pin_in3);
	if (ret < 0)
	{
		LOG_ERR("Could not toggle l298n GPIO (%d)", ret);
		return;
	}
}

/**
 * @brief Set speed
 *
 * @param dev an L928N Motor Driver Instance
 * @param speed in percents (100 is pedal to the metal)
 * @return int
 */
static int motor_l928n_set_speed(const struct device *dev,
								 char speed)
{
	const struct motor_l298n_config *config = dev->config;
	struct motor_l298n_data *data = dev->data;
	int ret = 0;
	LOG_DBG("Setting speed: %d", speed);
	if (speed == 0)
	{
		k_timer_stop(&data->timer);

		ret = pwm_set_dt(&config->pin_pwm_b, data->max_period, data->max_period);
		if (ret != 0)
		{
			LOG_ERR("failed stopping pwm. ret %d", ret);
		}
		ret = gpio_pin_set_dt(&config->pin_in4, 0);
		if (ret != 0)
		{
			LOG_ERR("failed turning off gpio. ret %d", ret);
		}
		ret = gpio_pin_set_dt(&config->pin_in3, 0);
		if (ret != 0)
		{
			LOG_ERR("failed turning off gpio. ret %d", ret);
		}
		return ret;
	}

	uint32_t pulse = data->current_period - (data->current_period / 100U) * speed;
	LOG_DBG("Got desired speed: %d. Setting pulse: %d", speed, pulse);
	if (pulse > data->max_period)
	{
		pulse = data->max_period;
	}

	ret = pwm_set_dt(&config->pin_pwm_b, data->current_period, pulse);
	if (ret != 0)
	{
		LOG_ERR("failed stopping pwm. ret %d", ret);
	}
	// k_timer_start(&data->timer, K_MSEC(speed * 100), K_MSEC(speed * 100));

	return ret;
}

static const struct motor_driver_api motor_l298n_api = {
	.set_speed = &motor_l928n_set_speed,
};

static int motor_l298n_init(const struct device *dev)
{
	const struct motor_l298n_config *config = dev->config;
	struct motor_l298n_data *data = dev->data;
	int ret;

	if (!pwm_is_ready_dt(&config->pin_pwm_b))
	{
		LOG_ERR("l298n %s PWM not ready", &config->pin_pwm_b.dev->name);
		return -ENODEV;
	}

	/*
	 * In case the default MAX_PERIOD value cannot be set for
	 * some PWM hardware, decrease its value until it can.
	 *
	 * Keep its value at least MIN_PERIOD * 4 to make sure
	 * the sample changes frequency at least once.
	 */
	LOG_DBG("Calibrating for channel %d...\n", &config->pin_pwm_b.channel);
	uint32_t max_period = MAX_PERIOD;
	while (pwm_set_dt(&config->pin_pwm_b, max_period, max_period / 2U))
	{
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD))
		{
			printk("Error: PWM device "
				   "does not support a period at least %lu\n",
				   4U * MIN_PERIOD);
			return 0;
		}
	}

	printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
		   max_period, MIN_PERIOD);

	data->max_period = max_period;
	data->current_period = MIN_PERIOD;
	if (!gpio_is_ready_dt(&config->pin_in4))
	{
		LOG_ERR("l298n %s GPIO not ready", &config->pin_in4.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&config->pin_in4, GPIO_OUTPUT_HIGH);
	if (ret < 0)
	{
		LOG_ERR("l298n %s GPIO not ready", &config->pin_in4.pin);
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
		LOG_ERR("l298n %s GPIO not ready", &config->pin_in3.pin);
		return ret;
	}

	k_timer_init(&data->timer, &motor_direction_toggle, NULL);
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
		.pin_pwm_b = PWM_DT_SPEC_INST_GET_BY_IDX(inst, 0),           \
		.pin_in4 = GPIO_DT_SPEC_INST_GET_BY_IDX(inst, gpios, 0),     \
		.pin_in3 = GPIO_DT_SPEC_INST_GET_BY_IDX(inst, gpios, 1),     \
	};                                                               \
	DEVICE_DT_INST_DEFINE(inst, motor_l298n_init, NULL, &data##inst, \
						  &config##inst, POST_KERNEL,                \
						  CONFIG_MOTOR_INIT_PRIORITY,                \
						  &motor_l298n_api);

DT_INST_FOREACH_STATUS_OKAY(MOTOR_L298N_DEFINE)
