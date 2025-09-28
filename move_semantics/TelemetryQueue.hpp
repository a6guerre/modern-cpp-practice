#ifndef TELEMETRY_QUEUE_HPP
#define TELEMETRY_QUEUE_HPP

#include <iostream>
#include <deque>

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
    void push_data(Telemetry data)
    {
        std::unique_lock<std::mutex> lock(m);
        if (q.size() == max_len)
        {
            cv.wait(lock, []{ return q.size() < max_len;});
        }
        q.push_back(std::move(data));
    }

    bool pop_data(Telemetry &data)
    {
        std::unique_lock<std::mutex> lock(m);
        if (!q.empty())
        {
            // get oldest data
            data = std::move(q.back());
            q.pop();
            if (q.size() == max_len - 1)
            {
                cv.notify_one();
            }
            return true;
        }
        else
        {
            std::cout << "Attempting to pop empty queue\n";
            return false;
        }
    }

    size_t getSize(void)
    {
        return q.get_size();
    }

private:
    std::deque<Telemetry> q;
    std::mutex m;
    std::contition_variable<
    size_t max_len;
};

#endif
