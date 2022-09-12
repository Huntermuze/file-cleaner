#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <pthread.h>
#include "../utilities/function_timer.h"
#include "../utilities/utils.h"
#include "../task_1/word_filter.h"

std::string dirty_file_path;
std::string clean_file_path;
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
    if (nice(arguments->thread_priority) == -1 && (errno = 0) != 0) {
        fprintf(stderr, "A nice value for this read thread could not be set!\n");
        return (void *) 9;
    }
    int fd = open(arguments->fifo_path.c_str(), O_RDONLY);
    auto *curr_word = new std::string();
    auto *length_n_fifo = new std::vector<std::string>();

    printf("Reading in file %s. Then creating a vector of these length n words.\n", arguments->fifo_path.c_str());
    while (read(fd, curr_word, sizeof(*curr_word)) != 0) {
        length_n_fifo->push_back(std::string(*curr_word));
    }
    close(fd);
    delete arguments;

    return length_n_fifo;
}

void *write_to_fifo(void *args) {
    auto *arguments = (struct WriteFunctionArgs *) args;
    if (nice(arguments->thread_priority) == -1 && (errno = 0) != 0) {
        fprintf(stderr, "A nice value for this write thread could not be set!\n");
        return (void *) 9;
    }
    int fd = open(arguments->fifo_path.c_str(), O_WRONLY);
    auto sort_rule = [](const int &i1, const int &i2) -> bool {
        return (*clean_words)[i1].substr(MIN_WORD_LENGTH - 1) < (*clean_words)[i2].substr(MIN_WORD_LENGTH - 1);
    };

    printf("Sorting (and writing to file) words of length %lu. There are %zu words in this list.\n",
           (*clean_words)[arguments->index4[0]].length(), arguments->index4.size());
    std::sort(arguments->index4.begin(), arguments->index4.end(), sort_rule);

    for (unsigned long i: arguments->index4) {
        if (write(fd, &(*clean_words)[i], sizeof((*clean_words)[i])) == -1) {
            fprintf(stderr, "Writing to a length n fifo failed!\n");
            return (void *) 10;
        }
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

    for (unsigned long i = 0; i < index_lists->size(); ++i) {
        index_to_size_list[i] = {i, (*index_lists)[i].size()};
    }

    std::sort(index_to_size_list.begin(), index_to_size_list.end(), sort_rule);
    for (int j = -13; j < 0; ++j) {
        auto index = index_to_size_list[j + 13].first;
        th_priority_of_each_index_list[index] = (short) j;
    }

    return th_priority_of_each_index_list;
}

std::vector<std::vector<long unsigned int>> *create_indexes_list() {
    auto *index_lists = new std::vector<std::vector<long unsigned int>>(13);

    // Fill the index_lists vector.
    printf("Building index list.\n");
    for (long unsigned int j = 0; j < clean_words->size(); ++j) {
        auto word_length = (*clean_words)[j].length();
        auto length_to_position = word_length - MIN_WORD_LENGTH;
        (*index_lists)[length_to_position].push_back(j);
    }

    return index_lists;
}

void *map4(void *arg) {
    auto *index_lists = static_cast<std::vector<std::vector<long unsigned int>> *>(arg);
    pthread_t workers[13];

    for (short m = MIN_WORD_LENGTH; m <= MAX_WORD_LENGTH; ++m) {
        auto *args = new WriteFunctionArgs{
                .thread_priority = th_priorities[m - MIN_WORD_LENGTH],
                .fifo_path = "fifo_files/length_" + std::to_string(m) + ".txt",
                .index4 = (*index_lists)[m - MIN_WORD_LENGTH]
        };

        printf("Pass fifo file path (%s), index4 list and thread priority %d (where nice value is %d) to upcoming map thread.\n",
               args->fifo_path.c_str(), 20 + args->thread_priority, args->thread_priority);
        if (pthread_create(&workers[m - MIN_WORD_LENGTH], nullptr, &write_to_fifo, args) != 0) {
            fprintf(stderr, "Failed to create worker thread.\n");
            return (void *) 6;
        }

        printf("Created map worker thread with id %lu.\n", workers[m - MIN_WORD_LENGTH]);
    }

    for (unsigned long worker: workers) {
        if (pthread_join(worker, nullptr) != 0) {
            fprintf(stderr, "Failed to join worker thread.\n");
            return (void *) 7;
        }
    }

    return EXIT_SUCCESS;
}

void *reduce4(void *) {
    auto *length_n_fifos = new std::vector<std::vector<std::string>>();
    pthread_t workers[13];

    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        auto *args = new ReadFunctionArgs{
                .thread_priority = th_priorities[i - MIN_WORD_LENGTH],
                .fifo_path = "fifo_files/length_" + std::to_string(i) + ".txt",
        };
        printf("Pass fifo file path (%s) and thread priority %d (where nice value is %d) to upcoming reduce thread.\n",
               args->fifo_path.c_str(), 20 + args->thread_priority, args->thread_priority);
        if (pthread_create(&workers[i - MIN_WORD_LENGTH], nullptr, &read_from_fifo, args) != 0) {
            fprintf(stderr, "Failed to create worker thread.\n");
            return (void *) 8;
        }
        printf("Created reduce worker thread with id %lu.\n", workers[i - MIN_WORD_LENGTH]);
    }

    for (unsigned long worker: workers) {
        std::vector<std::string> *length_n_fifo;
        if (pthread_join(worker, (void **) &length_n_fifo) != 0) {
            fprintf(stderr, "Failed to join worker thread.\n");
            return (void *) 9;
        }
        length_n_fifos->push_back(*length_n_fifo);

    }
    std::sort(length_n_fifos->begin(), length_n_fifos->end(), WordFilter::compare_vector_of_string);
    printf("Perform merge algorithm, and then write final result to file %s\n", clean_file_path.c_str());
    WordFilter::merge_and_write(length_n_fifos, clean_file_path);
//    for (int i = 0; i < length_n_fifos->size(); ++i) {
//        delete &length_n_fifos[i];
//    }
//    delete length_n_fifos;
    return EXIT_SUCCESS;
}

