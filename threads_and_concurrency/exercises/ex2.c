#include <stdexcept>
#include <thread>
#include <future>
#include <string>
#include <iostream>
#include "upper_helpers.h"

void task(std::promise<std::string> &p)
{
    p.set_exception(std::runtime_error("Error occured"));
    return 0;
}

int main() {
    // Part A:
    //   Start a background execution agent that processes input guaranteed to fail.
    //   Attempt to catch an error only around the main thread’s synchronization step.
    //   Observe and explain whether the error is intercepted there.
    //
    // Part B:
    //   Redo the scenario, but route any error from the background agent to the main thread
    //   using a write/read channel pair.
    //   Demonstrate consumption of the result on success and rethrow on failure.
    //
    // Part C (reflection):
    //   Explain when abnormal termination may occur in a background agent and how to avoid it.

    // A
    std::promise<std::string> p;
    std::future<std::string> f = p.get_future();
    std::thread t(task,std::ref(p));
    try
    {
        std::cout << "Future's shared state value " << f.get() << "\n";
    }
    catch (std::exception &e)
    {
        std::cout << "Exception occured " << e.what();
    }

    return 0;
}
