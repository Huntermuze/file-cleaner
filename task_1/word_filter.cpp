#include <fstream>
#include <regex.h>
#include "word_filter.h"

std::vector<std::string> *WordFilter::task_filter(std::string *dirty_path) {
    // Just sort the data and shuffle it. Ignore third
    std::string curr_line;
    std::ifstream DirtyFile(*dirty_path);
    auto *words_list = new std::vector<std::string>();
    regex_t regx;
    regcomp(&regx, CLEAN_FILE_RULES, REG_EXTENDED);

    while (std::getline(DirtyFile, curr_line)) {
        if (!regexec(&regx, curr_line.c_str(), 0, nullptr, 0)) {
            words_list->push_back(curr_line);
        }
    }

    regfree(&regx);
    DirtyFile.close();

    return words_list;
}