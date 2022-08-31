#ifndef OPERATING_SYSTEM_PRINCIPLES_TIMER_H
#define OPERATING_SYSTEM_PRINCIPLES_TIMER_H

#include <chrono>
#include <iostream>
#include <functional>

template<typename T>
double duration_in_seconds(T seconds) {
    return std::chrono::duration<double, std::ratio<1>>(seconds).count();
}

template<typename T, typename N, typename ...M>
std::pair<double, T> time_func(T(*func)(N, M...), N n, M...m) {
    auto start = std::chrono::high_resolution_clock::now();
    T result = func(n, m...);
    auto end = std::chrono::high_resolution_clock::now();
    return {duration_in_seconds(end - start), result};
}

template<typename T, typename M, typename N>
std::pair<double, T> time_func(T(*func)(M, N), M m, N n) {
    auto start = std::chrono::high_resolution_clock::now();
    T result = func(m, n);
    auto end = std::chrono::high_resolution_clock::now();
    return {duration_in_seconds(end - start), result};
}

template<typename T, typename ...M>
std::pair<double, T> time_func(T(*func)(M...), M...m) {
    auto start = std::chrono::high_resolution_clock::now();
    T result = func(m...);
    auto end = std::chrono::high_resolution_clock::now();
    return {duration_in_seconds(end - start), result};
}

template<typename T>
std::pair<double, T> time_func(T(*func)()) {
    auto start = std::chrono::high_resolution_clock::now();
    T result = func();
    auto end = std::chrono::high_resolution_clock::now();
    return {duration_in_seconds(end - start), result};
}


#endif //OPERATING_SYSTEM_PRINCIPLES_TIMER_H
