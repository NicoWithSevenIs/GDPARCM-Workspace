#pragma once
#include "SFML/Graphics.hpp"
#include <string>
class Window
{
	protected:
		sf::RenderWindow m_hwnd;

	public:
		Window(float width, float height, std::string window_name = "Window");
		~Window();
		virtual void Run();

};

