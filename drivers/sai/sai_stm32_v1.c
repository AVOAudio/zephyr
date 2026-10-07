/*
 * Copyright (c) 2024-2025 Christopher Leo
 * 
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/drivers/sai/sai_stm32_v1.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT st_stm32_sai_v1

int sai_stm32_trigger(const struct device *dev, enum sai_trigger_cmd cmd)
{

}

int sai_stm32_mute(const struct device *dev, bool onOff)
{

}

int sai_stm32_setSampleRate(const struct device *dev, samplerate_t samplerate)
{

}

int sai_stm32_setBitDepth(const struct device *dev, bitdepth_t bitdepth)
{

}

 int sai_stm32_stereoEn(const struct device *dev, stereoMono_t stereoMono)
 {

 }

/* API assigned here but delcared in the sai subsystem */
static DEVICE_API(sai, sai_stm32_api) = 
{
    .trigger       = sai_stm32_trigger,
    .mute          = sai_stm32_mute,
    .setSampleRate = sai_stm32_setSampleRate,
    .setBitDepth   = sai_stm32_setBitDepth,
    .stereoEn      = sai_stm32_stereoEn,
};

static void setSaiSyncOut(const uint32_t reg, const char* syncout)
{
    uint32_t value = 0;

    if (strcmp(syncout, "syncout-a") == 0)
          value = (1 << 4);
    else if (strcmp(syncout, "syncout-b") == 0)
          value = (2 << 4);
    else if (strcmp(syncout, "nosync") == 0)
        return;
    else
        return;

    sys_update32(value, reg + 0x00);
}

static void setSaiSyncIn(const uint32_t reg, const char* syncin)
{
    uint32_t value = 0;

    if (strcmp(syncin, "sai1") == 0)
          value = (0 << 0);
    else if (strcmp(syncin, "sai2") == 0)
          value = (1 << 0);
    else if (strcmp(syncin, "sai3") == 0)
           value = (2 << 4);
    else if (strcmp(syncin, "sai4") == 0)
           value = (3 << 4);
    else
        return;

    sys_update32(value, reg + 0x00);
}

static void setSaiSync(const uint32_t reg, const char* sync)
{
    uint32_t value = 0;

    if (strcmp(sync, "async") == 0)
        value = 0 << 10;
    else if (strcmp(sync, "int-snyc") == 0)
        value = 1 << 10;
    else if (strcmp(sync, "ext-sync") == 0)
        value = 2 << 10;
    else if (strcmp(sync, "no-sync") == 0)
       return;
    else
        return;

    sys_update32(value, reg + 0x4);
}

static void setSaiProtocol(const uint32_t reg, const char* protocol)
{

    if (strcmp(protocol, "i2s") == 0)
{
    /* 2's complement */
    sys_update32(1 << 13, reg + 0x08);

    /* MSB worded */
    sys_update32(0 << 8, reg + 0x04);

    /* WS Assert one before the MSB */
    sys_update32(1 << 18, reg + 0x0C);

    /* WS is an identifying signal */
    sys_update32(1 << 16, reg + 0x0C);

    /* WS initial Assertion */
    sys_update32(0 << 17, reg + 0x0C);

    /* Frame length / FS definition */
    sys_update32(3 << 16, reg + 0x10);

    /* 2 Channels = Audio Frame */
    sys_update32(1 << 8, reg + 0x10);

    /* Bits Size = Data size */
    sys_update32(0 << 6, reg + 0x10);

    /* 0 Bit Offset */
    sys_update32(0 << 4, reg + 0x10);

    /* SCK changes on falling edge and sample on rising edge */
    sys_update32(1 << 9, reg + 0x04);

    /* Free Protocol Mode */
    sys_update32(0 << 2, reg + 0x04);
}
    else
        return;
}

static void setSaiMode(const uint32_t reg, const char* mode)
{
    uint32_t value = 0;

if (strcmp(mode, "master-rx") == 0)
{
    value = 1 << 0; 
}
else if (strcmp(mode, "master-tx") == 0)
{
    value = 0 << 0;
}
else if (strcmp(mode, "slave-rx") == 0)
{
    value = 3 << 0;
}
else if (strcmp(mode, "slave-tx") == 0)
{
    value = 2 << 0;
}
 else
    return;
    sys_update32(value, reg + 0x04);
}

