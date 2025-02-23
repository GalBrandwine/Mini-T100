/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_DRIVERS_MOTOR_H_
#define APP_DRIVERS_MOTOR_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>

enum direction
{
	LEFT,
	RIGHT,
};

/**
 * @defgroup drivers_motor Motor drivers
 * @ingroup drivers
 * @{
 *
 * @brief A custom driver class to control motor
 *
 * This driver class is based on the provided example of how to create custom driver
 * classes. It provides an interface to turn a motor.
 */

/**
 * @defgroup drivers_motor_ops Motor driver operations
 * @{
 *
 * @brief Operations of the motor driver class.
 *
 * Each driver class tipically provides a set of operations that need to be
 * implemented by each driver. These are used to implement the public API. If
 * support for system calls is needed, the operations structure must be tagged
 * with `__subsystem` and follow the `${class}_driver_api` naming scheme.
 */

/** @brief Motor driver class operations */
__subsystem struct motor_driver_api
{
	/**
	 * @brief Configure the Motor speed.
	 *
	 * @param dev Motor device instance.
	 * @param speed desired motor rotation speed, 0 to
	 * disable motor.
	 *
	 * @retval 0 if successful.
	 * @retval -EINVAL if @p speed can not be set.
	 * @retval -errno Other negative errno code on failure.
	 */
	int (*set_speed)(const struct device *dev, char speed);

	/**
	 * @brief Turn Motor to given direction, with given speed.
	 *
	 * @param dev Motor device instance.
	 * @param
	 * @param speed desired motor rotation direction and speed, 0 to
	 * disable motor.
	 *
	 * @retval 0 if successful.
	 * @retval -EINVAL if @p speed can not be set.
	 * @retval -errno Other negative errno code on failure.
	 */
	int (*set_direction_speed)(const struct device *dev, enum direction direction, char speed);
};

/** @} */

/**
 * @defgroup drivers_motor_api Motor driver API
 * @{
 *
 * @brief Public API provided by the motor driver class.
 *
 * The public API is the interface that is used by applications to interact with
 * devices that implement the blink driver class. If support for system calls is
 * needed, functions accessing device fields need to be tagged with `__syscall`
 * and provide an implementation that follows the `z_impl_${function_name}`
 * naming scheme.
 */

/**
 * @brief Configure the motor speed
 *
 * @param dev Motor device instance
 * @param speed desired motor rotation speed
 *
 * @retval 0 if successful.
 * @retval -EINVAL if @p period_ms can not be set.
 * @retval -errno Other negative errno code on failure.
 */
__syscall int motor_set_speed(const struct device *dev,
							  char speed);

static inline int z_impl_motor_set_speed(const struct device *dev,
										 char speed)
{
	const struct motor_driver_api *api =
		(const struct motor_driver_api *)dev->api;

	return api->set_speed(dev, speed);
}

/**
 * @brief Turn Motor off.
 *
 * This is a convenience function to turn off the motor. It also shows
 * how to create convenience functions that re-use other driver functions, or
 * driver operations, to provide a higher-level API.
 *
 * @param dev Blink device instance.
 *
 * @return See motor_set_speed().
 */
static inline int motor_off(const struct device *dev)
{
	return motor_set_speed(dev, 0);
}

#include <syscalls/motor.h>

/** @} */

/** @} */

#endif /* APP_drivers_motor_H_ */
