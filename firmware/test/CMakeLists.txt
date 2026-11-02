cmake_minimum_required(VERSION 3.22)

# Host unit tests for App/. Everything below App/Bsp is replaced by fakes/fake_bsp.c.
# From firmware/:
#   cmake -S test -B build/tests && cmake --build build/tests && ctest --test-dir build/tests

project(mouse_mover_tests LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TEST_SANITIZERS "Build the tests with AddressSanitizer and UndefinedBehaviorSanitizer" ON)
if(TEST_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

include(FetchContent)
FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(unity)

enable_testing()

set(APP ${CMAKE_CURRENT_SOURCE_DIR}/../App)

set(TEST_WARNINGS
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wstrict-prototypes
    -Wmissing-prototypes
)

# The fake board and the display reader, shared by every test.
add_library(test_support STATIC
    fakes/fake_bsp.c
    support/display_probe.c
)
target_include_directories(test_support PUBLIC
    fakes
    support
    ${APP}/Inc
    ${APP}/Drivers/Inc
    ${APP}/Bsp/Inc
)
target_compile_options(test_support PRIVATE ${TEST_WARNINGS})

# add_unit_test(<name> SOURCE <test file> UNDER_TEST <App sources...> [DEFINES <macros...>])
#
# Each test is its own executable, so module-level state (static variables)
# starts fresh for every test file.
function(add_unit_test name)
    cmake_parse_arguments(ARG "" "SOURCE" "UNDER_TEST;DEFINES" ${ARGN})

    add_executable(${name} ${ARG_SOURCE} ${ARG_UNDER_TEST})
    target_link_libraries(${name} PRIVATE test_support unity)
    target_compile_definitions(${name} PRIVATE ${ARG_DEFINES})
    target_compile_options(${name} PRIVATE ${TEST_WARNINGS})

    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_unit_test(test_event_queue SOURCE test_event_queue.c UNDER_TEST ${APP}/Src/event_queue.c)
add_unit_test(test_button      SOURCE test_button.c      UNDER_TEST ${APP}/Src/button.c)
add_unit_test(test_settings    SOURCE test_settings.c    UNDER_TEST ${APP}/Src/settings.c)
add_unit_test(test_seg7        SOURCE test_seg7.c        UNDER_TEST ${APP}/Drivers/Src/seg7.c)

# The state machine runs on the real display driver and settings module.
# Built twice, with the countdown unit of each firmware build type.
set(FSM_UNDER_TEST
    ${APP}/Src/fsm.c
    ${APP}/Src/settings.c
    ${APP}/Drivers/Src/seg7.c
)
add_unit_test(test_fsm_seconds SOURCE test_fsm.c UNDER_TEST ${FSM_UNDER_TEST} DEFINES COUNTDOWN_UNIT_SECONDS=1u)  # Debug
add_unit_test(test_fsm_minutes SOURCE test_fsm.c UNDER_TEST ${FSM_UNDER_TEST} DEFINES COUNTDOWN_UNIT_SECONDS=60u) # Release

add_unit_test(test_app
    SOURCE     test_app.c
    UNDER_TEST ${APP}/Src/app.c ${APP}/Src/button.c ${APP}/Src/event_queue.c ${FSM_UNDER_TEST}
    DEFINES    COUNTDOWN_UNIT_SECONDS=1u
)
