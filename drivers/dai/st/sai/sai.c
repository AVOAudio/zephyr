/*
 * Copyright (c) 2024-2025 Christopher Leo
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/dai/st/sai/sai.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT st_dai_sai

/* Internal Helper Functions */
static void setSaiSyncOut(uint32_t reg, const char *syncout);
static void setSaiSyncIn(uint32_t reg, const char *syncin);
static void setSaiSync(uint32_t reg, const char *sync);
static void setSaiProtocol(uint32_t reg, const char *protocol);
static void setSaiMode(uint32_t reg, const char *mode);
static void setSaiNoMclk(uint32_t reg, bool nomclk);
static void sai_setSampleRate(const struct device *dev, uint32_t mckdiv, bool osr);
static void sai_setBitDepth(const struct device *dev, uint32_t bitdepth);

/* Probe functions */
static int sai_sub_probe(const struct device *dev);
static int sai_probe(const struct device *dev);

/* DAI APIs */
static int sai_trigger(const struct device *dev, enum dai_dir dir, enum dai_trigger_cmd cmd);
static int sai_config_set(const struct device *dev, const struct dai_config *cfg, const void *bespoke_cfg, size_t size);

int sai_trigger(const struct device *dev, enum dai_dir dir, enum dai_trigger_cmd cmd)
{
    switch (cmd)
    {
        case DAI_TRIGGER_START:
            sys_update32(1 << 16, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg + 0x04, 1 << 16);
            break;
        case DAI_TRIGGER_STOP:
            sys_update32(0 << 16, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg + 0x04, 1 << 16);
            break;
        default:
            return -1;
    };

    return 0;
}

int sai_config_set(const struct device *dev, const struct dai_config *cfg, const void *bespoke_cfg, size_t size)
{
    if (dev == NULL)
        return -1;
    if (cfg == NULL)
        return -1;

    struct sai_sub_block_stm32_dts_config_t* config = dev->config;
    struct st_sai_bespoke_cfg* bespokeCfg = (struct st_sai_bespoke_cfg*)bespoke_cfg;

    uint32_t samplerate = cfg->rate;
    uint32_t bitDepth = cfg->word_size;

    sai_setBitDepth(dev, bitDepth);
    sai_setSampleRate(dev, bespokeCfg->mckdiv, bespokeCfg->osr);
}

/* API assigned here but delcared in the sai subsystem */
static DEVICE_API(dai, dai_api) =
{
    .probe = NULL,
    .remove = NULL,
    .config_set = sai_config_set,
    .config_get = NULL,
    .config_update = NULL,
    .get_properties = NULL,
    .get_properties_copy = NULL,
    .trigger = sai_trigger,
    .ts_config = NULL,
    .ts_start = NULL,
    .ts_stop = NULL,
    .ts_get = NULL,
};

void sai_setSampleRate(const struct device *dev, uint32_t mckdiv, bool osr)
{
    if (dev == NULL)
        return;
    if (mckdiv < 1 || mckdiv > 64)
        return;

    struct sai_sub_block_stm32_dts_config_t* config = dev->config;

    sys_update32((mckdiv << 20) | (osr ? 1 << 26 : 0 << 26), config->reg + 0x04, 0x7F00000);
}

void sai_setBitDepth(const struct device *dev, uint32_t bitdepth)
{
    uint32_t value = 0;

    switch (bitdepth)
    {
        case 32:
            value = (7 << 5);
            break;
        case 24:
            value = (6 << 5);
            break;
        case 16:
            value = (4 << 5);
            break;
        default:
            return;
    }

    sys_update32(value, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg + 0x04, 0xE0);
}

void setSaiSyncOut(const uint32_t reg, const char* syncout)
{
    uint32_t value = 0;

    if (strcmp(syncout, "sync-a") == 0)
          value = (1 << 4);
    else if (strcmp(syncout, "sync-b") == 0)
          value = (2 << 4);
    else if (strcmp(syncout, "nosync") == 0)
        return;
    else
        return;

    sys_update32(value, reg + 0x00, 0x30);
}

void setSaiSyncIn(const uint32_t reg, const char* syncin)
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

    sys_update32(value, reg + 0x00, 0x3);
}

void setSaiSync(const uint32_t reg, const char* sync)
{
    uint32_t value = 0;

    if (strcmp(sync, "async") == 0)
        value = 0 << 10;
    else if (strcmp(sync, "int-sync") == 0)
        value = 1 << 10;
    else if (strcmp(sync, "ext-sync") == 0)
        value = 2 << 10;
    else if (strcmp(sync, "no-sync") == 0)
       return;
    else
        return;

    sys_update32(value, reg + 0x4, 0xC00);
}

void setSaiProtocol(const uint32_t reg, const char* protocol)
{
    if (strcmp(protocol, "i2s") == 0)
    {
        sys_update32(1 << 13, reg + 0x8, 1 << 13);
        sys_update32(0x200, reg + 0x04,  0x30C);
        sys_update32(0x51F3F, reg + 0xC, 0x77FFF);
        sys_update32(0x30100, reg + 0x10, 0xFFFF0FDF);
    }
    else
        return;
}

void setSaiMode(const uint32_t reg, const char* mode)
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

    sys_update32(value, reg + 0x04, 0x3);
}

void setSaiNoMclk(const uint32_t reg, const bool nomclk)
{
    sys_update32(nomclk << 19, reg + 0x04, 1 << 19);
}

int sai_sub_probe(const struct device* dev)
{
    /* Init Pinctrl */
    pinctrl_apply_state(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->pinctrl, PINCTRL_STATE_DEFAULT);

    setSaiSync(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->sync);
    setSaiProtocol(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->protocol);
    setSaiMode(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->mode);
    setSaiNoMclk(((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->nomclk);

    sys_update32(1 << 17, ((struct sai_sub_block_stm32_dts_config_t*)dev->config)->reg + 0x04, 1 << 17);
    return 0;
}

int sai_probe(const struct device* dev)
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
    static const struct sai_sub_block_stm32_dts_config_t sub_cfg##node =     \
    {                                                                        \
        .reg      = DT_REG_ADDR(node),                                       \
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
        NULL,                          			                             \
        &sub_cfg##node,                                                      \
        POST_KERNEL,                                                         \
        80,                                                                  \
        &dai_api);

#define SAI_INIT(inst)                                                          \
static const struct stm32_pclken sai_clock_dts_data##inst[] =                   \
{                                                                               \
        SAI_CLOCKS(inst)                                                        \
};                                                                              \
                                                                                \
static const struct sai_stm32_dts_config_t sai_cfg##inst =                      \
{                                                                               \
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
        POST_KERNEL,                                                            \
        70,                  					                                \
        NULL);                                                                  \
    DT_FOREACH_CHILD_STATUS_OKAY(DT_DRV_INST(inst), SAI_SUB_INIT)

DT_INST_FOREACH_STATUS_OKAY(SAI_INIT)
