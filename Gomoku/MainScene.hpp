#pragma once
#include "BaseScene.hpp"
#include <iostream>
#include "Config.hpp"

class MainScene : public BaseScene
{
private:
	sf::VertexArray m_rectangle{ sf::PrimitiveType::Quads, 4 };
	sf::VertexArray m_circle;
	sf::Texture m_floorTexture;
	//sf::Sprite m_floorSprite;
	float xOffset{ 270 }, yOffset{ 20 };
	float xIncrement{ 45 }, yIncrement{ 45 };
	sf::FloatRect m_boardRect{270, 20, 670, 670};
public:
	MainScene(sf::RenderWindow& window)
		: BaseScene(window) 
	{	
		loadAndInitAssets();
	
		for (uint32_t i = 0; i < 15; ++i) {
			for (uint32_t j = 0; j < 15; ++j) {
				auto entity = m_registry.create();
				m_registry.emplace<CBackgroundRenderable>(entity, xOffset + i * xIncrement, yOffset + j * yIncrement, m_rectangle, &m_floorTexture);
			}
		}

	}

	void render() override {

		m_registry.view<CBackgroundRenderable>().each([this](const CBackgroundRenderable& cRenderable) {
			m_window.draw(cRenderable.vertexArray, cRenderable.state);
			});
		m_registry.view<CRenderable>().each([this](const CRenderable& cRenderable) {
			m_window.draw(cRenderable.vertexArray, cRenderable.state);
			});
		

		
	}

	void update() override {}

	bool handleInput(sf::Event event) override {
		if (event.type == sf::Event::MouseButtonPressed){
			auto buttonPos = event.mouseButton;
			if (m_boardRect.contains({static_cast<float>(buttonPos.x), static_cast<float>(buttonPos.y)})) {
				std::cout << buttonPos.x << " " << buttonPos.y << "\n";
				auto entity = m_registry.create();
				m_registry.emplace<CRenderable>(entity, buttonPos.x, buttonPos.y, m_circle);
			}
		}
		return true;
	}
private:
	void loadAndInitAssets() {
		if (!m_floorTexture.loadFromFile("resources/dark_wood_diff_2k.jpg"))
			exit(-1);
		m_floorTexture.setSmooth(true);
		//m_floorSprite.setTexture(m_floorTexture);
		//m_floorSprite.setScale({ 0.0196f, 0.0196f });
		m_rectangle[0].position = { 0, 0 };
		m_rectangle[1].position = { 40, 0 };
		m_rectangle[2].position = { 40, 40 };
		m_rectangle[3].position = { 0, 40 };
		m_rectangle[0].texCoords = { 0, 0 };
		m_rectangle[1].texCoords = { 2048, 0 };
		m_rectangle[2].texCoords = { 2048, 2048 };
		m_rectangle[3].texCoords = { 0, 2048 };

		m_circle = makeCircle(20, 30);
	}

	void setColor(sf::VertexArray& arr, sf::Color color) {
		auto count = arr.getVertexCount();
		for (uint32_t i = 0; i < count; ++i) {
			arr[i].color = color;
		}
	}

	sf::VertexArray makeCircle(float radius, uint32_t sides) {
		sf::VertexArray circle{ sf::PrimitiveType::TriangleFan, sides + 2 };
		auto pi = Config::instance().pi;
		circle[0].position = { 0, 0 };
		auto step = (2 * pi) / sides;
		for (uint32_t i = 1; i <= sides+1; ++i) {
			circle[i].position = { radius * cos(step * i), radius * sin(step * i) };
		}

		return circle;
	}

};

