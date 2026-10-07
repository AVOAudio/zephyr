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

/**
 * @defgroup sai_interface SAI
 * @since 1.0
 * @version 1.0.0
 * @ingroup io_interfaces
 * @brief Interfaces for Serial Audio Interface (SAI) controllers.
 *
 * The SAI API provides a hardware-independent interface for configuring,
 * controlling, and accessing SAI audio blocks.
 *
 * SAI controllers typically contain one or more independent audio blocks.
 * Each block can be configured for transmit or receive operation and may
 * operate as either a master or slave depending on the hardware.
 *
 * The API provides functions for:
 * - Configuring an SAI audio block.
 * - Retrieving the current SAI configuration.
 * - Starting and stopping SAI operation.
 *
 * @{
 */

#include <zephyr/types.h>
#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supported SAI sample rates.
 */
typedef enum samplerate_t {
	/** 384 kHz sample rate. */
	SR_384KHZ,

	/** 192 kHz sample rate. */
	SR_192KHZ,

	/** 96 kHz sample rate. */
	SR_96KHZ,

	/** 48 kHz sample rate. */
	SR_48KHZ,
} samplerate_t;

/**
 * @brief Supported SAI audio sample widths.
 */
typedef enum bitdepth_t {
	/** 32-bit audio samples. */
	BD_32BIT,

	/** 24-bit audio samples. */
	BD_24BIT,

	/** 16-bit audio samples. */
	BD_16BIT,
} bitdepth_t;

/**
 * @brief SAI audio block selection.
 *
 * STM32 SAI controllers typically contain two independent audio blocks,
 * referred to as block A and block B.
 */
/**
 * @brief SAI channel configuration.
 */
typedef enum stereoMono_t {
	/** Mono audio. */
	AUDIO_MONO,

	/** Stereo audio. */
	AUDIO_STEREO,
} stereoMono_t;

/**
 * @brief SAI control commands.
 */
typedef enum sai_trigger_cmd {
	/** Start SAI audio operation. */
	SAI_START,

	/** Stop SAI audio operation. */
	SAI_STOP,
} sai_trigger_cmd;

/**
 * @brief SAI driver API.
 *
 * Provides the driver-specific implementation of the SAI subsystem API.
 */
__subsystem struct sai_driver_api {
	/*
	 * @brief Start or stop SAI operation.
	 *
	 * @param dev Pointer to the SAI device.
	 * @param cmd Trigger command to execute.
	 *
	 * @return 0 on success, otherwise a negative error code.
	 */
	int (*trigger)(const struct device *dev, enum sai_trigger_cmd cmd);
	int (*mute)(const struct device *dev, bool onOff);
	int (*setSampleRate)(const struct device *dev, samplerate_t samplerate);
	int (*setBitDepth)(const struct device *dev, bitdepth_t bitdepth);
	int (*stereoEn)(const struct device *dev, stereoMono_t stereoMono);
};
/**
 * @brief Start or stop SAI operation.
 *
 * @param dev Pointer to the SAI device.
 * @param cmd Trigger command to execute.
 *
 * @return 0 on success, otherwise a negative error code.
 */
__syscall int sai_trigger(const struct device *dev, enum sai_trigger_cmd cmd);

/**
 * @brief Kernel-side implementation of sai_trigger().
 *
 * @param dev Pointer to the SAI device.
 * @param cmd Trigger command to execute.
 *
 * @return 0 on success, otherwise a negative error code.
 */
static inline int z_impl_sai_trigger(const struct device *dev, enum sai_trigger_cmd cmd)
{
	return DEVICE_API_GET(sai, dev)->trigger(dev, cmd);
}

__syscall int sai_mute(const struct device *dev, bool onOff);
__syscall int setSampleRate(const struct device *dev, samplerate_t samplerate);
__syscall int setBitDepth(const struct device *dev,  bitdepth_t bitdepth);
__syscall int stereoEn(const struct device *dev, stereoMono_t stereoMono);

static inline int z_impl_sai_mute(const struct device *dev, bool onOff)
{
	return DEVICE_API_GET(sai, dev)->trigger(dev, onOff);
}
static inline int z_impl_setSampleRate(const struct device *dev, samplerate_t samplerate)
{
	return DEVICE_API_GET(sai, dev)->trigger(dev, samplerate);
}
static inline int z_impl_setBitDepth(const struct device *dev, bitdepth_t bitdepth)
{
	return DEVICE_API_GET(sai, dev)->trigger(dev, bitdepth);
}
static inline int z_impl_stereoEn(const struct device *dev, stereoMono_t stereoMono)
{
	return DEVICE_API_GET(sai, dev)->trigger(dev, stereoMono);
}

#ifdef __cplusplus
}
#endif

#include <zephyr/syscalls/sai.h>

#endif /* ZEPHYR_INCLUDE_DRIVERS_SAI_H_ */

/**
 * @}
 */