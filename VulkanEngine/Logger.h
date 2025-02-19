#pragma once
#include <iostream>
#include <string>

class Logger
{
public:
	static void Log(const std::string& functionName, const std::string& message);
};

