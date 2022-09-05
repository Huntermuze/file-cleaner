#include <unistd.h>
#include "utils.h"

bool WordFilter::compare_string(const std::string_view &s1, const std::string_view &s2) {
    return s1.substr(MIN_WORD_LENGTH - 1) < s2.substr(MIN_WORD_LENGTH - 1);
}

bool WordFilter::compare_vector_of_string(const std::vector<std::string> &s1, const std::vector<std::string> &s2) {
    return s1[0].length() < s2[0].length();
}

void unlink_fifos() {
    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        std::string fifo_path = std::string("fifo_files/length_" + std::to_string(i) + ".txt");
        unlink(fifo_path.c_str());
    }
}