#pragma once
#include "entt.hpp"
#include "Component.hpp"
#include <SFML/Graphics.hpp>

class BaseScene
{
protected:
	entt::registry m_registry;
	sf::RenderWindow& m_window;
public:
	BaseScene(sf::RenderWindow& window) 
		: m_window(window)
	{};
	virtual ~BaseScene() = default;
	virtual void render() = 0;
	virtual void update() = 0;
	virtual bool handleInput(sf::Event event) = 0;
};

