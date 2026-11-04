zephyr_syscall_header(${ZEPHYR_BASE}/include/zephyr/drivers/sai.h)


# SAI subsystem driver library
zephyr_library()

# STM32 SAI implementation
zephyr_library_sources_ifdef(CONFIG_SAI_STM32 sai_stm32.c)
