cmake_minimum_required(VERSION 3.31)
project(RTk_GPIO)

set(CMAKE_CXX_STANDARD 20)

add_executable(RTk_GPIO
        main.cpp
        RTkGPIO.cpp
        RTkGPIO.h)
