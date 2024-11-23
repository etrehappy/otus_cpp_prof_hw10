#pragma once
#include <vector>
#include <string>

class CommandReader
{
public:
	CommandReader(size_t);
	~CommandReader() = default;

	void Read(std::istream&);
	std::vector<std::string>& GetCommands();
	bool IsEof() const;
	void ClearState();
private:

	const size_t max_command_number_;
	size_t command_counter_;
	std::vector<std::string> container_;
	bool eof_flag_;
};


