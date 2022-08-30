#include "Task2.h"

separated_lists getLengthSepLists() {
    separated_lists s;
    std::string curr_line;
    std::ifstream CleanFile("../task_1/clean_cpp.txt");

    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        std::vector<std::string> n_length_list;

        while (std::getline(CleanFile, curr_line)) {
            if (curr_line.length() == i) {
                n_length_list.push_back(curr_line);
            }
        }
        s.push_back(n_length_list);
    }
    CleanFile.close();

    return s;
}

void map2() {
    int i;
    // Check if this gets copied. If it does, fix it. Copying is expensive.
    separated_lists lists = getLengthSepLists();
    std::vector<std::string> words;
    // We do max - min - 1 bcs the curr process counts as one. So we only need 12 child processes.
    for (i = 0; i < MAX_WORD_LENGTH - MIN_WORD_LENGTH - 1; ++i) {
        int pid = fork();
        words = lists[i];
        if (pid == 0) {
            break;
        }
//        std::cout << "PID [" << (i + 1) << "]: " << pid << std::endl;
        // This causes copy. Use move semantics to avoid.
    }

    std::cout << words[0] << std::endl;
    // TODO NEED TO FIX THIS.
}

int main() {
    map2();
}

