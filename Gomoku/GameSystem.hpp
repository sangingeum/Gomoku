#pragma once
#include "entt.hpp"
#include "Component.hpp"
#include "MainScene.hpp"
#include "Config.hpp"
#include <vector>
#include <memory>

class GameSystem
{
private:
	Config& m_config;
	sf::RenderWindow m_window;
	std::unique_ptr<BaseScene> m_scene;
public:
	GameSystem();
	~GameSystem() = default;
	void run();
private:
	void init();
	void render();
	void handleInput();
	void update();
	void assignScene(std::unique_ptr<BaseScene>&& scene);
};

