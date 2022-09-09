#include <iostream>
#include <fstream>
#include "word_filter.h"
#include "../utilities/utils.h"
#include "../utilities/function_timer.h"

int sort_and_write(std::string *dirty_file_path, std::string *clean_file_path) {
    printf("Obtaining clean words list.\n");
    auto *words_list = WordFilter::task_filter(dirty_file_path);
    std::ofstream CleanFile(*clean_file_path);
    // Remove duplicates.
    printf("Removing duplicates from clean words list.\n");
    words_list->erase(unique(words_list->begin(), words_list->end()), words_list->end());
    // Sort the data on 3rd char onwards.
    printf("Sorting the clean words list on the 3rd character onwards.\n");
    std::sort(words_list->begin(), words_list->end(), WordFilter::compare_string);

    if (!CleanFile.good()) {
        return EXIT_FAILURE;
    }
    printf("Writing the clean output to provided file destination.\n");
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
        fprintf(stderr, "You may only use the following syntax: \"./task1 [dirty_file_name] [clean_file_name]\".\n");
        return EXIT_FAILURE;
    }

    double time_taken = time_func(sort_and_write, &dirty_file_path, &clean_file_path).first;
    printf("Successfully cleansed dirty file. Check the current directory for the cleaned file located at %s.",
           clean_file_path.c_str());

    printf("Time taken in seconds: %fs.\n", time_taken);;
}
