#include "Logger.h"

void Logger::Log(const std::string& functionName, const std::string& message)
{
	std::cout << "[" << functionName << "] : " << message << std::endl;
}