static void setSaiNoMclk(const uint32_t reg, const bool nomclk)
{
    sys_update32(nomclk ? 1 << 19 : 0 << 19, reg + 0x04);
}

static int sai_sub_probe(const struct device* dev)
{
    /* Init Pinctrl */
    pinctrl_apply_state(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->pinctrl, PINCTRL_STATE_DEFAULT);

    setSaiSync(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->sync);
    setSaiProtocol(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->protocol);
    setSaiMode(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->mode);
    setSaiNoMclk(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->nomclk);
	return 0;
}

static int sai_probe(const struct device* dev)
{
	/* Init Clock */
    const struct device* clock_dev = ((struct sai_stm32_dts_config_t*)dev->config)->clockDev;

    for (uint32_t clockIdx = 0; clockIdx < ((struct sai_stm32_dts_config_t*)dev->config)->clock_num; clockIdx++)
    {
        clock_control_on(clock_dev, &((struct sai_stm32_dts_config_t*)dev->config)->clockData[clockIdx]);
    }

    setSaiSyncOut(((struct sai_stm32_dts_config_t*)dev->config)->reg, ((struct sai_stm32_dts_config_t*)dev->config)->syncout);
    setSaiSyncIn(((struct sai_stm32_dts_config_t*)dev->config)->reg, ((struct sai_stm32_dts_config_t*)dev->config)->syncin);
	return 0;
}
                               
#define SAI_SUB_INIT(node)                                                   \
    PINCTRL_DT_DEFINE(node);                                                 \
    static const struct sai_dma_dts_data_t sai_dma_dts_config##node = {      \
        .channel        = DT_DMAS_CELL_BY_IDX(node, 0, channel),             \
        .slot           = DT_DMAS_CELL_BY_IDX(node, 0, slot),                \
        .channel_config = DT_DMAS_CELL_BY_IDX(node, 0, channel_config),      \
        .features       = DT_DMAS_CELL_BY_IDX(node, 0, features),            \
    };                                                                       \
                                                                             \
    static const struct sai_sub_block_stm32_dts_config_t sub_cfg##node = {   \
        .reg      = DT_REG_ADDR(node),                                       \
        .dmas     = DEVICE_DT_GET(DT_DMAS_CTLR_BY_IDX(node, 0)),             \
        .dmaData  = &sai_dma_dts_config##node,                               \
        .pinctrl  = PINCTRL_DT_DEV_CONFIG_GET(node),                         \
        .sync     = DT_PROP(node, sync),                                     \
        .protocol = DT_PROP(node, protocol),                                 \
        .mode     = DT_PROP(node, mode),                                     \
        .nomclk   = DT_PROP(node, nomck),                                    \
    };                                                                       \
                                                                             \
    DEVICE_DT_DEFINE(node,                                                   \
        sai_sub_probe,                                                       \
        NULL,                                                                \
        NULL,                                                                \
        &sub_cfg##node,                                                      \
        PRE_KERNEL_1,                                                        \
        CONFIG_SAI_STM32_V1_INIT_PRIORITY,                                   \
        &sai_stm32_api);
                                                                            
#define SAI_INIT(inst)                                                          \
static const struct stm32_pclken sai_clock_dts_data##inst[] = {                 \
        SAI_CLOCKS(inst)                                                        \
};                                                                              \
                                                                                \
static const struct sai_stm32_dts_config_t sai_cfg##inst = {                    \
        .clockDev  = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR_BY_IDX(inst, 0)),        \
        .clockData = sai_clock_dts_data##inst,                                  \
        .clock_num = DT_INST_NUM_CLOCKS(inst),                                  \
        .syncout   = DT_PROP(DT_DRV_INST(inst), syncout),                       \
        .syncin    = DT_PROP(DT_DRV_INST(inst), syncin),                        \
        .reg       = DT_INST_REG_ADDR(inst),                                    \
};                                                                              \
                                                                                \
    DEVICE_DT_INST_DEFINE(inst,                                                 \
        sai_probe,                                                              \
        NULL,                                                                   \
        NULL,                                                                   \
        &sai_cfg##inst,                                                         \
        PRE_KERNEL_1,                                                           \
        70,                                                                     \
        NULL);                                                                  \
    DT_FOREACH_CHILD_STATUS_OKAY(DT_DRV_INST(inst), SAI_SUB_INIT)

DT_INST_FOREACH_STATUS_OKAY(SAI_INIT)