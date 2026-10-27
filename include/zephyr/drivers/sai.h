/*
 * Copyright (c) 2026 Christopher Leo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @ingroup sai_interface
 * @brief Main header file for the SAI (Serial Audio Interface) driver API.
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_SAI_H_
#define ZEPHYR_INCLUDE_DRIVERS_SAI_H_

#include <zephyr/types.h>
#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bitdepth_t {
	BD_32BIT,
	BD_24BIT,
	BD_16BIT,
} bitdepth_t;

typedef enum stereoMono_t {
	AUDIO_MONO,
	AUDIO_STEREO,
} stereoMono_t;

typedef enum sai_trigger_cmd {
	SAI_START,
	SAI_STOP,
} sai_trigger_cmd;

__subsystem struct sai_driver_api 
{
	int (*trigger)(const struct device *dev, enum sai_trigger_cmd cmd);
	int (*mute)(const struct device *dev, bool onOff);
	int (*setSampleRate)(const struct device *dev, uint32_t mckdiv, bool osr);
	int (*setBitDepth)(const struct device *dev, bitdepth_t bitdepth);
	int (*stereoEn)(const struct device *dev, stereoMono_t stereoMono);
};

__syscall int mute(const struct device *dev, bool onOff);
__syscall int setSampleRate(const struct device *dev, uint32_t mckdiv, bool osr);
__syscall int setBitDepth(const struct device *dev,  bitdepth_t bitdepth);
__syscall int stereoEn(const struct device *dev, stereoMono_t stereoMono);
__syscall int trigger(const struct device *dev, enum sai_trigger_cmd cmd);

static inline int z_impl_mute(const struct device *dev, bool onOff)
{
	return DEVICE_API_GET(sai, dev)->mute(dev, onOff);
}
static inline int z_impl_setSampleRate(const struct device *dev,  uint32_t mckdiv, bool osr)
{
	return DEVICE_API_GET(sai, dev)->setSampleRate(dev, mckdiv, osr);
}
static inline int z_impl_setBitDepth(const struct device *dev, bitdepth_t bitdepth)
{
	return DEVICE_API_GET(sai, dev)->setBitDepth(dev, bitdepth);
}
static inline int z_impl_stereoEn(const struct device *dev, stereoMono_t stereoMono)
{
	return DEVICE_API_GET(sai, dev)->stereoEn(dev, stereoMono);
}
static inline int z_impl_trigger(const struct device *dev, enum sai_trigger_cmd cmd)
{
	return DEVICE_API_GET(sai, dev)->trigger(dev, cmd);
}

#ifdef __cplusplus
}
#endif

#include <zephyr/syscalls/sai.h>

#endif /* ZEPHYR_INCLUDE_DRIVERS_SAI_H_ */

/**
 * @}
 */
