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
    /*
 * 'wait' waits for a child process to terminate, and returns that child process's pid. On error (eg when there are no
 * child processes), -1 is returned. So, basically, the code keeps waiting for child processes to finish, until the
 * wait-ing errors out, and then you know they are all finished.
 *
 * So, the childs with no children (leafs) will terminate immediately, since they don't have anything to wait for
 * and -1 will be returned as soon as this loop is checked.
 */
    if (id != 0) {
        wait(NULL);
    }

//    std::cout << __cplusplus << std::endl;
    return EXIT_SUCCESS;
}