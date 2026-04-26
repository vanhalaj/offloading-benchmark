add_library(decision STATIC
    decision.c
    decision.h
    greedy/greedy.h
    greedy/greedy.c
    task.h
    device.h
    lyapunov/lyapunov.h
    lyapunov/lyapunov.c
    rl/rl.h
    rl/rl.c
    optimal/optimal.h
    optimal/optimal.c 
    network.h)

target_include_directories(decision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenMP COMPONENTS C REQUIRED)
target_link_libraries(decision PUBLIC OpenMP::OpenMP_C)
