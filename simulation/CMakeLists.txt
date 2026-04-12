add_executable(simulation 
    main.c 
    simulate.c 
    simulate.h
    scheduler.h
    scheduler.c
)

target_link_libraries(simulation
    PRIVATE
        decision
        task_examples
)
