#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <wait.h>
#include <bits/stdc++.h>
#include "../task_1/word_filter.h"
#include "../utilities/function_timer.h"

#define PROCESS_NUM 12

std::vector<std::vector<std::string>> getLengthSepLists() {
    std::string dirty_file_path = "../data/dirty.txt";
    std::string clean_file_path = "../data/clean_cpp.txt";
    std::vector<std::vector<std::string>> s;
    auto clean_words = WordFilter::TaskFilter(&dirty_file_path, &clean_file_path);

    for (long unsigned int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        std::vector<std::string> n_length_list;
        for (auto &word: clean_words) {
            if (word.length() == i) {
                n_length_list.push_back(word);
            }
        }
        s.push_back(n_length_list);
    }

    return s;
}

bool compareString(const std::string_view &s1, const std::string_view &s2) {
    return s1.substr(MIN_WORD_LENGTH - 1) < s2.substr(MIN_WORD_LENGTH - 1);
}

int map2() {
    // Check if this gets copied. If it does, fix it. Copying is expensive.
    std::vector<std::vector<std::string>> lists = getLengthSepLists();
    std::vector<std::string> words;
    int pid;
    // We do max - min - 1 bcs the curr process counts as one. So we only need 12 child processes.
    for (long unsigned int i = 0; i < lists.size() - 1; ++i) {
        // This causes copy. Use move semantics to avoid.
        pid = fork();
        if (pid == -1) {
            return EXIT_FAILURE;
        }
        words = lists[i];
        if (pid == 0) {
            break;
        }
    }

    if (pid != 0) {
        words = lists[lists.size() - 1];
    }

    std::sort(words.begin(), words.end(), compareString);
    std::ofstream OutFile("separated_lists/length_" + std::to_string(words[0].length()) + ".txt");
    for (auto &word: words) {
        OutFile << word << "\n";
    }
    OutFile.close();

    if (pid == 0) {
        exit(0);
    }
    while (wait(nullptr) != -1);

    return EXIT_SUCCESS;
}

int reduce() {
    // Read in all thirteen files, so that we can take from them when we need.
    std::vector<std::vector<std::string>> length_n_lists;
    int length_n_list_counters[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::ofstream SortedList("sorted_list.txt");
    std::string current_line;
    long unsigned int i;

    for (i = 0; i <= MAX_WORD_LENGTH - MIN_WORD_LENGTH; ++i) {
        length_n_lists.emplace_back();
    }

    for (i = 0; i <= MAX_WORD_LENGTH - MIN_WORD_LENGTH; ++i) {
        //FIXME change to "separated_lists/length_" once finished debugging.
        std::fstream LengthFile("../task_2/separated_lists/length_" + std::to_string(i + MIN_WORD_LENGTH) + ".txt");

        if (LengthFile.eof()) {
            return EXIT_FAILURE;
        }

        while (std::getline(LengthFile, current_line)) {
            length_n_lists[i].push_back(current_line);
        }

        LengthFile.close();
    }

    // TODO work out how to use string_view properly and use it!

    std::vector<std::string> comparison_container;
    for (i = 0; i < length_n_lists.size(); ++i) {
        comparison_container.push_back(length_n_lists[i][length_n_list_counters[i]]);
        length_n_list_counters[i] += 1;
    }
    int len_chosen_word;
    int desired_wordlist_pos;
    while (!comparison_container.empty()) {
        std::sort(comparison_container.begin(), comparison_container.end(), compareString);
        std::string to_add = comparison_container[0];
        SortedList << to_add << "\n";
        // Ignore narrowing conversion warning, as we know for a fact that this vector will be at most 13 elements in size.
        len_chosen_word = (int) to_add.length();
        desired_wordlist_pos = (int) len_chosen_word - MIN_WORD_LENGTH;

        // Remove first element.
        comparison_container.erase(comparison_container.begin());

        // Replace popped element with a new element from that same list.
        if (length_n_list_counters[desired_wordlist_pos] >= (int) length_n_lists[desired_wordlist_pos].size()) {
            continue;
        } else {
            comparison_container.push_back(
                    length_n_lists[desired_wordlist_pos][length_n_list_counters[desired_wordlist_pos]]);
            length_n_list_counters[desired_wordlist_pos] += 1;
        }
    }

    return EXIT_SUCCESS;
}

int main() {
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