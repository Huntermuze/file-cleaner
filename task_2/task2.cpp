#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <wait.h>
#include "../task_1/word_filter.h"
#include "../utilities/utils.h"
#include "../utilities/function_timer.h"

std::string dirty_file_path = "../data/dirty.txt";
std::vector<std::string> *clean_words;

std::vector<std::vector<std::string>> *get_length_lists() {
    auto *s = new std::vector<std::vector<std::string>>();
    for (long unsigned int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        std::vector<std::string> n_length_list;
        for (auto &word: *clean_words) {
            if (word.length() == i) {
                n_length_list.push_back(word);
            }
        }
        s->push_back(n_length_list);
    }

    delete clean_words;

    return s;
}

int map2() {
    printf("Obtaining clean words list.\n");
    clean_words = WordFilter::task_filter(&dirty_file_path);
    auto *lists = get_length_lists();
    std::vector<std::string> words;
    int pid = 0;

    // We do max - min - 1 bcs the curr process counts as one. So we only need 12 child processes.
    printf("Creating child processes...\n");
    for (long unsigned int i = 0; i < lists->size() - 1; ++i) {
        // This causes copy. Use move semantics to avoid.
        pid = fork();
        if (pid == -1) {
            return EXIT_FAILURE;
        }
        words = (*lists)[i];
        if (pid == 0) {
            break;
        }
        printf("Created child process with PID=%d\n", pid);
    }

    if (pid != 0) {
        words = (*lists)[lists->size() - 1];
    }

    printf("Sorting (and writing to file) words of length %lu. There are %zu words in this list.\n", words[0].length(),
           words.size());
    std::sort(words.begin(), words.end(), WordFilter::compare_string);
    std::ofstream OutFile("separated_lists/length_" + std::to_string(words[0].length()) + ".txt");
    for (auto &word: words) {
        OutFile << word << "\n";
    }
    OutFile.close();

    printf("Destroying a child process.\n");
    if (pid == 0) {
        exit(EXIT_SUCCESS);
    }
    while (wait(nullptr) != -1);
    delete lists;

    return EXIT_SUCCESS;
}

int reduce2() {
    // Read in all thirteen files, so that we can take from them when we need.
    std::vector<std::vector<std::string>> length_n_lists(13);
    std::string current_line;
    long unsigned int i;

    for (i = 0; i <= MAX_WORD_LENGTH - MIN_WORD_LENGTH; ++i) {
        std::string path = "separated_lists/length_" + std::to_string(i + MIN_WORD_LENGTH) + ".txt";
        printf("Reading in file %s. Then creating a vector of these length n words.\n", path.c_str());
        std::fstream LengthFile(path);

        if (LengthFile.eof()) {
            return EXIT_FAILURE;
        }

        while (std::getline(LengthFile, current_line)) {
            length_n_lists[i].push_back(current_line);
        }

        LengthFile.close();
        unlink(path.c_str());
    }

    std::string output_path = "../data/task2_sorted_list.txt";
    printf("Perform merge algorithm, and then write final result to file %s\n", output_path.c_str());
    WordFilter::merge_and_write(&length_n_lists, output_path);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // Default to 15 seconds.
    int graceful_exit_threshold = GRACEFUL_EXIT_DEFAULT_THRESHOLD;
    if (argc == 2) {
        graceful_exit_threshold = std::stoi(argv[1]);
    }

    if (graceful_exit(&graceful_exit_threshold) != 0) {
        return EXIT_FAILURE;
    }

    printf("Begin mapping process...\n");
    auto map2_run = time_func(map2);
    double time_map2 = map2_run.first;
    int status_map2 = map2_run.second;

    printf("Begin reduce process...\n");
    auto reduce2_run = time_func(reduce2);
    double time_reduce2 = reduce2_run.first;
    int status_reduce2 = reduce2_run.second;

    if (status_map2 != EXIT_SUCCESS) {
        fprintf(stderr, "An error occurred while attempting to create a fork.\n");
        return status_map2;
    }
    if (status_reduce2 != EXIT_SUCCESS) {
        fprintf(stderr,
                "One or more of the files that were read in is empty! All files must have at least one line of content.\n");
        return status_reduce2;
    }
    printf("Time taken in seconds: %fs.\n", time_map2 + time_reduce2);

    return EXIT_SUCCESS;
}