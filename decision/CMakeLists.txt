add_library(decision STATIC
    decision.c
    decision.h
)

target_include_directories(decision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
