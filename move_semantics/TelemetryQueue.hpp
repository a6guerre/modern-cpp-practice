#ifndef TELEMETRY_QUEUE_HPP
#define TELEMETRY_QUEUE_HPP

#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cassert>

struct Telemetry {
    // hot metadata (small)
    uint64_t timestamp_ns;
    uint32_t seq;
    std::string topic;                // e.g., "imu", "gps", "power"

    // cold, potentially huge payload
    std::vector<std::byte> payload;   // compressed protobuf / flatbuffer blob

    // Moving is cheap (pointer/size steals); copying is expensive (deep copy).
};

class TelemetryQueue
{
public:
    explicit TelemetryQueue(size_t len) : max_len(len) { assert(max_len > 0); }
    void push_data(Telemetry&& data)
    {
        std::unique_lock<std::mutex> lock(m);
        not_full_cv.wait(lock, [&]{ return q.size() < max_len;});
        q.push_back(std::move(data));
        lock.unlock();
        not_empty_cv.notify_one();
    }

    Telemetry pop_data(void)
    {
        std::unique_lock<std::mutex> lock(m);
        not_empty_cv.wait(lock, [&] { return !q.empty(); });
        // get oldest data
        Telemetry data = std::move(q.front());
        q.pop_front();
        lock.unlock();
        not_full_cv.notify_one();
        return data;
    }

    size_t getSize(void)
    {
        std::unique_lock<std::mutex> lock(m);
        return q.size();
    }

private:
    std::deque<Telemetry> q;
    std::mutex m;
    std::condition_variable not_empty_cv;
    std::condition_variable not_full_cv;
    size_t max_len;
};

#endif
