#ifndef OPERATING_SYSTEM_PRINCIPLES_WORD_FILTER_H
#define OPERATING_SYSTEM_PRINCIPLES_WORD_FILTER_H
#define MAX_WORD_LENGTH 15
#define MIN_WORD_LENGTH 3
#define CLEAN_FILE_RULES "^[a-z]{3,15}$"

#include <vector>

namespace WordFilter {
    int task_filter(std::string *dirty_path, std::string *clean_path);

    std::vector<std::string> task_filter(std::string *dirty_path);

    std::vector<std::string> task_filter_dual(std::string *dirty_path, std::string *clean_path);
}


#endif //OPERATING_SYSTEM_PRINCIPLES_WORD_FILTER_H
