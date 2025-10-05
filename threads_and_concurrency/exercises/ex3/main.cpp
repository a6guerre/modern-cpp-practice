#include <iostream>
#include <thread>
#include <future>
#include <queue>
#include <chrono>
#include <stdexcept>
#include <csignal>
// Create a tiny worker thread that owns a queue of std::packaged_task<int()>.
// A submit(std::function<int()>) -> std::future<int> pushes a task and returns its future.
// Show clean shutdown (poison pill or flag).

// Improvements to be made
// mRunning in while loop is not thread safe
// Mutex is held too long after getting task from queue
// Shutdown semantics - do we train tasks or cancel them?
// Late submissions - API as written allows for submit even after shudtown
// Replace copy and assignment operators when appropriate
// Stretch goal: Make submit accept any callable

class taskQueue
{
public:
    taskQueue(bool gracefulExit) : mRunning(true), mGracefulExit(gracefulExit)
    {
        runWorkerThread();
    }

    ~taskQueue()
    {
        std::cout << "taskQueue destructor called\n";
        shutdown();       
    }

    std::future<int> submit(std::function<int()> func)
    {
        {
            std::unique_lock<std::mutex> lock(mMtx);
            if (!mRunning)
            {
                throw std::runtime_error("Attempting to submit when taskQueue is not running");
            }
        }

        std::packaged_task<int()> task(std::move(func));
        std::future<int> f = task.get_future();
        std::unique_lock<std::mutex> lock(mMtx);
        mTaskQueue.push(std::move(task));
        lock.unlock();
        mQueueCv.notify_one();
        std::cout << "Submitted task\n";
        return f;
    }
private:
    bool mRunning;
    bool mGracefulExit;
    std::mutex mMtx;
    std::condition_variable mQueueCv;
    std::thread t;
    std::queue<std::packaged_task<int()>> mTaskQueue;
    void prepareWorkerExit(void)
    {
        if (mGracefulExit)
        {
            while (true)
            {
                std::unique_lock<std::mutex> lock(mMtx);
                if (!mTaskQueue.empty())
                {
                    auto task = std::move(mTaskQueue.front());
                    mTaskQueue.pop();
                    lock.unlock();
                    std::cout << "Flushing out task to allow gracefull texit\n";
                    task();
                }
                else
                {
                    lock.unlock();
                    std::cout << "No remaining tasks. Exiting gracefully\n";
                    return;
                }
            }
        }
        else
        {
            std::cout << "Exiting non-gracefully, setting exception indicating promises were broken\n";
            while (true)
            {
                std::unique_lock<std::mutex> lock(mMtx);
                if (!mTaskQueue.empty())
                {
                    while (!mTaskQueue.empty())
                    {
                        mTaskQueue.pop();
                    }
                    lock.unlock();
                    std::cout << "Could not complete remaining tasks in queue due to shutdown\n";
                }
                else
                {
                    lock.unlock();
                    std::cout << "In non-graceful exit mode, but no remaining tasks. Exiting without throwing\n";
                    return;
                }
            }
        } 
    }

    void runWorkerThread(void)
    {
         t = std::thread([this](){
                                     while (true)
                                     {
                                         std::unique_lock<std::mutex> lock(mMtx);
                                         mQueueCv.wait(lock,
                                                       [this]()
                                                       { return !mTaskQueue.empty() || !mRunning; });
                                         if (!mRunning)
                                         {
                                             lock.unlock();
                                             prepareWorkerExit();
                                             break;
                                         }
                                         else
                                         {
                                             auto task = std::move(mTaskQueue.front());
                                             mTaskQueue.pop();
                                             lock.unlock();
                                             task();
                                         }
                                     }
                                     std::cout << "Worker thread is exiting\n";
                                });
        std::cout << "Worker thread is running\n";
    }

    void shutdown(void)
    {
        {
            std::unique_lock<std::mutex> lock(mMtx);
            mRunning = false;
        }
        mQueueCv.notify_all();
        std::cout << "Threads notified to shutdown\n";
        t.join();
        std::cout << "Worker thread joined\n";
    }
};

int task(void)
{
    std::cout << "Task doing work\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Task produced result\n";
    return 42;
}

int main(void)
{
    const int NUM_TASKS = 5;
    // true = graceful exit, pop and execute any remaining tasks if queue is non-empty when
    // receving shutdown
    taskQueue taskManager(true);
    std::vector<std::future<int>> futures;
    for (int i = 0; i < NUM_TASKS; ++i)
    { 
        futures.emplace_back(taskManager.submit(task));
    }

    for (auto &f : futures)
    {
        std::cout << "Result: " << f.get() << "\n";    
    }

    return 0;
}
