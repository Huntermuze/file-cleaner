#include <fstream>
#include <random>
#include <regex>
#include "word_filter.h"

std::vector<std::string> WordFilter::task_filter_dual(std::string *dirty_path, std::string *clean_path) {
    auto words_list = task_filter(dirty_path);
    std::ofstream CleanFile(*clean_path);
    for (auto &i: words_list) {
        CleanFile << i << "\n";
    }

    CleanFile.close();
    return words_list;
}

int WordFilter::task_filter(std::string *dirty_path, std::string *clean_path) {
    auto words_list = task_filter(dirty_path);
    std::ofstream CleanFile(*clean_path);
    
    if (!CleanFile.good()) {
        return EXIT_FAILURE;
    }
    for (auto &i: words_list) {
        CleanFile << i << "\n";
    }
    CleanFile.close();

    return EXIT_SUCCESS;
}

std::vector<std::string> WordFilter::task_filter(std::string *dirty_path) {
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
    // Shuffle the data.
    std::shuffle(words_list.begin(), words_list.end(), std::mt19937(std::random_device()()));
    // Remove duplicates.
    words_list.erase(unique(words_list.begin(), words_list.end()), words_list.end());

    return words_list;
}