# Mini T100 Tank project

![Iimage of mini-t100 tank chasis](https://github.com/GalBrandwine/Mini-T100/blob/main/doc/minit-100%20chasis.png "Minit T100 Chasis")

A fun project for learning Zephyr Drivers API and implementing a driver.

[A youtube video of the Tank](https://youtube.com/shorts/_LrFASj23rY?si=4gkAO-4bpCex8b0E)

Introducing new API:

```C
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
	 * @param speed desired motor rotation direction and speed, 0 to
	 * disable motor.
	 *
	 * @retval 0 if successful.
	 * @retval -EINVAL if @p speed can not be set.
	 * @retval -errno Other negative errno code on failure.
	 */
	int (*set_direction_speed)(const struct device *dev, enum direction direction, char speed);
};
```

## The Mini-T100 is based on the nRF Connect SDK example application

so there's might be little bloat.

## Getting started

I developed using nrf5820dk:
Before getting started, make sure you have a proper nRF Connect SDK development environment.
Follow the official [Installation guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/installation/install_ncs.html).

### Initialization

Once Zephyr and nRF ncs installed. Use nrfutil for installing & sourcing all ENV-VARS:

```zsh
# Searching
nrfutil toolchain-manager search

# Installing 
nrfutil toolchain-manager install --ncs-version v2.8.0

# Listing
 nrfutil toolchain-manager list

#Inspecting environment variables
nrfutil toolchain-manager env

# Creating script
nrfutil toolchain-manager env --as-script >> v2.8.0.sh
```

## Fetching my application

```shell
# Initialize mini-t100-workspace for the Tank app (main branch)
west init -m https://github.com/GalBrandwine/Mini-T100 --mr main mini-t100-workspace

# update nRF Connect SDK modules
cd mini-t100-workspace

west update # Might take a while
```

## Building

Using west cli:

```shell
# Sourcing
source ./v2.8.0.sh

# Building (I've included the example for my CONF-FILES and DTS OVERLAY)
west build -p auto -b nrf52840dk/nrf52840  --build-dir build_nrf52840dk app -- -DCONF_FILE="prj.conf"
```

A sample debug configuration is also provided. To apply it, run the following
command:

```shell
west build -p auto -b nrf52840dk/nrf52840  --build-dir build_nrf52840dk app -- -DCONF_FILE="prj.conf" -DOVERLAY_CONFIG="debug.conf"
```

Once you have built the application, run the following command to flash it:

```shell
west flash --build-dir ./build_nrf52840dk --skip-rebuild
```

### Documentation

A minimal documentation setup is provided for Doxygen and Sphinx. To build the
documentation first change to the ``doc`` folder:

```shell
cd doc
```

Before continuing, check if you have Doxygen installed. It is recommended to
use the same Doxygen version used in [CI](.github/workflows/docs.yml). To
install Sphinx, make sure you have a Python installation in place and run:

```shell
pip install -r requirements.txt
```

API documentation (Doxygen) can be built using the following command:

```shell
doxygen
```

The output will be stored in the ``_build_doxygen`` folder. Similarly, the
Sphinx documentation (HTML) can be built using the following command:

```shell
make html
```

The output will be stored in the ``_build_sphinx`` folder. You may check for
other output formats other than HTML by running ``make help``.
