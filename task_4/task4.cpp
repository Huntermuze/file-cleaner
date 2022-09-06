#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cmath>
#include "../utilities/function_timer.h"
#include "../utilities/utils.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <pthread.h>

std::string dirty_file_path = "../data/dirty.txt";
std::vector<std::string> *clean_words;
short *th_priorities;

struct WriteFunctionArgs {
    int thread_priority;
    std::string fifo_path;
    std::vector<long unsigned int> index4;
};

struct ReadFunctionArgs {
    int thread_priority;
    std::string fifo_path;
};

void *read_from_fifo(void *args) {
    auto *arguments = (struct ReadFunctionArgs *) args;
    nice(arguments->thread_priority);
    int fd = open(arguments->fifo_path.c_str(), O_RDONLY);
    auto *curr_word = new std::string();
    auto *length_n_fifo = new std::vector<std::string>();

    while (read(fd, curr_word, sizeof(*curr_word)) != 0) {
        length_n_fifo->push_back(std::string(*curr_word));
    }
    close(fd);
    delete arguments;

    return length_n_fifo;
}

void *write_to_fifo(void *args) {
    auto *arguments = (struct WriteFunctionArgs *) args;
    nice(arguments->thread_priority);
    int fd = open(arguments->fifo_path.c_str(), O_WRONLY);
    auto sort_rule = [](const int &i1, const int &i2) -> bool {
        return (*clean_words)[i1].substr(MIN_WORD_LENGTH - 1) < (*clean_words)[i2].substr(MIN_WORD_LENGTH - 1);
    };

    std::sort(arguments->index4.begin(), arguments->index4.end(), sort_rule);

    for (unsigned long i: arguments->index4) {
        write(fd, &(*clean_words)[i], sizeof((*clean_words)[i]));
    }
    close(fd);
    delete arguments;

    return EXIT_SUCCESS;
}

short *calculate_thread_priorities(std::vector<std::vector<long unsigned int>> *index_lists) {
    std::vector<std::pair<int, int>> index_to_size_list(13);
    auto *th_priority_of_each_index_list = new short[13];
    auto sort_rule = [](const std::pair<int, int> &p1, const std::pair<int, int> &p2) -> bool {
        return p1.second < p2.second;
    };

    for (int i = 0; i < index_lists->size(); ++i) {
        index_to_size_list[i] = {i, (*index_lists)[i].size()};
    }

    std::sort(index_to_size_list.begin(), index_to_size_list.end(), sort_rule);

    for (int j = 0; j < 13; ++j) {
        auto index = index_to_size_list[j].first;
        th_priority_of_each_index_list[index] = (short) j;
    }

    return th_priority_of_each_index_list;
}

std::vector<std::vector<long unsigned int>> *create_indexes_list() {
    auto *index_lists = new std::vector<std::vector<long unsigned int>>(13);

    // Fill the index_lists vector.
    for (long unsigned int j = 0; j < clean_words->size(); ++j) {
        auto word_length = (*clean_words)[j].length();
        auto length_to_position = word_length - MIN_WORD_LENGTH;
        (*index_lists)[length_to_position].push_back(j);
    }

    return index_lists;
}

void *map4(void *arg) {
    auto *index_lists = static_cast<std::vector<std::vector<long unsigned int>> *>(arg);

    // Create 13 FIFO files IFF they do not exist.
    for (short k = MIN_WORD_LENGTH; k <= MAX_WORD_LENGTH; ++k) {
        if (mkfifo(("fifo_files/length_" + std::to_string(k) + ".txt").c_str(), 0777) == -1 &&
            errno != EEXIST) {
            std::cerr << "Failed to create a FIFO file." << std::endl;
            return (void *) 5;
        }
    }

    pthread_t workers[13];
    for (short m = MIN_WORD_LENGTH; m <= MAX_WORD_LENGTH; ++m) {
        // FIXME stop this struct from constantly copying - makes it much slower.
        auto *args = new WriteFunctionArgs{
                .thread_priority = th_priorities[m - MIN_WORD_LENGTH],
                .fifo_path = "fifo_files/length_" + std::to_string(m) + ".txt",
                .index4 = (*index_lists)[m - MIN_WORD_LENGTH]
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

void *reduce4(void *) {
    auto *length_n_fifos = new std::vector<std::vector<std::string>>();
    std::string output_path = "../data/task4_sorted_list.txt";
    pthread_t workers[13];

    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        auto *args = new ReadFunctionArgs{
                .thread_priority = th_priorities[i - MIN_WORD_LENGTH],
                .fifo_path = "fifo_files/length_" + std::to_string(i) + ".txt",
        };
        if (pthread_create(&workers[i - MIN_WORD_LENGTH], nullptr, &read_from_fifo, args) != 0) {
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
    WordFilter::merge_and_write(length_n_fifos, output_path);
//    for (int i = 0; i < length_n_fifos->size(); ++i) {
//        delete &length_n_fifos[i];
//    }
//    delete length_n_fifos;
    return EXIT_SUCCESS;
}

int generate_sorted_list() {
    clean_words = WordFilter::task_filter(&dirty_file_path);
    auto *index_lists = create_indexes_list();
    th_priorities = calculate_thread_priorities(index_lists);
    pthread_t map_thread;
    pthread_t reduce_thread;

    if (pthread_create(&map_thread, nullptr, &map4, index_lists) != 0) {
        std::cerr << "Failed to create map thread." << std::endl;
        return EXIT_FAILURE;
    }
    if (pthread_create(&reduce_thread, nullptr, &reduce4, nullptr) != 0) {
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

int main(int argc, char **argv) {
    auto task4_run = time_func(generate_sorted_list);
//    sleep(15);
//    pthread()
    if (task4_run.second == 0) {
        std::cout << std::endl << "Time taken in seconds: " << task4_run.first << "s." << std::endl;
    }

    // FIXME Causes error?
//    unlink_fifos();

    return task4_run.second;
}