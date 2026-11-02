#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// Length of one countdown unit. CMakeLists.txt sets it to 1 s for Debug builds
#ifndef COUNTDOWN_UNIT_SECONDS
#define COUNTDOWN_UNIT_SECONDS 60u
#endif

#define COUNTDOWN_PERIOD_OPTIONS       2u, 4u, 9u, 14u, 59u
#define COUNTDOWN_DEFAULT_PERIOD_INDEX 2u

#define MOTOR_PULSE_SECONDS    1u
#define CONFIG_TIMEOUT_SECONDS 5u // settings mode saves and closes after this long without a press
#define BUTTON_LONG_PRESS_MS   500u

#endif /* APP_CONFIG_H */
