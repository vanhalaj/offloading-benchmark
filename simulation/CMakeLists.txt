add_executable(simulation 
    main.c 
    simulate.c 
    simulate.h
)

target_link_libraries(simulation
    PRIVATE
        decision
        task_examples
)
