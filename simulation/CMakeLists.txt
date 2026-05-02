add_executable(simulation 
    main.c 
    simulate.c 
    simulate.h
    scheduler.h
    scheduler.c
    testcase.h
    testcases/test_latency.h
    testcases/test_complexity.h
    testcases/test_latency_var.h
    testcases/test_cpu_freq_local.h
    testcases/test_bandwidth_up.h
    testcases/test_dependency.h
    testcases/test_complexity_size.h
)

target_include_directories(simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(simulation
    PRIVATE
        decision
        task_examples
)
