cmake_minimum_required(VERSION 3.8)
project(hoarding)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c userDefinedTypes.c userDefinedTypes.h)
add_executable(hoarding ${SOURCE_FILES})