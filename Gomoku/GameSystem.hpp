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
	GameSystem() 
		: m_config(Config::instance()), m_window(sf::VideoMode(m_config.windowWidth, m_config.widowHeight), m_config.windowName)
	{
		m_window.setFramerateLimit(m_config.frameRate);
	}
	~GameSystem() = default;

	void run() {
		init();
		while (m_window.isOpen()) {
			handleInput();
			update();
			render();
		}
	}
	
private:
	void init() {
		assignScene(std::make_unique<MainScene>(m_window));
	}

	void render() {
		m_window.clear();
		m_scene->render();
		m_window.display();
	}

	void handleInput() {
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window.close();
			// Let the current scene to handle user inputs
			m_scene->handleInput(event);
		}
	}

	void update() {
		m_scene->update();
	}

	void assignScene(std::unique_ptr<BaseScene>&& scene) {
		m_scene = std::move(scene);
	}

};

