#include "controller.h"
#include <filesystem>
#include <iostream>

#include <fstream>

namespace async
{

    static std::mutex kCoutMutex{}, kFileOutMutex{}, kQueueMutex{};
    static const char* kDirPath{"./logs/"};

    std::condition_variable kConsoleCv{}, kFileCv{};
    ThreadSafeQueue kQueue{};
    Controller* kController{nullptr};
    static std::ostream& cout = std::cout;
} // namespace async 

Controller::Controller()
{
    if (!std::filesystem::exists(async::kDirPath))
    {
        std::filesystem::create_directories(async::kDirPath);
    }

    console_thread_ = std::thread(&Controller::PrintToConsole, this);
    file_thread_1_ = std::thread(&Controller::PrintToFile, this);
    file_thread_2_ = std::thread(&Controller::PrintToFile, this);
}


Controller::~Controller() {
    StopThreads();
    async::kQueue.Clear();
}


void Controller::StopThreads()
{
    FlushStaticCommands();

    {
        std::unique_lock<std::mutex> lock(async::kCoutMutex);
        stop_threads_ = true;
    }

    if (console_thread_.joinable()) { console_thread_.join(); }
    if (file_thread_1_.joinable()) { file_thread_1_.join(); }
    if (file_thread_2_.joinable()) { file_thread_2_.join(); }
}

std::unordered_map<async::handle_t, std::shared_ptr<Context>>& Controller::GetContextList()
{
    return context_container_;
}

async::handle_t Controller::GetNewConnectHandle(size_t bulk)
{
    auto temp_context = std::make_shared<Context>(bulk);
    async::handle_t handle = reinterpret_cast<async::handle_t>(temp_context.get());
    context_container_[handle] = std::move(temp_context);
    return handle;
}

void Controller::AddStaticCommands(const std::vector<std::string>& commands)
{
    for (const auto& command : commands) 
    { 
        shared_static_commands_.insert(command);
    }
}


void Controller::FlushStaticCommands() 
{
    if (!shared_static_commands_.empty()) 
    {
        std::vector<std::string> current_vector;
        for (const auto& command : shared_static_commands_) 
        { 
            current_vector.push_back(command); 
            if (current_vector.size() == bulk_size_)
            {            
                async::kQueue.MovePush(current_vector);
                current_vector.clear(); 
            } 
        } 

        if (!current_vector.empty())
        {
            async::kQueue.MovePush(current_vector);
        }

        shared_static_commands_.clear();
    }
}

void Controller::PrintToConsole() {
    std::unique_lock<std::mutex> lock(async::kCoutMutex);
    while (true) {
        if (async::kQueue.IsEmpty()) {
            if (stop_threads_) return;
            async::kConsoleCv.wait(lock, [this] { return !async::kQueue.IsEmpty() || stop_threads_; });
        }

        if (stop_threads_ && async::kQueue.IsEmpty()) return;

        QueueItem* item = async::kQueue.GetFront();
        if (item == nullptr || item->console_ready_) {
            continue;
        }

        async::cout << "bulk: ";
        for (size_t i = 0; i < item->data_.size(); ++i) {
            async::cout << item->data_[i];
            if (i < item->data_.size() - 1) {
                async::cout << ", ";
            }
        }
        async::cout << std::endl;

        if (item->log_ready_) {
            async::kQueue.Pop();
            continue;
        }
        item->console_ready_ = true;
    }
}


static int kFileCounter = 0;

void Controller::PrintToFile()
{
    std::unique_lock<std::mutex> lock(async::kFileOutMutex);


    while (true)
    {
        /* 1. Проверка */

        if (async::kQueue.IsEmpty()) {
            if (stop_threads_) return;
            async::kFileCv.wait(lock, [this] { return !async::kQueue.IsEmpty() || stop_threads_; });
        }
        if (stop_threads_ && async::kQueue.IsEmpty()) return;


        QueueItem* item = async::kQueue.GetFront();

        if (item == nullptr || item->log_ready_) { continue; }

        /* 2. Подготовка */

        std::ostringstream oss_t_id;
        oss_t_id << std::this_thread::get_id();
        const std::string& thread_id = oss_t_id.str();

        std::string time = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

        std::ofstream out_file(std::string{async::kDirPath} + "bulk_" + thread_id + "_" + time + std::to_string(++kFileCounter) + ".log", std::ios::app);

        /* 3. Вывод */

        if (item->data_.size() > 1)
        {
            for (auto it = item->data_.cbegin(); it != item->data_.cend() - 1; it++)
            {
                out_file << *it << ", ";
            }
        }
        if (item->data_.back() != "\n")
        {
            out_file << item->data_.back();
        }


        /* 4. Удаление */

        if (item->console_ready_)
        {
            async::kQueue.Pop();
            continue;
        }
        item->log_ready_ = true;

    }

}


Context::Context(std::size_t s)
    :bulk_size_{s}
{
}

void Controller::ProcessData(std::istream& input)
{
    parser_.ClearState();
    reader_.ClearState();

    while (!reader_.IsEof())
    {
        reader_.Read(input);
        parser_.Analyses(reader_.GetCommands());
    }

    if (reader_.IsEof())
    {
        async::kController->FlushStaticCommands();
    }
}