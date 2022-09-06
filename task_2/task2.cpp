#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <wait.h>
#include "../utilities/utils.h"
#include "../utilities/function_timer.h"

std::vector<std::vector<std::string>> *get_length_lists() {
    std::string dirty_file_path = "../data/dirty.txt";
    auto *clean_words = WordFilter::task_filter(&dirty_file_path);
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
    // Check if this gets copied. If it does, fix it. Copying is expensive.
    auto *lists = get_length_lists();
    std::vector<std::string> words;
    int pid;
    // We do max - min - 1 bcs the curr process counts as one. So we only need 12 child processes.
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
    }

    if (pid != 0) {
        words = (*lists)[lists->size() - 1];
    }

    std::sort(words.begin(), words.end(), WordFilter::compare_string);
    std::ofstream OutFile("separated_lists/length_" + std::to_string(words[0].length()) + ".txt");
    for (auto &word: words) {
        OutFile << word << "\n";
    }
    OutFile.close();

    if (pid == 0) {
        exit(0);
    }
    while (wait(nullptr) != -1);
    delete lists;

    return EXIT_SUCCESS;
}

// TODO work out how to use string_view properly and use it!
int reduce() {
    // Read in all thirteen files, so that we can take from them when we need.
    std::vector<std::vector<std::string>> length_n_lists(13);
    std::string current_line;
    long unsigned int i;

    for (i = 0; i <= MAX_WORD_LENGTH - MIN_WORD_LENGTH; ++i) {
        std::string path = "separated_lists/length_" + std::to_string(i + MIN_WORD_LENGTH) + ".txt";
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
    WordFilter::merge_and_write(&length_n_lists, output_path);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    auto map2_run = time_func(map2);
    double time_map2 = map2_run.first;
    int status_map2 = map2_run.second;

    auto reduce_run = time_func(reduce);
    double time_reduce = reduce_run.first;
    int status_reduce = reduce_run.second;


    if (status_map2 != 0) {
        std::cerr << "An error occurred while attempting to create a fork." << std::endl;
    }
    if (status_reduce != 0) {
        std::cerr
                << "One or more of the files that were read in is empty! All files must have at least one line of content."
                << std::endl;
    }
    if (status_map2 == 0 && status_reduce == 0) {
        std::cout << std::endl << "Time taken in seconds: " << time_map2 + time_reduce << "s." << std::endl;
    }
}