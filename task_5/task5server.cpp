#include <iostream>
#include <cstdlib>
#include "../task_1/word_filter.h"

void write_to_stdout(const std::vector<std::string> *clean_words) {
    for (unsigned long i = 0; i < clean_words->size(); ++i) {
        printf("%s\n", (*clean_words)[i].c_str());
    }
}

int run_server() {
    std::string dirty_file_path = "../data/dirty.txt";
    auto *clean_words = WordFilter::task_filter(&dirty_file_path);
    write_to_stdout(clean_words);

    delete clean_words;

    return EXIT_SUCCESS;
}

int main() {
    run_server();

    return EXIT_SUCCESS;
}