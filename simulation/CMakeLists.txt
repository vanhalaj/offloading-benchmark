add_executable(simulation main.c)

target_link_libraries(simulation
    PRIVATE
        decision
        task_examples
)
