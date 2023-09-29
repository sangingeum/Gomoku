#pragma once
#include "BaseScene.hpp"
#include <iostream>
#include "Config.hpp"
#include "KDTree.hpp"
#include "Gomoku.hpp"

class MainScene : public BaseScene
{
private:
	sf::VertexArray m_rectangle{ sf::PrimitiveType::Quads, 4 };
	sf::VertexArray m_whiteCircle;
	sf::VertexArray m_blackCircle;
	sf::Texture m_floorTexture;
	using Point = typename std::array<int, 2>;
	using PointPair = typename std::pair<Point, Point>;
	KDTree<2, Point, int> m_tree;
	Gomoku m_game;
	int xOffset{ 270 }, yOffset{ 20 };
	int xIncrement{ 45 }, yIncrement{ 45 };
	sf::IntRect m_boardRect{270, 20, 670, 670};
	


public:
	MainScene(sf::RenderWindow& window)
		: BaseScene(window)
	{	
		loadAndInit();
		reset();
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
			if (m_boardRect.contains({ buttonPos.x, buttonPos.y })) {

				
				std::cout << buttonPos.x << " " << buttonPos.y << "\n";
				auto found = m_tree.findNearestNeighbor({ buttonPos.x, buttonPos.y });
				auto [x, y] = found.first;
				std::cout << found.second[1] << " " << found.second[0] << "\n";
				auto put = m_game.putPiece(found.second[0] + found.second[1]* 14);
				std::cout << "put: " << put << " is Over: " << m_game.isOver(found.second[0] + found.second[1] * 14) << "\n";
				std::cout << "turn: " << m_game.getTurn() << "\n";

				unsigned bestAction;
				if (put) {
					auto entity = m_registry.create();
					if (m_game.getTurn())
						m_registry.emplace<CRenderable>(entity, x, y, m_blackCircle);
					else
						m_registry.emplace<CRenderable>(entity, x, y, m_whiteCircle);

					unsigned bestAction = unsigned(m_game.minimax(3));
					std::cout << "Best action: " << bestAction << " Row: " << bestAction / 14 << " Col: " << bestAction % 14 << "\n";
				}
				
				
				std::cout << "Board evaluation: " << m_game.evaluate() << "\n";
				
				

				/*
				auto found = m_tree.findNearestNeighbor({ buttonPos.x, buttonPos.y });
				auto [x, y] = found.first;
				auto put = m_game.putPiece(found.second[0] + found.second[1]* 14);
				if (put) {
					auto entity = m_registry.create();
					if (m_game.getTurn())
						m_registry.emplace<CRenderable>(entity, x, y, m_blackCircle);
					else
						m_registry.emplace<CRenderable>(entity, x, y, m_whiteCircle);
				}
				auto bestAction = unsigned(m_game.minimax(3));
				*/
				
			}
			return true;
		}
		
		return false;
	}
private:
	void loadAndInit() {
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
		m_whiteCircle = makeCircle(20, 30, sf::Color::White);
		m_blackCircle = makeCircle(20, 30, sf::Color::Black);

		for (int i = 0; i < 15; ++i) {
			for (int j = 0; j < 15; ++j) {
				auto entity = m_registry.create();
				m_registry.emplace<CBackgroundRenderable>(entity, xOffset + i * xIncrement, yOffset + j * yIncrement, m_rectangle, &m_floorTexture);
			}
		}
		std::vector<PointPair> treeData;
		treeData.reserve(196); // 14*14
		for (int i = 1; i <= 14; ++i) {
			for (int j = 1; j <= 14; ++j) {
				treeData.push_back(std::make_pair(Point({ xOffset + i * xIncrement, yOffset + j * yIncrement }), Point({ i - 1, j - 1 })));
			}
		}
		m_tree.buildTree(treeData);
	}

	void reset() {
		m_game.reset();
	}


	void setColor(sf::VertexArray& arr, sf::Color color) {
		auto count = arr.getVertexCount();
		for (uint32_t i = 0; i < count; ++i) {
			arr[i].color = color;
		}
	}

	sf::VertexArray makeCircle(float radius, uint32_t sides, sf::Color color) {
		sf::VertexArray circle{ sf::PrimitiveType::TriangleFan, sides + 2 };
		auto pi = Config::instance().pi;
		circle[0].position = { 0, 0 };
		circle[0].color = color;
		auto step = (2 * pi) / sides;
		for (uint32_t i = 1; i <= sides+1; ++i) {
			circle[i].position = { radius * cos(step * i), radius * sin(step * i) };
			circle[i].color = color;
		}

		return circle;
	}

};

