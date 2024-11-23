#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>
#include <thread>
#include <sstream>
#include "async.h"
#include "cmd_reader.h"
#include "parser.h"
#include "safe_queue.h"
#include <boost/asio.hpp>

class Context
{
public:
    Context(std::size_t bulk_size);
    std::mutex context_mutex_;

private:
    std::size_t bulk_size_;
};

class Controller
{
public:
    static Controller& Instance()
    { 
        static Controller instance;                
        return instance; 
    }

    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;
    void StopThreads();
    std::unordered_map<async::handle_t, std::shared_ptr<Context>>& GetContextList();
    async::handle_t GetNewConnectHandle(size_t bulk);


    void AddStaticCommands(const std::vector<std::string>& commands);
    void FlushStaticCommands();
    void ProcessData(std::istream& input);

private:
    Controller();
    ~Controller();

    void PrintToConsole();
    void PrintToFile();    
 
    bool stop_threads_{false};
    std::thread console_thread_{}, file_thread_1_{}, file_thread_2_{};
    std::unordered_map<async::handle_t, std::shared_ptr<Context>> context_container_{};
    std::unordered_multiset<std::string> shared_static_commands_{};
    std::mutex mix_mutex_{};
    std::size_t bulk_size_{3};
    Parser parser_{};
    CommandReader reader_{bulk_size_};

};


