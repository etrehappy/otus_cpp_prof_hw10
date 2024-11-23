#pragma once
#include <cstddef>

namespace async 
{

using handle_t = void *;

handle_t connect(std::size_t bulk);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

} // namespace async 

/*#include <cstddef>
#include <memory>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <chrono>

#include "cmd_reader.h"
#include "parser.h"
#include "safe_queue.h"
#include <filesystem>
#include <iostream>
#include <condition_variable>
#include <thread>*/







