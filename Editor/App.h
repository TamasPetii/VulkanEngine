#pragma once
#include "../Engine/Engine.h"
#include "Gui/Gui.h"
#include <memory>

struct GLFWwindow;

class App
{
public:
	App();
	~App();
	void Run();
	auto GetEngine() { return engine; }
private:
	void Init();
	void Clear();
private:
	void InitWindow();
	void InitEngine();
	void InitGui();
private:
	GLFWwindow* window;
	std::shared_ptr<Gui> gui;
	std::shared_ptr<Engine> engine;
};

