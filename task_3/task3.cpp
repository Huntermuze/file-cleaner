#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include "../utilities/function_timer.h"
#include "../utilities/utils.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <pthread.h>

std::string dirty_file_path = "../data/dirty.txt";
std::vector<std::string> *clean_words;

struct WriteFunctionArgs {
    std::string fifo_path;
    std::vector<long unsigned int> index3;
};

void *read_from_fifo(void *fifo_path) {
    auto *path = static_cast<std::string *>(fifo_path);
    int fd = open(path->c_str(), O_RDONLY);
    auto *curr_word = new std::string();
    auto *length_n_fifo = new std::vector<std::string>();

    while (read(fd, curr_word, sizeof(*curr_word)) != 0) {
        length_n_fifo->push_back(std::string(*curr_word));
    }

    close(fd);
    delete path;
    return length_n_fifo;
}

void *write_to_fifo(void *args) {
    auto *arguments = (struct WriteFunctionArgs *) args;
    int fd = open(arguments->fifo_path.c_str(), O_WRONLY);
    auto sort_rule = [](const int &i1, const int &i2) -> bool {
        return (*clean_words)[i1].substr(MIN_WORD_LENGTH - 1) < (*clean_words)[i2].substr(MIN_WORD_LENGTH - 1);
    };

    std::sort(arguments->index3.begin(), arguments->index3.end(), sort_rule);

    for (unsigned long i: arguments->index3) {
        write(fd, &(*clean_words)[i], sizeof((*clean_words)[i]));
    }
    close(fd);
    delete arguments;

    return EXIT_SUCCESS;
}

void *map3(void *) {
    // Fill the index_lists vector.
    std::vector<std::vector<long unsigned int>> index_lists(13);

    for (long unsigned int j = 0; j < clean_words->size(); ++j) {
        auto word_length = (*clean_words)[j].length();
        auto length_to_position = word_length - MIN_WORD_LENGTH;
        index_lists[length_to_position].push_back(j);
    }

    // Create 13 FIFO files.
    for (short k = MIN_WORD_LENGTH; k <= MAX_WORD_LENGTH; ++k) {
        if (mkfifo(("fifo_files/length_" + std::to_string(k) + ".txt").c_str(), 0777) == -1 &&
            errno != EEXIST) {
            std::cerr << "Failed to create a FIFO file." << std::endl;
            return (void *) 5;
        }
    }

    pthread_t workers[13];
    for (short m = MIN_WORD_LENGTH; m <= MAX_WORD_LENGTH; ++m) {
        auto *args = new WriteFunctionArgs{
                .fifo_path = "fifo_files/length_" + std::to_string(m) + ".txt",
                .index3 = index_lists[m - MIN_WORD_LENGTH]
        };

        if (pthread_create(&workers[m - MIN_WORD_LENGTH], nullptr, &write_to_fifo, args) != 0) {
            std::cerr << "Failed to create worker thread." << std::endl;
            return (void *) 6;
        }
    }

    for (unsigned long worker: workers) {
        if (pthread_join(worker, nullptr) != 0) {
            std::cerr << "Failed to create join thread." << std::endl;
            return (void *) 7;
        }
    }

    return EXIT_SUCCESS;
}

void merge_and_write(std::vector<std::vector<std::string>> *length_n_fifos) {
    int len_chosen_word;
    int desired_wordlist_pos;
    int length_n_list_counters[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::ofstream SortedList("../data/task3_sorted_list.txt");
    std::vector<std::string> comparison_container;

    for (long unsigned int i = 0; i < length_n_fifos->size(); ++i) {
        comparison_container.push_back((*length_n_fifos)[i][length_n_list_counters[i]]);
        length_n_list_counters[i] += 1;
    }

    while (!comparison_container.empty()) {
        std::sort(comparison_container.begin(), comparison_container.end(), WordFilter::compare_string);
        std::string to_add = comparison_container[0];
        SortedList << to_add << "\n";
        // Ignore narrowing conversion warning, as we know for a fact that this vector will be at most 13 elements in size.
        len_chosen_word = (int) to_add.length();
        desired_wordlist_pos = (int) len_chosen_word - MIN_WORD_LENGTH;

        // Remove first element.
        comparison_container.erase(comparison_container.begin());

        // Replace popped element with a new element from that same list.
        if (length_n_list_counters[desired_wordlist_pos] >= (int) (*length_n_fifos)[desired_wordlist_pos].size()) {
            continue;
        } else {
            comparison_container.push_back(
                    (*length_n_fifos)[desired_wordlist_pos][length_n_list_counters[desired_wordlist_pos]]);
            length_n_list_counters[desired_wordlist_pos] += 1;
        }
    }
}

void *reduce3(void *) {
    auto *length_n_fifos = new std::vector<std::vector<std::string>>();
    pthread_t workers[13];

    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        auto *fifo_path = new std::string("fifo_files/length_" + std::to_string(i) + ".txt");
        if (pthread_create(&workers[i - MIN_WORD_LENGTH], nullptr, &read_from_fifo, fifo_path) != 0) {
            std::cerr << "Failed to create worker thread." << std::endl;
            return (void *) 8;
        }
    }

    // FIXME might be the problem where all the threads fill up length_n_fifos in a random order.
    //  means we have extra overhead of having to search for items in that list.
    for (unsigned long worker: workers) {
        std::vector<std::string> *length_n_fifo;
        if (pthread_join(worker, (void **) &length_n_fifo) != 0) {
            std::cerr << "Failed to create join thread." << std::endl;
            return (void *) 9;
        }
        length_n_fifos->push_back(*length_n_fifo);

    }
    std::sort(length_n_fifos->begin(), length_n_fifos->end(), WordFilter::compare_vector_of_string);
    merge_and_write(length_n_fifos);

//    for (int i = 0; i < length_n_fifos->size(); ++i) {
//        delete &length_n_fifos[i];
//    }
//    delete length_n_fifos;
    return EXIT_SUCCESS;
}

int generate_sorted_list() {
    clean_words = WordFilter::task_filter(&dirty_file_path);
    pthread_t map_thread;
    pthread_t reduce_thread;

    if (pthread_create(&map_thread, nullptr, &map3, nullptr) != 0) {
        std::cerr << "Failed to create map thread." << std::endl;
        return EXIT_FAILURE;
    }
    if (pthread_create(&reduce_thread, nullptr, &reduce3, nullptr) != 0) {
        std::cerr << "Failed to create reduce thread." << std::endl;
        return 2;
    }
    if (pthread_join(map_thread, nullptr) != 0) {
        std::cerr << "Failed to join map thread." << std::endl;
        return 3;
    }
    if (pthread_join(reduce_thread, nullptr) != 0) {
        std::cerr << "Failed to join reduce thread." << std::endl;
        return 4;
    }

    delete clean_words;
    return EXIT_SUCCESS;
}

int main() {
    auto task3_run = time_func(generate_sorted_list);
    if (task3_run.second == 0) {
        std::cout << std::endl << "Time taken in seconds: " << task3_run.first << "s." << std::endl;
    }

    return task3_run.second;
}