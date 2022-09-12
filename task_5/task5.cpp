#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <pthread.h>
#include "../task_1/word_filter.h"
#include "../utilities/utils.h"

#define cursup "\033[A"

// THE ORIGINAL PLAN:
// MAP opens the fifos for reading, and sends the word to the appropriate reduce thread, which the reduce thread
// then increments the counter for that length n word and another thread displays the word_length_counts on screen real-time.
// This was SCRAPPED because it is very inefficient, and takes a lot more unnecessary code to get working, as opposed to
// the current way.
bool refresh_data = true;
int word_length_counts[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void *map5(void *arg) {
    auto *path = static_cast<std::string *>(arg);
    int fd = open(path->c_str(), O_RDONLY);
    auto *curr_word = new std::string();

    while (read(fd, curr_word, sizeof(*curr_word)) != 0) {
        ++word_length_counts[curr_word->length() - MIN_WORD_LENGTH];
    }

    close(fd);
//    delete curr_word;

    return EXIT_SUCCESS;
}

void *display_count(void *) {
    printf("LIST DATA: (COUNT), (RATIO)\n");
    unsigned long sum;

    while (refresh_data) {
        int point_zero_five_of_second = 50 * 1000;
        usleep(point_zero_five_of_second);
        sum = 0;

        for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
            sum += word_length_counts[i - MIN_WORD_LENGTH];
        }

        for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
            int count = word_length_counts[i - MIN_WORD_LENGTH];
            double ratio = (sum == 0) ? 0.0 : ((double) count / (double) sum) * 100;
            printf("Length %d list data: %d, %.2f%%\n", i, count, ratio);
        }

        for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
            printf("%s", cursup);
        }
        printf("\r");
    }

    // Bring cursor back to where it should be.
    for (int i = MIN_WORD_LENGTH; i <= MAX_WORD_LENGTH; ++i) {
        printf("\n");
    }

    return EXIT_SUCCESS;
}

int run_task5() {
    auto *fifo_path = new std::string("task_5_fifo");
    pthread_t map_thread;
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
        fprintf(stderr, "Failed to create display_count thread.\n");
        return 5;
    }

    char current_word[16];
    std::string to_write;
    int fd = open(fifo_path->c_str(), O_WRONLY);
    while (scanf("%15s", current_word) != EOF) {
        to_write = current_word;
        if (write(fd, &to_write, sizeof(to_write)) == -1) {
            fprintf(stderr, "Writing to the fifo failed!\n");
            return 11;
        }
    }
    close(fd);
    // We will wait half a second, so that the display_count thread can print the most updated version of the word_length_counts
    // list (ALWAYS), otherwise, depending on the sleep amount in the display_count thread, it may terminate before the most updated
    // sizes are printed, making the output per run sometimes different.
    usleep(500 * 1000);
    refresh_data = false;

    if (pthread_join(display_count_thread, nullptr) != 0) {
        fprintf(stderr, "Failed to join display_count thread.\n");
        return 9;
    }

    // Join map thread at the end, ONCE it is finished.
    if (pthread_join(map_thread, nullptr) != 0) {
        fprintf(stderr, "Failed to join map thread.\n");
        return 3;
    }
    printf("\n");
    unlink(fifo_path->c_str());
    delete fifo_path;
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    // Default to 15 seconds.
    int graceful_exit_threshold = GRACEFUL_EXIT_DEFAULT_THRESHOLD;
    if (argc == 2) {
        graceful_exit_threshold = std::stoi(argv[1]);
    }

    if (graceful_exit(&graceful_exit_threshold) != 0) {
        return EXIT_FAILURE;
    }

    return run_task5();
}
