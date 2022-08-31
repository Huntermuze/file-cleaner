cmake_minimum_required(VERSION 3.23)
project(Operating_System_Principles)

set(CMAKE_CXX_STANDARD 20)

add_executable(Operating_System_Principles
        task_1/word_filter.cpp
        task_1/word_filter.h
        utilities/function_timer.h
        utilities/function_timer.cpp
        task_2/task2.cpp)
