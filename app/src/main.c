/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include <app/drivers/blink.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define prop DT_PROP(DT_NODELABEL(left_motor_gpio_nodelabel), gals_property)

// static const struct pwm_dt_spec motor_pwm_pin = PWM_DT_SPEC_GET(DT_NODELABEL(motor_pwm_pin));
#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)
#define SLEEP_DELTA_MSEC 1000U
#define NUM_STEPS 50U
#define SLEEP_MSEC 25U
#define SLEEP_SEC 1U

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_NODELABEL(motor_b_in_3)
#define LED1_NODE DT_NODELABEL(motor_b_in_4)
#define LED2_NODE DT_NODELABEL(motor_en_b)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec motor_b_in_3_pin = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec motor_b_in_4_pin = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec motor_en_b_pin = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

struct k_timer timer;

static void motor_direction_toggle(struct k_timer *timer)
{
	// const struct device *dev = k_timer_user_data_get(timer);
	// const struct blink_gpio_led_config *config = dev->config;
	int ret;
	LOG_DBG("timer expired");
	ret = gpio_pin_toggle_dt(&motor_b_in_3_pin);
	if (ret < 0)
	{
		LOG_ERR("Could not toggle motor_b_in_3_pin (%d)", ret);
	}
	ret = gpio_pin_toggle_dt(&motor_b_in_4_pin);
	if (ret < 0)
	{
		LOG_ERR("Could not toggle motor_b_in_4_pin (%d)", ret);
	}
}

#define BLINK_PERIOD_MS_STEP 100U
#define BLINK_PERIOD_MS_MAX 1000U
#define MOTOR_MAX_SPEED 255
int main(void)
{
	int ret;
	unsigned int period_ms = BLINK_PERIOD_MS_MAX;
	const struct device *sensor, *blink;
	struct sensor_value last_val = {0}, val;

	printk("Zephyr Example Application %s\n", APP_VERSION_STRING);
	printk("Got property: %s\n", prop);
	// if (!pwm_is_ready_dt(&motor_pwm_pin))
	// {
	// 	printk("Error: PWM device %s is not ready\n", motor_pwm_pin.dev->name);
	// 	return 0;
	// }
	uint8_t dir = 1U;
	uint32_t pulse_width = 0U;
	// uint32_t step = motor_pwm_pin.period / NUM_STEPS;

	if (!gpio_is_ready_dt(&motor_en_b_pin))
	{
		printk("Error: GPIO %s is not ready\n", motor_en_b_pin.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&motor_en_b_pin, GPIO_OUTPUT_HIGH);
	if (ret < 0)
	{
		return 0;
	}

	if (!gpio_is_ready_dt(&motor_b_in_3_pin))
	{
		printk("Error: GPIO %s is not ready\n", motor_b_in_3_pin.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&motor_b_in_3_pin, GPIO_OUTPUT_HIGH);
	if (ret < 0)
	{
		return 0;
	}

	if (!gpio_is_ready_dt(&motor_b_in_4_pin))
	{
		printk("Error: GPIO %s is not ready\n", motor_b_in_4_pin.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&motor_b_in_4_pin, GPIO_OUTPUT_LOW);
	if (ret < 0)
	{
		return 0;
	}

	sensor = DEVICE_DT_GET(DT_NODELABEL(example_sensor));
	if (!device_is_ready(sensor))
	{
		LOG_ERR("Sensor not ready");
		return 0;
	}

	// blink = DEVICE_DT_GET(DT_NODELABEL(blink_led));
	// if (!device_is_ready(blink)) {
	// 	LOG_ERR("Blink LED not ready");
	// 	return 0;
	// }

	// ret = blink_off(blink);
	// if (ret < 0) {
	// 	LOG_ERR("Could not turn off LED (%d)", ret);
	// 	return 0;
	// }

	// printk("Use the sensor to change LED blinking period\n");

	// while (1) {
	// 	ret = sensor_sample_fetch(sensor);
	// 	if (ret < 0) {
	// 		LOG_ERR("Could not fetch sample (%d)", ret);
	// 		return 0;
	// 	}

	// 	ret = sensor_channel_get(sensor, SENSOR_CHAN_PROX, &val);
	// 	if (ret < 0) {
	// 		LOG_ERR("Could not get sample (%d)", ret);
	// 		return 0;
	// 	}

	// 	if ((last_val.val1 == 0) && (val.val1 == 1)) {
	// 		if (period_ms == 0U) {
	// 			period_ms = BLINK_PERIOD_MS_MAX;
	// 		} else {
	// 			period_ms -= BLINK_PERIOD_MS_STEP;
	// 		}

	// 		printk("Proximity detected, setting LED period to %u ms\n",
	// 		       period_ms);
	// 		// blink_set_period_ms(blink, period_ms);
	// 	}

	// 	last_val = val;

	// 	k_sleep(K_MSEC(100));
	// }

	uint32_t period = 1000000, pulse = 1000000;
	// ret = pwm_set_dt(&motor_pwm_pin, period, pulse);
	// ret = pwm_set_pulse_dt(&motor_pwm_pin, motor_pwm_pin.period);
	// printk("Using pulse_width %d motor_pwm_pin.period %d step %d\n", pulse_width, motor_pwm_pin.period, step);
	// printk("Using pulse width %d%%\n", 100 * pulse_width / motor_pwm_pin.period);
	// while (1)
	// {
	// 	ret = pwm_set_pulse_dt(&motor_pwm_pin, pulse_width);
	// 	// ret = pwm_set_dt(&motor_pwm_pin,)
	// 	if (ret < 0)
	// 	{
	// 		printk("Error %d: failed to set pulse width\n", ret);
	// 		return 0;
	// 	}
	// 	printk("Using pulse_width %d motor_pwm_pin.period %d step %d\n", pulse_width, motor_pwm_pin.period, step);
	// 	printk("Using pulse width %d%%\n", 100 * pulse_width / motor_pwm_pin.period);
	// 	if (dir)
	// 	{
	// 		pulse_width += step;
	// 		if (pulse_width >= motor_pwm_pin.period)
	// 		{
	// 			pulse_width = motor_pwm_pin.period - step;
	// 			dir = 0U;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if (pulse_width >= step)
	// 		{
	// 			pulse_width -= step;
	// 		}
	// 		else
	// 		{
	// 			pulse_width = step;
	// 			dir = 1U;
	// 		}
	// 	}

	// 	k_sleep(K_SECONDS(SLEEP_SEC));
	// }

	k_timer_init(&timer, motor_direction_toggle, NULL);
	// k_timer_user_data_set(&data->timer, (void *)dev);

	k_timer_start(&timer, K_MSEC(500), K_MSEC(3000));
	return 0;
}
