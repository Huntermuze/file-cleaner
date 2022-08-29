#ifndef OPERATING_SYSTEM_PRINCIPLES_TASK1_H
#define OPERATING_SYSTEM_PRINCIPLES_TASK1_H
#include <fstream>
#include <regex>

namespace WordFilter {
    void TaskFilter(const std::string &dirty_path, const std::string &clean_path, const std::regex &filter) {
        // Just sort the data and shuffle it. Ignore third
        std::string curr_line;
        std::ifstream DirtyFile(dirty_path);
        std::ofstream CleanFile(clean_path);

        while (std::getline(DirtyFile, curr_line)) {
            if (std::regex_match(curr_line, filter)) {
                CleanFile << curr_line << std::endl;
            }
        }

        DirtyFile.close();
        CleanFile.close();
    };
}


#endif //OPERATING_SYSTEM_PRINCIPLES_TASK1_H
