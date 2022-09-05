#include <iostream>
#include <fstream>
#include "word_filter.h"
#include "../utilities/utils.h"
#include "../utilities/function_timer.h"

int sort_and_write(std::string *dirty_file_path, std::string *clean_file_path) {
    auto *words_list = WordFilter::task_filter(dirty_file_path);
    std::ofstream CleanFile(*clean_file_path);
    // Remove duplicates.
    words_list->erase(unique(words_list->begin(), words_list->end()), words_list->end());
    // Sort the data on 3rd char onwards.
    std::sort(words_list->begin(), words_list->end(), WordFilter::compare_string);

    if (!CleanFile.good()) {
        return EXIT_FAILURE;
    }
    for (auto &i: *words_list) {
        CleanFile << i << "\n";
    }
    CleanFile.close();

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // Skipping proper validation & error-checking, as this is not important in this assignment.
    std::string dirty_file_path;
    std::string clean_file_path;

    if (argc == 3) {
        dirty_file_path = argv[1];
        clean_file_path = argv[2];

    } else {
        std::cerr << "You may only use the following syntax: \"./task1 [dirty_file_name] [clean_file_name]\"."
                  << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Cleaning file \"" << dirty_file_path << "\"..." << std::endl;
    double time_taken = time_func(sort_and_write, &dirty_file_path, &clean_file_path).first;
    std::cout << "Successfully cleansed dirty file. Check the current directory "
                 "for the cleaned file located at \"" << clean_file_path << "\"." << std::endl;

    std::cout << std::endl << "Time taken in seconds: " << time_taken << "s." << std::endl;
}
