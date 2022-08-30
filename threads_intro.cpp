#include <iostream>
#include <thread>

// An example of concurrent execution. Might be parallel.
static bool s_Finished = false;

// Essentially the work to be done on a thread. Equivalent to a Runnable in Java.
void do_work() {
    // Allow usage of 1s.
    using namespace std::literals::chrono_literals;

    std::cout << "Started thread id = " << std::this_thread::get_id() << std::endl;

    while (!s_Finished) {
        std::cout << "Working...\n";
        // Sleep thread for 1 second so as not to have this thread using 100% CPU usage.
        std::this_thread::sleep_for(1s);
    }
}

int main() {
    std::thread worker(do_work);
    // Calling cin.get() starts a new thread that waits until the ENTER key is pressed. So it blocks the thread it was
    // called in (in this case the main thread). This thread and the do_work one
    // we established are being run "at the same time" (might be context-switched, thus running concurrently, or it might
    // actually be parallel). do_work prints "Working..." as fast as it can, and cin.get() is listening for an event (i.e.,
    // ENTER key being pressed). However, cin.get() thread is blocking (waiting for I/O - input in this case), so in
    // order for s_Finished (a global variable) to be changed to true, the thread needs to unblock or be destroyed. We
    // can simply unblock it by pressing ENTER, and then s_Finished = true; will execute. That will then terminate the
    // loop in do_work function, which will then terminate the thread that was running do_work function.
    std::cin.get();
    s_Finished = true;

    // Join is the same as wait() in other prog languages (Java, C#). Essentially, the main thread (the thread that
    // worker.join() is called in) will block UNTIL the worker thread has completed execution. Once worker has completed,
    // then std::cin.get() will finally run (waiting for ENTER, yet again).
    worker.join();
    std::cout << "Finished." << std::endl;
    std::cout << "Started thread id = " << std::this_thread::get_id() << std::endl;

    std::cin.get();
}