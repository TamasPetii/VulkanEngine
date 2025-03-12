#pragma once
#include <format>
#include <exception>
#include <iostream>

#define VK_CHECK(function) if(function != VK_SUCCESS) throw std::runtime_error(std::format("Error occurred in file {} at line {} in function {}", __FILE__, __LINE__, __FUNCTION__));
#define VK_CHECK_MESSAGE(function, message) if(function != VK_SUCCESS) throw std::runtime_error(std::format("{} In file {} at line {} in function {}", message ,  __FILE__, __LINE__, __FUNCTION__));
#define CHECK_ERROR_NULL(function) if(function == 0) throw std::runtime_error(std::format("Error occurred in file {} at line {} in function {}", __FILE__, __LINE__, __FUNCTION__));
#define CHECK_ERROR_NOTNULL(function) if(function != 0) throw std::runtime_error(std::format("Error occurred in file {} at line {} in function {}", __FILE__, __LINE__, __FUNCTION__));