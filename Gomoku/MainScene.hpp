#pragma once
#include "BaseScene.hpp"
#include "Config.hpp"
#include "KDTree.hpp"
#include "Gomoku.hpp"
#include <thread>
#include <atomic>

class MainScene : public BaseScene
{
private:
	sf::VertexArray m_flootRect;
	sf::VertexArray m_buttonRect;
	sf::VertexArray m_disabledButtonRect;
	sf::VertexArray m_enabledButtonRect;
	sf::VertexArray m_whiteCircle;
	sf::VertexArray m_blackCircle;
	sf::VertexArray m_yellowCircle;
	sf::Texture m_floorTexture;
	sf::Font m_font;

	entt::entity m_blackAutoButton{ entt::null };
	entt::entity m_whiteAutoButton{ entt::null };
	entt::entity m_gameResetButton{ entt::null };
	entt::entity m_depthButton{ entt::null };
	entt::entity m_hintButton{ entt::null };

	sf::Text m_blackAutoText{ sf::Text(sf::String("Black Auto"), m_font) };
	sf::Text m_whiteAutoText{ sf::Text(sf::String("White Auto"), m_font) };
	sf::Text m_depthText{ sf::Text(sf::String("Depth: 3"), m_font) };
	sf::Text m_gameResetText{ sf::Text(sf::String("Reset"), m_font) };
	sf::Text m_hintText{ sf::Text(sf::String("Hint"), m_font) };
	sf::Text m_statusText{ sf::Text(sf::String("Status : Idle"), m_font) };

	std::atomic<bool> m_blackAuto{ false }, m_whiteAuto{ false }, m_busy{ false }, m_over{ false };
	std::atomic<unsigned> m_depth{ 3 };

	int xOffset{ 270 }, yOffset{ 20 };
	int xIncrement{ 45 }, yIncrement{ 45 };
	sf::FloatRect m_boardRect{ 270, 20, 670, 670 };

	using Point = typename std::array<int, 2>;
	using PointPair = typename std::pair<Point, Point>;
	KDTree<2, Point, int> m_tree;
	Gomoku m_game;

public:
	MainScene(sf::RenderWindow& window);
	void render() override;
	void update() override;
	bool handleInput(sf::Event event) override;
private:
	void loadAndInit();
	bool disableCriticalButtons();
	void enableCriticalButtons();
	void gameOver(int over);
	void playAI();
	void createHint();
	void destoryHint();
	void reset();
	int putPiece(uint8_t index);
	sf::VertexArray makeCircle(float radius, uint32_t sides, sf::Color color) const;
	sf::VertexArray makeRect(float width, float height, sf::Color color, float textCoord = 0) const;

};

