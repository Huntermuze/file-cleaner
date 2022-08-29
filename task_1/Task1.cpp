#include <iostream>
#include <chrono>
#include "Task1.h"

long time_func(const std::string &dirty_file_path, const std::string &clean_file_path, const std::regex& filter);

int main(int argc, char **argv) {
    // Skipping proper validation & error-checking, as this is not important in this assignment.
    std::string dirtyFilePath;
    std::string clean_file_path;

    if (argc == 3) {
        dirtyFilePath = argv[1];
        clean_file_path = argv[2];

    } else {
        std::cout << "You may only use the following syntax: \"./t1 [dirty_file_name] [clean_file_name]\"."
                  << std::endl;
    }

    std::cout << "Cleaning file " << dirtyFilePath << "..." << std::endl;
    long time_taken = time_func(dirtyFilePath, clean_file_path, std::regex("^[a-z]{3,15}$"));
    std::cout << "Successfully cleansed dirty file. Check the current directory "
                 "for the cleaned file located at \"" << clean_file_path << "\"." << std::endl;

    std::cout << std::endl << "Time taken in seconds: " << time_taken << "s." << std::endl;
}

long time_func(const std::string &dirty_file_path, const std::string &clean_file_path, const std::regex& filter) {
    auto start = std::chrono::high_resolution_clock::now();
    WordFilter::TaskFilter(dirty_file_path, clean_file_path, filter);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

    return duration.count();
}