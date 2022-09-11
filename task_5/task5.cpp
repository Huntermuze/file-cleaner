#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <iomanip>
#include <pthread.h>
#include "../utilities/function_timer.h"
#include "../task_1/word_filter.h"
#include "../utilities/utils.h"

#define cursup    "\033[A"

bool refresh_data = true;
int word_length_counts[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
std::vector<int *> reduce_file_descriptors(13);

// MAP opens the fifos for reading, and sends the word to the appropriate reduce thread, which the reduce thread
// then increments the counter for that length n word and another thread displays the word_length_counts on screen real-time.

void *map5(void *arg) {
    auto *path = static_cast<std::string *>(arg);
    int fd = open(path->c_str(), O_RDONLY);
    auto *curr_word = new std::string();

    while (read(fd, curr_word, sizeof(*curr_word)) != 0) {
        ++word_length_counts[curr_word->length() - MIN_WORD_LENGTH];
    }

    refresh_data = false;
    close(fd);
//    delete curr_word;

    return EXIT_SUCCESS;
}

void *display_count(void *) {
    printf("LIST DATA: (COUNT), (RATIO)\n");
    unsigned long sum = 0;

    while (refresh_data) {
//        //FIXME
        usleep(100000);
        sum = 0;

        for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
            sum += word_length_counts[i - MIN_WORD_LENGTH];
        }

//        for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
//            int count = word_length_counts[i - MIN_WORD_LENGTH];
//            double ratio = (sum == 0) ? 0.0 : ((double) count / (double) sum) * 100;
//            printf("Length %d list data: %d, %.2f%%\n", i, count, ratio);
//        }
//
//        for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
//            printf("%s", cursup);
//        }
//        printf("\r");
    }

    return EXIT_SUCCESS;
}

int run_task5() {
    auto *fifo_path = new std::string("task_5_fifo");
    pthread_t map_thread;
    pthread_t reduce_thread;
    pthread_t display_count_thread;

    int status = mkfifo(fifo_path->c_str(), 0777);

    if (status == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create a FIFO file.\n");
        return 10;
    } else if (status == 0) {
        printf("Successfully created fifo file: %s.\n", fifo_path->c_str());
    }

    if (pthread_create(&map_thread, nullptr, &map5, fifo_path) != 0) {
        fprintf(stderr, "Failed to create map thread.\n");
        return 2;
    }

    if (pthread_create(&display_count_thread, nullptr, &display_count, nullptr) != 0) {
        fprintf(stderr, "Failed to create reduce thread.\n");
        return 5;
    }

    char current_word[16];
    std::string to_write;
    int fd = open(fifo_path->c_str(), O_WRONLY);

    while (scanf("%15s", current_word) != EOF) {
        to_write = current_word;
        write(fd, &to_write, sizeof(to_write));
    }

    close(fd);

    // Join map thread at the end, ONCE it is finished.
    if (pthread_join(map_thread, nullptr) != 0) {
        fprintf(stderr, "Failed to join map thread.\n");
        return 3;
    }

    if (pthread_join(display_count_thread, nullptr) != 0) {
        fprintf(stderr, "Failed to join map thread.\n");
        return 9;
    }

    unlink(fifo_path->c_str());
    delete fifo_path;
    return EXIT_SUCCESS;
}


int main() {
    run_task5();
}

// TODO map literally passes it to the right reduce thread, and reduce increments a counter for that length_n_list and
//  (no need to store the words in the lists, just increment the counter) and then display the numbers, in real-time
//  to stdout on terminal.
