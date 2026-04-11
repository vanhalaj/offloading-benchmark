add_executable(profiler main.c)

target_link_libraries(profiler
    PRIVATE
        task_examples
)
