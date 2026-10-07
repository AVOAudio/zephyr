#ifndef ZEPHYR_DRIVERS_SAI_SAI_STM32_V1_H_
#define ZEPHYR_DRIVERS_SAI_SAI_STM32_V1_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/device.h>
#include <zephyr/sys/util.h>

#include <zephyr/drivers/clock_control/stm32_clock_control.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/dma/dma_stm32.h>

#include <zephyr/drivers/sai.h>

struct sai_dma_dts_data_t {
    const uint32_t channel;
    const uint32_t slot;
    const uint32_t channel_config;
    const uint32_t features;
};

struct sai_sub_block_stm32_dts_config_t {
    const uint32_t reg;
    /* DMA DTS */
    const struct device* dmas;
    const struct sai_dma_dts_data_t* dmaData;

    /* PINCTRL DTS */
    const struct pinctrl_dev_config* pinctrl;

    /* SAI SUB BLOCK DTS */
    const char* sync;
    const char* protocol;
    const char* mode;
    const bool nomclk;
};

struct sai_stm32_dts_config_t {
    /* CLOCK DTS */
    const struct device* clockDev;
    const struct stm32_pclken* clockData;
    const uint32_t clock_num;

    /* SAI BLOCK DTS */
    const uint32_t reg;
    const char* syncout;
    const char* syncin;
};

#define SAI_CLOCK_CONFIG(idx, inst)                                      \
    {                                                                    \
        .bus = DT_INST_CLOCKS_CELL_BY_IDX(inst, idx, bus),               \
        .enr = DT_INST_CLOCKS_CELL_BY_IDX(inst, idx, bits),              \
    }

#define SAI_CLOCKS(inst)                                                 \
    LISTIFY(DT_INST_NUM_CLOCKS(inst), SAI_CLOCK_CONFIG, (,), inst)

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_DRIVERS_SAI_SAI_STM32_V1_H_ */

