cmake_minimum_required(VERSION 3.13)

project(offloading C)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_compile_definitions(PLATFORM_RPI=1)
else()
    add_compile_definitions(PLATFORM_DESKTOP=1)
endif()

if(PLATFORM_RPI)
    add_compile_options(-march=armv6 -mfpu=vfp -mfloat-abi=hard)
endif()

add_subdirectory(decision)
add_subdirectory(task_examples)

add_subdirectory(simulation)
add_subdirectory(client)
add_subdirectory(server)
