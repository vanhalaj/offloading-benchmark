add_library(task STATIC
    task.c
    task.h
)

target_include_directories(task PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
