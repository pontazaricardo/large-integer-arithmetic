cmake_minimum_required(VERSION  3.28)
project(largeIntegerArithmetic VERSION 0.1 LANGUAGES C ASM)

enable_testing()

set(CMAKE_C_STANDARD 11)

set(DEBUG_FLAGS -O0)
set(RELEASE_FLAGS -Wextra -Wpedantic)
set(COMMON_FLAGS -Wall -fPIC -march=native -mavx2)

#message("Apple: " ${APPLE})
message(STATUS "Detected architecture: " ${CMAKE_SYSTEM_PROCESSOR})

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    # ARM

elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    # Intel 64

    if(APPLE)
        # REQUIRED for current ASM prime field arithmetic
        set(CONFIG_OS_FLAG "-D__APPLE__")
    endif()

else()
    # Unsupported architecture
    message(FATAL_ERROR "Unsupported architecture" )
endif()

add_subdirectory(src)

