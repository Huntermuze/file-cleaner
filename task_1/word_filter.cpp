#include <fstream>
#include <random>
#include <regex>
#include "word_filter.h"

std::vector<std::string> WordFilter::TaskFilter(std::string *dirty_path, std::string *clean_path) {
    // Just sort the data and shuffle it. Ignore third
    std::string curr_line;
    std::ifstream DirtyFile(*dirty_path);
    std::vector<std::string> words_list = std::vector<std::string>();
    std::regex filter = std::regex(CLEAN_FILE_RULES);

    while (std::getline(DirtyFile, curr_line)) {
        if (std::regex_match(curr_line, filter)) {
            words_list.push_back(curr_line);
        }
    }

    DirtyFile.close();
    std::ofstream CleanFile(*clean_path);
    std::shuffle(words_list.begin(), words_list.end(), std::mt19937(std::random_device()()));
    words_list.erase(unique(words_list.begin(), words_list.end()), words_list.end());

    for (auto &i: words_list) {
        CleanFile << i << "\n";
    }

    CleanFile.close();
    return words_list;
}