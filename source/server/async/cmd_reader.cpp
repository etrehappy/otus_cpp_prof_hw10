#include "cmd_reader.h"
#include <iostream>

CommandReader::CommandReader(size_t N)
    :max_command_number_{N}, command_counter_{0}, container_{}, 
   eof_flag_{false}
{
    container_.resize(max_command_number_);
}


void CommandReader::Read(std::istream& input)
{   
    ClearState(); 

    while (command_counter_ < max_command_number_)
    {
        std::string command{};
        std::getline(input, command);
        
        if (!command.empty())
        {
            container_.push_back(std::move(command));
            ++command_counter_;
        }

        if (input.eof())
        {  
            eof_flag_ = true;
            break;
        }
    }   
}


std::vector<std::string>& CommandReader::GetCommands()
{
    return container_;
}

bool CommandReader::IsEof() const
{
    return eof_flag_;
}

void CommandReader::ClearState()
{
    command_counter_ = 0;
    eof_flag_ = false;
    container_.clear();
}