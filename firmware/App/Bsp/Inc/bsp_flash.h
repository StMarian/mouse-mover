#ifndef BSP_FLASH_H
#define BSP_FLASH_H

/*
 * The last 1 KiB flash page, reserved for settings by STM32F103XX_FLASH.ld
 */

#include <stdbool.h>
#include <stddef.h>

const void *bsp_flash_settings_data(void);

size_t bsp_flash_settings_size(void);

bool bsp_flash_settings_write(const void *data, size_t size);

#endif /* BSP_FLASH_H */
