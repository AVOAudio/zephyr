#ifndef ZEPHYR_DRIVERS_SAI_SAI_STM32_H_
#define ZEPHYR_DRIVERS_SAI_SAI_STM32_H_

#include <zephyr/device.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/clock_control/stm32_clock_control.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sai_sub_block_stm32_config {
	const struct device* dma_dev;
	const struct pinctrl_dev_config* pinCfg;
	const char* sync;
	const char* protocol;
	const char* mode;
	bool nomck;
};

struct sai_stm32_config {
	//const struct sai_sub_block_stm32_config* subConfig;
	const struct stm32_pclken *clocks;

	/* Synchronization */
	const char* syncout;
	const char* syncin;
};

struct sai_sub_stm32_data {
	/* Runtime driver state */
};


struct sai_stm32_data {
	/* Runtime driver state */
};


#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_DRIVERS_SAI_SAI_STM32_H_ */