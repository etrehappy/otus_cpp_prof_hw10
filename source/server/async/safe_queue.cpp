#include "safe_queue.h"

QueueItem::QueueItem(std::vector<std::string>&& d)
    : data_(std::move(d))
{
}

void ThreadSafeQueue::MovePush(std::vector<std::string>& value)
{
    {
        std::lock_guard<std::mutex> lock(q_mutex_);
        queue_.push(QueueItem{std::move(value)});
    }

    async::kConsoleCv.notify_one();
    async::kFileCv.notify_all();
}

QueueItem* ThreadSafeQueue::GetFront()
{
    std::lock_guard<std::mutex> lock(q_mutex_);
    if (queue_.empty()) {
        return nullptr;
    }
    return &queue_.front();
}


void ThreadSafeQueue::Pop()
{
    std::lock_guard<std::mutex> lock(q_mutex_);
    queue_.pop();
}

bool ThreadSafeQueue::IsEmpty()
{
    std::lock_guard<std::mutex> lock(q_mutex_);
    return queue_.empty();
}

void ThreadSafeQueue::Clear() {
    std::lock_guard<std::mutex> lock(q_mutex_);
    while (!queue_.empty()) {
        queue_.pop();
    }
}



