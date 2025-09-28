#include <stdexcept>
#include <thread>
#include <future>
#include <string>
#include <iostream>

void task1(void)
{
    throw std::runtime_error("boom");
}

void task2(std::promise<std::string> &p, bool success)
{
    try
    {
        if (success)
        {
            p.set_value("hello");
        }
        else
        {
            throw std::runtime_error("Boom");
        }
    }
    catch (const std::exception& e)
    {
        p.set_exception(std::current_exception());
    }
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

    // A use try-catch in main from exception thrown by thread. (Won't see exception gracefully)
    // Run either A, B, or B2
    /*std::promise<std::string> p;
    std::future<std::string> f = p.get_future();
    std::thread t(task1);
    try
    {
        t.join(); // Won't catch
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception occured " << e.what();
    }*/

    // B
    // std::async with a shared state will allow us to catch the error in main
    // thread
  /*  auto fut = std::async(std::launch::async, []() -> int
                                   {
                                       throw std::runtime_error("boom");
                                   });
    try
    {
        int val = fut.get();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
    }*/
    // B2 
    std::promise<std::string> p;
    auto f = p.get_future();
    std::thread t(task2, std::ref(p), false);
    try
    {
        std::cout << f.get() << "\n";
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
    }
    t.join();
    return 0;
}
