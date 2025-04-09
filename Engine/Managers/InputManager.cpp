#include "InputManager.h"

InputManager* InputManager::m_Instance = nullptr;

InputManager* InputManager::Instance()
{
	if (m_Instance == nullptr)
		m_Instance = new InputManager();
	return m_Instance;
}

void InputManager::Destroy()
{
	if (m_Instance != nullptr)
		delete m_Instance;
	m_Instance = nullptr;
}

void InputManager::UpdatePrevious()
{
	for (auto& key : m_KeyStatus) {
		key.second.previous = key.second.current;
	}

	for (auto& button : m_ButtonStatus) {
		button.second.previous = button.second.current;
	}

	m_LastMouseX = m_MouseX;
	m_LastMouseY = m_MouseY;
}

void InputManager::SetKeyboardKey(int key, bool active)
{
	m_KeyStatus[key].current = active;
}

bool InputManager::IsKeyPressed(int key) {
	return m_KeyStatus[key].current && !m_KeyStatus[key].previous;
}

bool InputManager::IsKeyHeld(int key) {
	return m_KeyStatus[key].current;
}

bool InputManager::IsKeyReleased(int key) {
	return !m_KeyStatus[key].current && m_KeyStatus[key].previous;
}

void InputManager::SetMouseButton(int button, bool active)
{
	m_ButtonStatus[button].current = active;
}

bool InputManager::IsButtonPressed(int key)
{
	return m_ButtonStatus[key].current && !m_ButtonStatus[key].previous;
}

bool InputManager::IsButtonHeld(int key)
{
	return m_ButtonStatus[key].current;
}

bool InputManager::IsButtonReleased(int key)
{
	return !m_ButtonStatus[key].current && m_ButtonStatus[key].previous;
}

void InputManager::SetMousePosition(double x, double y)
{
	m_MouseX = x;
	m_MouseY = y;
}

std::pair<double, double> InputManager::GetMouseDelta() const
{
	return { m_MouseX - m_LastMouseX, m_MouseY - m_LastMouseY };
}

std::pair<double, double> InputManager::GetMousePosition() const
{
	return { m_MouseX, m_MouseY };
}