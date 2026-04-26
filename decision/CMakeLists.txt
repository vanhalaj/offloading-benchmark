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
    optimal/optimal.c)

target_include_directories(decision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
