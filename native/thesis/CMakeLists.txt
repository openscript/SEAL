# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

cmake_minimum_required(VERSION 3.12)

project(SEALThesis VERSION 3.4.5 LANGUAGES CXX)

# Executable will be in ../bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SEALThesis_SOURCE_DIR}/../bin)

add_executable(1_noise_growth_addition 1_noise_growth_addition.cpp)
add_executable(2_noise_growth_subtraction 2_noise_growth_subtraction.cpp)
add_executable(3_noise_growth_multiplication 3_noise_growth_multiplication.cpp)
add_executable(4_noise_growth_division 4_noise_growth_division.cpp)

# Import Microsoft SEAL
find_package(SEAL 3.4.5 EXACT REQUIRED
    # Providing a path so this can be built without installing Microsoft SEAL
    PATHS ${SEALThesis_SOURCE_DIR}/../src/cmake
)

# Link Microsoft SEAL
target_link_libraries(1_noise_growth_addition SEAL::seal)
target_link_libraries(2_noise_growth_subtraction SEAL::seal)
target_link_libraries(3_noise_growth_multiplication SEAL::seal)
target_link_libraries(4_noise_growth_division SEAL::seal)
