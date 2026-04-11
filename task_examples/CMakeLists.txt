add_library(task_examples STATIC
    task_examples.c
    task_examples.h
)

target_include_directories(task_examples PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
