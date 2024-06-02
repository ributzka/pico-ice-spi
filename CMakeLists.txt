cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(external/pico-sdk/pico_sdk_init.cmake)

project(pico-ice-spi)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# configure the pico-ice-sdk
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/pico-ice-sdk)

# Pico firmware
add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    bitstream.inc
    )
target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_ice_sdk
    )
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 0)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)

# FPGA bitstream
set(VERILOG ${CMAKE_CURRENT_SOURCE_DIR}/top.v)

add_custom_command(
    OUTPUT bitstream.json
    DEPENDS ${VERILOG}
    COMMAND yosys -p 'synth_ice40 -abc9 -top top -json bitstream.json' ${VERILOG}
)

add_custom_command(
    OUTPUT bitstream.asc
    DEPENDS bitstream.json
    COMMAND nextpnr-ice40  --package sg48 --up5k --freq 48 --top top --pcf ${CMAKE_SOURCE_DIR}/pico_ice.pcf --json bitstream.json --asc bitstream.asc
)

add_custom_command(
    OUTPUT bitstream.bin
    DEPENDS bitstream.asc
    COMMAND icepack bitstream.asc bitstream.bin
)

add_custom_command(
    OUTPUT bitstream.inc
    DEPENDS bitstream.bin
    COMMAND python3 -c \"print(', '.join(map(hex, open('bitstream.bin', 'rb').read())))\" > bitstream.inc
    )