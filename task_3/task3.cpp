#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <pthread.h>
#include "../utilities/function_timer.h"
#include "../task_1/word_filter.h"
#include "../utilities/utils.h"

std::string dirty_file_path;
std::string clean_file_path;
std::vector<std::string> *clean_words;
std::vector<std::vector<long unsigned int>> index_lists(13);

struct WriteFunctionArgs {
    std::string fifo_path;
    std::vector<long unsigned int> index3;
};

void *read_from_fifo(void *fifo_path) {
    auto *path = static_cast<std::string *>(fifo_path);
    int fd = open(path->c_str(), O_RDONLY);
    auto *curr_word = new std::string();
    auto *length_n_fifo = new std::vector<std::string>();

    printf("Reading in file %s. Then creating a vector of these length n words.\n", path->c_str());
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

    printf("Sorting (and writing to file) words of length %lu. There are %zu words in this list.\n",
           (*clean_words)[arguments->index3[0]].length(), arguments->index3.size());
    std::sort(arguments->index3.begin(), arguments->index3.end(), sort_rule);

    for (unsigned long i: arguments->index3) {
        if (write(fd, &(*clean_words)[i], sizeof((*clean_words)[i])) == -1) {
            fprintf(stderr, "Writing to a length n fifo failed!\n");
            return (void *) 9;
        }
    }
    close(fd);
    delete arguments;

    return EXIT_SUCCESS;
}

void *map3(void *) {
    // Fill the index_lists vector.
    printf("Building index list.\n");
    for (long unsigned int j = 0; j < clean_words->size(); ++j) {
        auto word_length = (*clean_words)[j].length();
        auto length_to_position = word_length - MIN_WORD_LENGTH;
        index_lists[length_to_position].push_back(j);
    }

    pthread_t workers[13];
    for (short m = MIN_WORD_LENGTH; m <= MAX_WORD_LENGTH; ++m) {
        auto *args = new WriteFunctionArgs{
                .fifo_path = "fifo_files/length_" + std::to_string(m) + ".txt",
                .index3 = index_lists[m - MIN_WORD_LENGTH]
        };

        printf("Pass fifo file path (%s) and index3 list to upcoming map thread.\n", args->fifo_path.c_str());
        if (pthread_create(&workers[m - MIN_WORD_LENGTH], nullptr, &write_to_fifo, args) != 0) {
            fprintf(stderr, "Failed to create worker thread.\n");
            return (void *) 6;
        }

        printf("Created map worker thread with id %lu.\n", workers[m - MIN_WORD_LENGTH]);
    }

    printf("Join all map worker threads.\n");
    for (unsigned long worker: workers) {
        if (pthread_join(worker, nullptr) != 0) {
            fprintf(stderr, "Failed to join worker thread.\n");
            return (void *) 7;
        }
    }

    return EXIT_SUCCESS;
}

void *reduce3(void *) {
    auto *length_n_fifos = new std::vector<std::vector<std::string>>();
    pthread_t workers[13];

    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        auto *fifo_path = new std::string("fifo_files/length_" + std::to_string(i) + ".txt");
        printf("Pass fifo file path (%s) to upcoming reduce thread.\n", fifo_path->c_str());
        if (pthread_create(&workers[i - MIN_WORD_LENGTH], nullptr, &read_from_fifo, fifo_path) != 0) {
            fprintf(stderr, "Failed to create worker thread.\n");
            return (void *) 8;
        }
        printf("Created reduce worker thread with id %lu.\n", workers[i - MIN_WORD_LENGTH]);
    }

    printf("Join all reduce worker threads.\n");
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
    length_n_fifos->erase(length_n_fifos->begin(), length_n_fifos->end());
    delete length_n_fifos;
    return EXIT_SUCCESS;
}

int generate_sorted_list() {
    printf("Obtaining clean words list.\n");
    clean_words = WordFilter::task_filter(&dirty_file_path);
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
    if (pthread_create(&map_thread, nullptr, &map3, nullptr) != 0) {
        fprintf(stderr, "Failed to create map thread.\n");
        return EXIT_FAILURE;
    }
    printf("Begin reduce process and create reduce map thread with id %lu...\n", reduce_thread);
    if (pthread_create(&reduce_thread, nullptr, &reduce3, nullptr) != 0) {
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

    for (auto &index_list: index_lists) {
        printf("There are %zu words in the list of length %lu. In total, this %.2f%% of the total words.\n",
               index_list.size(),
               (*clean_words)[index_list[0]].length(),
               ((double) index_list.size() / (double) clean_words->size()) * 100.0);
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
                "You may only use the following syntax: \"./task3 [dirty_file_name] [clean_file_name] [graceful_exit_threshold_seconds]\"."
                "\nNote that the graceful exit threshold is OPTIONAL, and will default to %d seconds if the argument is ignored.\n",
                GRACEFUL_EXIT_DEFAULT_THRESHOLD);
        return EXIT_FAILURE;
    }

    if (graceful_exit(&graceful_exit_threshold) != 0) {
        return EXIT_FAILURE;
    }

    auto task3_run = time_func(generate_sorted_list);
    if (task3_run.second == 0) {
        printf("Time taken in seconds: %fs.\n", task3_run.first);
    }

    return task3_run.second;
}