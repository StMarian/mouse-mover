#include "bsp_flash.h"

#include <stdint.h>
#include <string.h>

#include "main.h"

// Defined in STM32F103XX_FLASH.ld. Linker symbols are addresses.
extern uint8_t _settings_page_start[];
extern uint8_t _settings_page_size[];

const void *bsp_flash_settings_data(void)
{
    return _settings_page_start;
}

size_t bsp_flash_settings_size(void)
{
    return (size_t)(uintptr_t)_settings_page_size;
}

bool bsp_flash_settings_write(const void *data, size_t size)
{
    const uint32_t page = (uint32_t)(uintptr_t)_settings_page_start;

    if ((data == NULL) || (size == 0u) || ((size % 2u) != 0u) || (size > bsp_flash_settings_size()))
    {
        return false;
    }

    if (HAL_FLASH_Unlock() != HAL_OK)
    {
        return false;
    }

    FLASH_EraseInitTypeDef erase = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .PageAddress = page,
        .NbPages = 1u,
    };
    uint32_t page_error = 0u;

    bool ok = (HAL_FLASHEx_Erase(&erase, &page_error) == HAL_OK) && (page_error == 0xFFFFFFFFu);

    // The F1 programs flash 16 bits at a time.
    const uint8_t *bytes = data;
    for (size_t offset = 0u; ok && (offset < size); offset += 2u)
    {
        uint16_t halfword;
        memcpy(&halfword, &bytes[offset], sizeof halfword);
        ok = (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, page + offset, halfword) == HAL_OK);
    }

    HAL_FLASH_Lock();

    return ok && (memcmp(bsp_flash_settings_data(), data, size) == 0);
}
