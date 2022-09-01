#include <iostream>
#include "../utilities/function_timer.h"
#include "word_filter.h"

int main(int argc, char **argv) {
    // Skipping proper validation & error-checking, as this is not important in this assignment.
    std::string dirty_file_path;
    std::string clean_file_path;

    if (argc == 3) {
        dirty_file_path = argv[1];
        clean_file_path = argv[2];

    } else {
        std::cerr << "You may only use the following syntax: \"./t1 [dirty_file_name] [clean_file_name]\"."
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Cleaning file " << dirty_file_path << "..." << std::endl;
    double time_taken = time_func(WordFilter::TaskFilter, &dirty_file_path, &clean_file_path).first;
    std::cout << "Successfully cleansed dirty file. Check the current directory "
                 "for the cleaned file located at \"" << clean_file_path << "\"." << std::endl;

    std::cout << std::endl << "Time taken in seconds: " << time_taken << "s." << std::endl;
}
