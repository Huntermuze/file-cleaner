#ifndef OPERATING_SYSTEM_PRINCIPLES_TASK1_H
#define OPERATING_SYSTEM_PRINCIPLES_TASK1_H
#define MAX_WORD_LENGTH 15
#define MIN_WORD_LENGTH 3

#include <fstream>
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <regex>

namespace WordFilter {
    void TaskFilter(const std::string &dirty_path, const std::string &clean_path, const std::regex &filter) {
        // Just sort the data and shuffle it. Ignore third
        std::string curr_line;
        std::ifstream DirtyFile(dirty_path);
        std::vector<std::string> words_list = std::vector<std::string>();

        while (std::getline(DirtyFile, curr_line)) {
            if (std::regex_match(curr_line, filter)) {
                words_list.push_back(curr_line);
            }
        }

        DirtyFile.close();
        std::ofstream CleanFile(clean_path);
        std::shuffle(words_list.begin(), words_list.end(), std::mt19937(std::random_device()()));
        words_list.erase( unique( words_list.begin(), words_list.end() ), words_list.end() );

        for (auto & i : words_list) {
            CleanFile << i << "\n";
        }

        CleanFile.close();
    }
}


#endif //OPERATING_SYSTEM_PRINCIPLES_TASK1_H
