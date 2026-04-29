add_executable(simulation 
    main.c 
    simulate.c 
    simulate.h
    scheduler.h
    scheduler.c
    testcase.h
    testcases/test_latency.h)

target_include_directories(simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(simulation
    PRIVATE
        decision
        task_examples
)
