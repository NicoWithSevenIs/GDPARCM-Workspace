#include "Window.h"

Window::Window(float width, float height, std::string window_name):
	m_hwnd(sf::RenderWindow(sf::VideoMode(width, height), window_name))
{}

Window::~Window(){}

void Window::Run()
{
    // Create a circle shape
    sf::CircleShape circle(100.f); // Radius of 100 pixels
    circle.setFillColor(sf::Color::Green); // Set its color to green

    while (m_hwnd.isOpen())
    {
        sf::Event event;
        while (m_hwnd.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                m_hwnd.close();
        }

        m_hwnd.clear();
        m_hwnd.draw(circle);
        m_hwnd.display();
    }
}
