#include <unistd.h>
#include <algorithm>
#include <fstream>
#include "utils.h"
#include "../task_1/word_filter.h"

void *countdown(void *arg) {
    auto *time_in_seconds_to_terminate = static_cast<int *>(arg);
    if (time_in_seconds_to_terminate != nullptr) {
        sleep(*time_in_seconds_to_terminate);
    }
    fprintf(stderr, "The program has gracefully terminated after exceeding its allowed (%ds) running duration!\n",
            *time_in_seconds_to_terminate);
    exit(EXIT_SUCCESS);
}

int graceful_exit(int *time_in_seconds_to_terminate) {
    pthread_t countdown_thread;
    if (pthread_create(&countdown_thread, nullptr, &countdown, time_in_seconds_to_terminate) != 0) {
        fprintf(stderr, "Failed to create graceful exit thread.\n");
        return EXIT_FAILURE;
    }
    // Do not wait until countdown finishes. This thread (the main one) can terminate before it does, and thus the
    // process ends. This is how we will implement the graceful exiting system.

    return EXIT_SUCCESS;
}

bool WordFilter::compare_string(const std::string_view &s1, const std::string_view &s2) {
    return s1.substr(MIN_WORD_LENGTH - 1) < s2.substr(MIN_WORD_LENGTH - 1);
}

bool WordFilter::compare_vector_of_string(const std::vector<std::string> &s1, const std::vector<std::string> &s2) {
    return s1[0].length() < s2[0].length();
}

void WordFilter::merge_and_write(std::vector<std::vector<std::string>> *length_n_fifos, std::string &sorted_file_path) {
    int len_chosen_word;
    int desired_wordlist_pos;
    int length_n_list_counters[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::ofstream SortedList(sorted_file_path);
    std::vector<std::string> comparison_container;

    for (long unsigned int i = 0; i < length_n_fifos->size(); ++i) {
        comparison_container.push_back((*length_n_fifos)[i][length_n_list_counters[i]]);
        length_n_list_counters[i] += 1;
    }

    while (!comparison_container.empty()) {
        std::sort(comparison_container.begin(), comparison_container.end(), WordFilter::compare_string);
        std::string to_add = comparison_container[0];
        SortedList << to_add << "\n";
        // Ignore narrowing conversion warning, as we know for a fact that this vector will be at most 13 elements in size.
        len_chosen_word = (int) to_add.length();
        desired_wordlist_pos = (int) len_chosen_word - MIN_WORD_LENGTH;

        // Remove first element.
        comparison_container.erase(comparison_container.begin());

        // Replace popped element with a new element from that same list.
        if (length_n_list_counters[desired_wordlist_pos] >= (int) (*length_n_fifos)[desired_wordlist_pos].size()) {
            continue;
        } else {
            comparison_container.push_back(
                    (*length_n_fifos)[desired_wordlist_pos][length_n_list_counters[desired_wordlist_pos]]);
            length_n_list_counters[desired_wordlist_pos] += 1;
        }
    }
}

void unlink_fifos() {
    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        std::string fifo_path = std::string("fifo_files/length_" + std::to_string(i) + ".txt");
        unlink(fifo_path.c_str());
    }
}