int generate_sorted_list() {
    printf("Obtaining clean words list.\n");
    clean_words = WordFilter::task_filter(&dirty_file_path);
    auto *index_lists = create_indexes_list();
    th_priorities = calculate_thread_priorities(index_lists);
    pthread_t map_thread;
    pthread_t reduce_thread;

    // Create 13 FIFO files IFF they do not exist.
    for (short k = MIN_WORD_LENGTH; k <= MAX_WORD_LENGTH; ++k) {
        auto fifo_path = "fifo_files/length_" + std::to_string(k) + ".txt";
        int status = mkfifo(fifo_path.c_str(), 0777);

        if (status == -1 && errno != EEXIST) {
            fprintf(stderr, "Failed to create a FIFO file.\n");
            return 5;
        } else if (status == 0) {
            printf("Successfully created fifo file: %s.\n", fifo_path.c_str());
        }
    }

    printf("Begin mapping process and create map thread with id %lu...\n", map_thread);
    if (pthread_create(&map_thread, nullptr, &map4, index_lists) != 0) {
        fprintf(stderr, "Failed to create map thread.\n");
        return EXIT_FAILURE;
    }
    printf("Begin reduce process and create reduce map thread with id %lu...\n", reduce_thread);
    if (pthread_create(&reduce_thread, nullptr, &reduce4, nullptr) != 0) {
        fprintf(stderr, "Failed to create reduce thread.\n");
        return 2;
    }
    if (pthread_join(map_thread, nullptr) != 0) {
        fprintf(stderr, "Failed to join map thread.\n");
        return 3;
    }
    if (pthread_join(reduce_thread, nullptr) != 0) {
        fprintf(stderr, "Failed to reduce map thread.\n");
        return 4;
    }

    delete clean_words;
    unlink_fifos();

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // Skipping proper validation & error-checking, as this is not important in this assignment.
    // Default to 15 seconds.
    int graceful_exit_threshold = GRACEFUL_EXIT_DEFAULT_THRESHOLD;
    if (argc == 4) {
        dirty_file_path = argv[1];
        clean_file_path = argv[2];
        graceful_exit_threshold = std::stoi(argv[3]);
    } else if (argc == 3) {
        dirty_file_path = argv[1];
        clean_file_path = argv[2];
    } else {
        fprintf(stderr,
                "You may only use the following syntax: \"./task4 [dirty_file_name] [clean_file_name] [graceful_exit_threshold_seconds]\"."
                "\nNote that the graceful exit threshold is OPTIONAL, and will default to %d seconds if the argument is ignored.\n",
                GRACEFUL_EXIT_DEFAULT_THRESHOLD);
        return EXIT_FAILURE;
    }

    if (graceful_exit(&graceful_exit_threshold) != 0) {
        return EXIT_FAILURE;
    }

    auto task4_run = time_func(generate_sorted_list);
    if (task4_run.second == 0) {
        printf("Time taken in seconds: %fs.\n", task4_run.first);
    }

    return task4_run.second;
}