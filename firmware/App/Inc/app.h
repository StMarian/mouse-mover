#ifndef APP_H
#define APP_H

// Main-loop context.
void app_init(void);
void app_run_iteration(void);

// Interrupt context.
void app_countdown_tick_isr(void); // TIM2, 1 Hz
void app_display_tick_isr(void);   // TIM3, 100 Hz

#endif /* APP_H */
