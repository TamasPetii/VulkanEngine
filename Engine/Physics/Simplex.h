#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>

class ENGINE_API Simplex
{
public:
	Simplex& operator=(const std::vector<glm::vec3>& list);
	glm::vec3& operator[](int i);
	void PushFront(const glm::vec3& point);
	uint32_t GetSize() const { return size; }
private:
	uint32_t size = 0;
	std::array<glm::vec3, 4> points;
};
