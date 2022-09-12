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
    // Default to 15 seconds.
    int graceful_exit_threshold = GRACEFUL_EXIT_DEFAULT_THRESHOLD;
    if (argc == 4) {
        dirty_file_path = argv[1];
        clean_file_path = argv[2];
        graceful_exit_threshold = std::stoi(argv[3]);
    } else if (argc == 3) {
        dirty_file_path = argv[1];
        clean_file_path = argv[2];
    } else {
        fprintf(stderr,
                "You may only use the following syntax: \"./task1 [dirty_file_name] [clean_file_name] [graceful_exit_threshold_seconds]\"."
                "\nNote that the graceful exit threshold is OPTIONAL, and will default to %d seconds if the argument is ignored.\n",
                GRACEFUL_EXIT_DEFAULT_THRESHOLD);
        return EXIT_FAILURE;
    }

    if (graceful_exit(&graceful_exit_threshold) != 0) {
        return EXIT_FAILURE;
    }
    auto run = time_func(sort_and_write, &dirty_file_path, &clean_file_path);
    printf("Successfully cleansed dirty file. Check the current directory for the cleaned file located at %s.",
           clean_file_path.c_str());
    printf("Time taken in seconds: %fs.\n", run.first);

    return run.second;
}
