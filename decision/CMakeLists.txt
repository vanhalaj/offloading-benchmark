add_library(decision STATIC
    decision.c
    decision.h
    greedy/greedy.h
    greedy/greedy.c
)

target_include_directories(decision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
