cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE DEBUG)

set(PICO_BOARD pico)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(main C CXX ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
# add_compile_definitions(SYS_CLK_MHZ=200)


add_executable(${PROJECT_NAME}
    main.c
    MLX90395/mlx90395.c
    I2C/i2c.c
)

include_directories(${PROJECT_NAME} PRIVATE
    .
    MLX90395
    I2C
)

# pico_generate_pio_header(${PROJECT_NAME}
# )

pico_add_extra_outputs(${PROJECT_NAME})


target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    # pico_cyw43_arch_none

    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(${PROJECT_NAME}  0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
