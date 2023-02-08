# Adapted from zephyr/samples/application_development/out_of_tree_driver

# SPDX-License-Identifier: Apache-2.0

# For the sake of demonstration, we add the driver directory as a zephyr module
# by hand. If your driver is a project that's managed by west, you can remove this line.

set(BOARD nrf52833dk_nrf52833)
set(DTC_OVERLAY_FILE app.overlay)

list(APPEND ZEPHYR_EXTRA_MODULES
  ${CMAKE_CURRENT_SOURCE_DIR}/squid_sensor_module
  )

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(zephyr-oot-demo-driver-squid)

target_sources(app PRIVATE
  src/main.c
  )
