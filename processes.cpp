#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int id = fork();

    // This means that the parent will finish executing and thus, terminate before the child has finished executing.
    // This means the child becomes parentless, and is thus a zombie. However, it is assigned a NEW parent as soon
    // as this happens (i.e., gets a new process).
    if (id == 0) {
        sleep(1);
    }

    printf("Current ID: %d, parent ID: %d\n", getpid(), getppid());

    // Wait for the child to die before main process terminates. Comment out to see parent process dying before child.
    if (id != 0) {
        wait(NULL);
    }

//    std::cout << __cplusplus << std::endl;
    return EXIT_SUCCESS;
}