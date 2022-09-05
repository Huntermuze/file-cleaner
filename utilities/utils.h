#ifndef OPERATING_SYSTEM_PRINCIPLES_UTILS_H
#define OPERATING_SYSTEM_PRINCIPLES_UTILS_H

#include <iostream>
#include "../task_1/word_filter.h"

void unlink_fifos();

namespace WordFilter {
    bool compare_string(const std::string_view &s1, const std::string_view &s2);

    bool compare_vector_of_string(const std::vector<std::string> &s1, const std::vector<std::string> &s2);
}

#endif //OPERATING_SYSTEM_PRINCIPLES_UTILS_H
