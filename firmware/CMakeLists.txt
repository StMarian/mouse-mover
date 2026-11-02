cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME mouse-mover)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# --- Firmware layers ------------------------------------------------------------
#
#   app  (App/Src, App/Drivers)  plain C: state machine, settings, button, display driver
#    |
#   bsp  (App/Bsp)               the only code that includes main.h / the HAL
#    |
#   CubeMX HAL + CMSIS
#
# app is built WITHOUT the CubeMX/HAL include paths, so a HAL include outside
# App/Bsp fails to compile. That keeps the layering honest and lets the app
# layer be built and unit-tested on a PC.

# Applied to everything under App/ (not to CubeMX or vendor code).
set(APP_WARNINGS
    -Wextra
    -Wpedantic
    -Wshadow
    -Wconversion
    -Wsign-conversion
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wcast-qual
    -Wundef
)

option(WARNINGS_AS_ERRORS "Treat warnings in App/ as errors" OFF)
if(WARNINGS_AS_ERRORS)
    list(APPEND APP_WARNINGS -Werror)
endif()

add_library(bsp STATIC
    App/Bsp/Src/bsp.c
    App/Bsp/Src/bsp_flash.c
)
target_include_directories(bsp PUBLIC App/Bsp/Inc)
target_link_libraries(bsp PRIVATE stm32cubemx)  # HAL headers stay private to bsp
target_compile_options(bsp PRIVATE ${APP_WARNINGS})

add_library(app STATIC
    App/Src/app.c
    App/Src/button.c
    App/Src/event_queue.c
    App/Src/fsm.c
    App/Src/settings.c
    App/Drivers/Src/seg7.c
)
target_include_directories(app PUBLIC App/Inc App/Drivers/Inc)
target_link_libraries(app PUBLIC bsp)
target_compile_options(app PRIVATE ${APP_WARNINGS})

# Debug builds count down in seconds instead of minutes, so a full cycle can be watched on the bench.
target_compile_definitions(app PRIVATE COUNTDOWN_UNIT_SECONDS=$<IF:$<CONFIG:Debug>,1u,60u>)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
    app
)
