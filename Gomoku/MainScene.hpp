#pragma once
#include "BaseScene.hpp"

class MainScene : public BaseScene
{
private:
	sf::VertexArray m_rectangle{sf::PrimitiveType::Quads, 4};

public:
	MainScene(sf::RenderWindow& window)
		: BaseScene(window) 
	{
		m_rectangle[0].position = { -50, -50 };
		m_rectangle[1].position = { 50, -50 };
		m_rectangle[2].position = { 50, 50 };
		m_rectangle[3].position = { -50, 50 };
		setColor(m_rectangle, sf::Color::Green);
	}
	void render() override {
		m_window.draw(m_rectangle);
	}
	void update() override {}
	bool handleInput(sf::Event event) override {
		return true;
	}
private:
	void setColor(sf::VertexArray& arr, sf::Color color) {
		auto count = arr.getVertexCount();
		for (uint32_t i = 0; i < count; ++i) {
			arr[i].color = color;
		}
	}

};

