#include <iostream>
#include <thread>
#include <future>

// Write int compute_with_timeout(std::function<int()> work, std::chrono::milliseconds dflt, int fallback)
// that launches work with std::async. If it doesn’t finish within dflt, return fallback;
// otherwise return the result. (Don’t block forever.)

int compute_with_timeout(std::function<int()> work, std::chrono::milliseconds dflt, int fallback)
{
    std::future<int> f = std::async(std::launch::async, std::move(work));
    auto status = f.wait_for(dflt);
    if (status == std::future_status::timeout)
    {
        std::cout << "Obtaining future from async timedout\n";
        return fallback;
    }
    else
    {
        std::cout << "No timeout, future data ready\n";
        return f.get();
    }
    
}

int main(void)
{
    // Play around with these times do trigger future being ready or timed-out
    const int SIMULATED_WORK_TIME_MS = 100;
    const int FUTURE_WAIT_TIME_MS = 10;

    int val = compute_with_timeout([](){ std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                         return 10; },
                                         std::chrono::milliseconds(10),42);
    std::cout << "Returned val: " << val << "\n";

}
