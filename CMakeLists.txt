# CMakeLists.txt
# For Pico-Chess-Queens
#
#
cmake_minimum_required(VERSION 3.13)
#
#
include(pico_sdk_import.cmake)
#
#
project(Pico-Chess-Queens LANGUAGES C CXX ASM)
#
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico)
#
#
pico_sdk_init()
#
#
add_executable(Pico-Chess-Queens
	Pico-Chess-Queens.c
	)
#
#
# Send Pico's output to USB and UART (for debug purposes).
pico_enable_stdio_uart(Pico-Chess-Queens 1)
pico_enable_stdio_usb(Pico-Chess-Queens  1)
#
#
# Pull in our pico_stdlib which pulls in commonly used features.
target_link_libraries(Pico-Chess-Queens pico_stdlib)
#
#
pico_add_extra_outputs(Pico-Chess-Queens)

