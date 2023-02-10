# SPDX-License-Identifier: Apache-2.0

if(CONFIG_SENSOR_SQUID)
# Add hello_world_driver.h to the set of global include paths.
  zephyr_include_directories(.)

  zephyr_library()
  zephyr_library_sources(
    sensor_squid_driver.c
    )
endif()
