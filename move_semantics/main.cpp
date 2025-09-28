#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>
#include "TelemetryQueue.hpp"

uint32_t last_sequence_num; // Telemetry data sequence number
using namespace std::chrono;
const int PAYLOAD_SIZE = 1024; // Simulated telemetry payload size received over the network
std::atomic<bool> running;
std::mutex m;

void writingDataToFile(Telemetry &&data)
{
    std::cout << "Simulating writing data to file ts: " << data.timestamp_ns << " seq: " << data.seq << " topic: " << data.topic << "\n";
    // Simulate writing to file taking longer
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
}

void getTelemetryFromNetwork(Telemetry &data)
{
    uint64_t ns = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
    data.timestamp_ns = ns;
    last_sequence_num = (last_sequence_num + 1) % UINT32_MAX;
    data.seq = last_sequence_num;
    data.topic = "imuFusedData";
    for (uint32_t i = 0; i < PAYLOAD_SIZE; ++i)
    {
        data.payload.emplace_back(std::byte(i % UINT8_MAX));
    }
}

void producer_via_move(TelemetryQueue &q)
{
    int i = 0;
    const int ITER = 100;
    Telemetry data;
    // Produces 100 elements so we can time and compare move and copy for pushing data into queue
    auto start = steady_clock::now();
    while (i < 100)
    {
        std::unique_lock<std::mutex> lock(m);
        if (!running)
        {
            std::cout << "Producing thread is exiting" << "\n";
            return;
        }
        lock.unlock();
        // Get simulated telemetry data over the network;
        getTelemetryFromNetwork(data);
        // Push simulated data to queue, queue is implemented in a thread-safe maner
        q.push_data(std::move(data));
        ++i;
    }
    auto stop  = steady_clock::now();
    auto ms = duration_cast<milliseconds>(stop - start).count();
    std::cout << "producing and pushing 100 elements in queue via move took " << ms << " ms\n";
}

void producer_via_copy(TelemetryQueue &q)
{
    int i = 0;
    const int ITER = 100;
    Telemetry data;
    // Produces 100 elements so we can time and compare move and copy for pushing data into queue
    auto start = steady_clock::now();
    while (i < 100)
    {
        std::unique_lock<std::mutex> lock(m);
        if (!running)
        {
            std::cout << "Producing thread is exiting" << "\n";
            return;
        }
        lock.unlock();
        // Get simulated telemetry data over the network;
        getTelemetryFromNetwork(data);
        // Push simulated data to queue, queue is implemented in a thread-safe maner
        q.push_data_copy(data);
        ++i;
    }
    auto stop  = steady_clock::now();
    auto ms = duration_cast<milliseconds>(stop - start).count();
    std::cout << "producing and pushing 100 elements in queue via move took " << ms << " ms\n";

}

void consumer(TelemetryQueue &q)
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m);
        if (!running)
        {
            std::cout << "Consuming thread is exiting" << "\n";
            return;
        }
        lock.unlock();
        // Get telemetry data from queue
        writingDataToFile(std::move(q.pop_data()));
    }
}

void handleSigint(int signum)
{
    if (signum == SIGINT)
    {
        std::cout << "SIGINT Caught. Program clean-up\n";
        running = false;
    }
}

int main(void)
{
    TelemetryQueue telemetryQ(100);
    running = true;
    std::signal(SIGINT, handleSigint);
    // Uncomment only one of the producers to show example
    std::thread producer_thread(producer_via_move, std::ref(telemetryQ));
    //std::thread producer_thread(producer_via_copy, std::ref(telemetryQ));
    std::thread consumer_thread(consumer, std::ref(telemetryQ));
    producer_thread.join();
    consumer_thread.join();
    return 0;
}
