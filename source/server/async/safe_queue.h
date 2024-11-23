#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace async
{
    extern std::condition_variable kConsoleCv, kFileCv;
}

struct QueueItem
{
    QueueItem(std::vector<std::string>&& d);

    std::vector<std::string> data_;
    bool console_ready_{false}, log_ready_{false};
};

class ThreadSafeQueue
{
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    void MovePush(std::vector<std::string>& value);
    QueueItem* GetFront();
    void Pop();
    bool IsEmpty();
    void Clear();

private:
    std::queue<QueueItem> queue_{};
    std::mutex q_mutex_{};

};