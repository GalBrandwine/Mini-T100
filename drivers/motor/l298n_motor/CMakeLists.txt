# Copyright (c) 2024 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
message(AUTHOR_WARNING "Gal is checking if this is proceed [CONFIG_L298N_MOTOR=${CONFIG_L298N_MOTOR}]")
zephyr_library_sources_ifdef(CONFIG_L298N_MOTOR l298n_motor.c)
