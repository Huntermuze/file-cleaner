#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <wait.h>
#include <bits/stdc++.h>
#include "../task_1/word_filter.h"

std::vector<std::vector<std::string>> getLengthSepLists() {
    std::string dirty_file_path = "../data/dirty.txt";
    std::string clean_file_path = "../data/clean_cpp.txt";
    std::vector<std::vector<std::string>> s;
    std::string curr_line;
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

bool compareString(const std::string& s1, const std::string& s2){
    auto l = s1.substr(MIN_WORD_LENGTH, s1.length());
    auto r = s2.substr(MIN_WORD_LENGTH, s2.length());
    return l < r;
}

void map2() {
    // Check if this gets copied. If it does, fix it. Copying is expensive.
    std::vector<std::vector<std::string>> lists = getLengthSepLists();
    std::vector<std::string> words;
    // We do max - min - 1 bcs the curr process counts as one. So we only need 12 child processes.

    int pid;
    for (long unsigned int i = 0; i < lists.size() - 1; ++i) {
        // This causes copy. Use move semantics to avoid.
        pid = fork();
        words = lists[i];
        if (pid == 0) {
            break;
        }
        //        std::cout << "PID [" << (i + 1) << "]: " << pid << std::endl;
    }

    if (pid != 0) {
        words = lists[lists.size() - 1];
    }

    std::sort(words.begin(), words.end(), compareString);
    std::cout << words[0] << std::endl;
    /*
     * 'wait' waits for a child process to terminate, and returns that child process's pid. On error (eg when there are no
     * child processes), -1 is returned. So, basically, the code keeps waiting for child processes to finish, until the
     * wait-ing errors out, and then you know they are all finished.
     *
     * So, the childs with no children (leafs) will terminate immediately, since they don't have anything to wait for
     * and -1 will be returned as soon as this loop is checked.
     */
    while (wait(nullptr) != -1 || errno != ECHILD);
}

int main() {
    map2();
}

