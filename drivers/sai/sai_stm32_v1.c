/*
 * Copyright (c) 2024-2025 Christopher Leo
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sai_stm32_v1.h"
#include <zephyr/drivers/sai.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT st_stm32_sai_v1

LOG_MODULE_REGISTER(sai_stm32_sai_v1);

static DEVICE_API(sai, sai_stm32_api) = 
{
    .trigger = NULL,
    .mute = NULL,
    .setSampleRate = NULL,
    .setBitDepth = NULL,
    .stereoEn = NULL,
};

static int sai_sub_init(const struct device* dev)
{
	LOG_DBG("We're so double in");

	return 0;
}

static int sai_init(const struct device* dev)
{
	LOG_DBG("We're so in");

	return 0;
}

#define SAI_SUB_INIT(node)                                                   \
    PINCTRL_DT_DEFINE(node);                                                  \
                                                                                \
    static struct sai_sub_stm32_data sub_data_##node;                        \
                                                                                \
    static const struct sai_sub_block_stm32_config sub_cfg_##node = {        \
        .dma_dev = DEVICE_DT_GET(DT_DMAS_CTLR_BY_IDX(node, 0)),              \
        .pinCfg = PINCTRL_DT_DEV_CONFIG_GET(node),                           \
        .sync = DT_PROP(node, sync),                                         \
        .protocol = DT_PROP(node, protocol),                                 \
        .mode = DT_PROP(node, mode),                                         \
        .nomck = DT_PROP(node, nomck),                                       \
    };                                                                       \
                                                                                \
    DEVICE_DT_DEFINE(node,                                                   \
        sai_sub_init,                                                        \
        NULL,                                                                \
        NULL,                                                    \
        &sub_cfg_##node,                                                     \
        PRE_KERNEL_1,                                                         \
        50,                                            \
        &sai_stm32_api);


#define SAI_INIT(inst)                                                       \
    static const struct stm32_pclken sai_clocks_##inst[] =                  \
        STM32_DT_INST_CLOCKS(inst);                                         \
                                                                                \
    static const struct sai_stm32_config sai_cfg_##inst = {                  \
        .clocks = sai_clocks_##inst,                                         \
        .syncout = DT_PROP(DT_DRV_INST(inst), syncout),                     \
        .syncin = DT_PROP(DT_DRV_INST(inst), syncin),                       \
    };                                                                       \
                                                                                \
    static struct sai_stm32_data sai_data_##inst;                            \
                                                                                \
    DEVICE_DT_INST_DEFINE(inst,                                               \
        sai_init,                                                            \
        NULL,                                                                \
        NULL,                                                    \
        &sai_cfg_##inst,                                                     \
        PRE_KERNEL_1,                                                         \
        50,                                            \
        NULL);                                                     \
                                                                                \
    DT_INST_FOREACH_CHILD_STATUS_OKAY(inst, SAI_SUB_INIT)


DT_INST_FOREACH_STATUS_OKAY(SAI_INIT)