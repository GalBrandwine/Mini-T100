# Mini T100 ncs+Zephyr project

[A youtube video of the Tank](https://youtube.com/shorts/_LrFASj23rY?si=4gkAO-4bpCex8b0E)

## building

**using west cli**

```shell
west build --build-dir /home/gal/dev/nrf52840dk/mini-t100-workspace/Mini-T100/app/build /home/gal/dev/nrf52840dk/mini-t100-workspace/Mini-T100/app --pristine --board nrf52840dk/nrf52840 -- -DNCS_TOOLCHAIN_VERSION=NONE -DCONF_FILE=/home/gal/dev/nrf52840dk/mini-t100-workspace/Mini-T100/app/prj.conf -DDTC_OVERLAY_FILE=../boards/nrf/52840dk/52840dk.overlay -DBOARD_ROOT=/home/gal/dev/nrf52840dk/mini-t100-workspace/zephyr
```

## toolchanis

### Setting / Switching / Sourcing

```zsh
# Searching
nrfutil toolchain-manager search

# Installing 
nrfutil toolchain-manager install --ncs-version v2.6.2

# Listing
 nrfutil toolchain-manager list


#Inspecting environment variables
nrfutil toolchain-manager env

# Creating script
~/.nrfutil/bin/nrfutil toolchain-manager env --as-script >> v2.8.0.sh
```

# Temporary addition

1. install toolchain using the provided appimage
2. form the appimage UI - select `create environment source script`
3. save this script in ~/dev/nrf52840dk
4. cd ~/dev/nrf52840dk
5. source .env.sh
6. unset $ZEPHYR_BASE
7. call west init -m <https://github.com/GalBrandwine/Mini-T100> mini-t100-workspace
8. cd mini-t100-workspace
9. west update
10. code /home/gal/dev/nrf52840dk/mini-t100-workspace
11. nrf plugin UI

* `open existing application` select `mini-t100-workspace/Mini-T100/app`

## The Mini-T100 is based on the nRF Connect SDK example application

so there's might be little bloat.

## Getting started

Before getting started, make sure you have a proper nRF Connect SDK development environment.
Follow the official
[Installation guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/installation/install_ncs.html).

### Initialization

The first step is to initialize the workspace folder (``my-workspace``) where
the ``ncs-example-application`` and all nRF Connect SDK modules will be cloned. Run the following
command:

```shell
# initialize my-workspace for the ncs-example-application (main branch)
west init -m https://github.com/GalBrandwine/Mini-T100 --mr main mini-t100-workspace
# update nRF Connect SDK modules
cd mini-t100-workspace
west update
```

### Building and running

To build the application, run the following command:

```shell
cd mini-t100-workspace
west build -b $BOARD app
west build -p auto -b nrf52840dk/nrf52840  . -- -DCONF_FILE="/home/gal/dev/nrf52840dk/mini-t100-workspace/Mini-T100/app/prj.conf;/home/gal/dev/nrf52840dk/mini-t100-workspace/Mini-T100/app/debug.conf"
```

where `$BOARD` is the target board. My case it was 

A sample debug configuration is also provided. To apply it, run the following
command:

```shell
west build -b $BOARD app -- -DOVERLAY_CONFIG=debug.conf
```

Once you have built the application, run the following command to flash it:

```shell
west flash
```

### Testing

To execute Twister integration tests, run the following command:

```shell
west twister -T tests --integration
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
