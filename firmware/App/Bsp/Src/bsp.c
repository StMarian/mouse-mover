#include "bsp.h"

#include "main.h"

#define SEGMENT_PORT DGT_A_GPIO_Port
#define SEGMENT_PINS ((uint32_t)(DGT_A_Pin | DGT_B_Pin | DGT_C_Pin | DGT_D_Pin | DGT_E_Pin | DGT_F_Pin | DGT_G_Pin))
#define DIGIT_PORT   DGT_SLT_0_GPIO_Port // both commons are on GPIOB

// BSRR: the low half sets pins, the high half resets them, in one atomic write.
#define BSRR_SET(pins)   ((uint32_t)(pins))
#define BSRR_RESET(pins) ((uint32_t)(pins) << 16)

#define MOTOR_PORT LED_GPIO_Port
#define MOTOR_PIN  LED_Pin

/*
 * Independent watchdog, clocked by the LSI, ~40 kHz
 * 40 kHz / 32 = 1.25 kHz; 2500 counts = ~2 s
 */
#define IWDG_KEY_ACCESS    0x5555u
#define IWDG_KEY_RELOAD    0xAAAAu
#define IWDG_KEY_START     0xCCCCu
#define IWDG_PRESCALER_32  3u
#define IWDG_RELOAD_COUNTS 2500u

static void watchdog_start(void)
{
    IWDG->KR = IWDG_KEY_START;
    IWDG->KR = IWDG_KEY_ACCESS;
    IWDG->PR = IWDG_PRESCALER_32;
    IWDG->RLR = IWDG_RELOAD_COUNTS - 1u;

    while (IWDG->SR != 0u)
    {
        // The new values cross into the LSI domain in a few LSI cycles.
    }

    IWDG->KR = IWDG_KEY_RELOAD;
}

void bsp_init(void)
{
#ifdef DEBUG
    // Keep the debugger usable: clocks stay on in WFI, and the watchdog pauses while the core is halted.
    DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_IWDG_STOP;
#endif

    bsp_safe_state();
    watchdog_start();
}

uint32_t bsp_millis(void)
{
    return HAL_GetTick();
}

void bsp_idle(void)
{
    __WFI();
}

void bsp_watchdog_kick(void)
{
    IWDG->KR = IWDG_KEY_RELOAD;
}

bool bsp_button_is_pressed(void)
{
    // Active low (R4 pull-up). Debounced in hardware: R4/C4 (~1 ms)
    return (BTN_GPIO_Port->IDR & BTN_Pin) == 0u;
}

void bsp_motor_set(bool on)
{
    MOTOR_PORT->BSRR = on ? BSRR_SET(MOTOR_PIN) : BSRR_RESET(MOTOR_PIN);
}

void bsp_display_show(uint8_t digit, uint8_t segments)
{
    // Segments off to prevent ghosting.
    SEGMENT_PORT->BSRR = BSRR_RESET(SEGMENT_PINS);

    if (digit == 0u)
    {
        DIGIT_PORT->BSRR = BSRR_RESET(DGT_SLT_0_Pin) | BSRR_SET(DGT_SLT_1_Pin);
    }
    else
    {
        DIGIT_PORT->BSRR = BSRR_RESET(DGT_SLT_1_Pin) | BSRR_SET(DGT_SLT_0_Pin);
    }

    SEGMENT_PORT->BSRR = BSRR_SET((uint32_t)segments & SEGMENT_PINS);
}

void bsp_display_off(void)
{
    SEGMENT_PORT->BSRR = BSRR_RESET(SEGMENT_PINS);
}

void bsp_safe_state(void)
{
    bsp_motor_set(false);
    bsp_display_off();
}
