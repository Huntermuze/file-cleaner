#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <wait.h>
#include <bits/stdc++.h>
#include "../task_1/word_filter.h"
#include "../utilities/function_timer.h"

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

void map2() {
    // Check if this gets copied. If it does, fix it. Copying is expensive.
    std::vector<std::vector<std::string>> lists = getLengthSepLists();
    std::vector<std::string> words;
    // We do max - min - 1 bcs the curr process counts as one. So we only need 12 child processes.

    int pid;
    for (long unsigned int i = 0; i < lists.size() - 1; ++i) {
        // This causes copy. Use move semantics to avoid.
        pid = fork();
        std::cout << "1=" << pid << std::endl;
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
//    std::cout << words[0].data() << std::endl;

//    std::ofstream OutFile("separated_lists/length_" + std::to_string(words[0].length()));
//    for (auto &word: words) {
//        OutFile << *word.data() << "\n";
//    }
//
//    OutFile.close();

    /*
     * 'wait' waits for a child process to terminate, and returns that child process's pid. On error (eg when there are no
     * child processes), -1 is returned. So, basically, the code keeps waiting for child processes to finish, until the
     * wait-ing errors out, and then you know they are all finished.
     *
     * So, the childs with no children (leafs) will terminate immediately, since they don't have anything to wait for
     * and -1 will be returned as soon as this loop is checked.
     */
    while (wait(NULL) != -1 || errno != ECHILD) {
//        std::cout << "hi" << std::endl;
    };

    std::cout << "2=" << pid << std::endl;
}

int main() {
    // TODO stop this from printing 13 times. Find a way to suspend the child processes from continuing.
    map2();
//    double time_taken = time_void_func(map2);
//    std::cout << std::endl << "Time taken in seconds: " << time_taken << "s." << std::endl;
}


