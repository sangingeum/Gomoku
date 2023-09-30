#include "GameSystem.hpp"
GameSystem::GameSystem()
	: m_config(Config::instance()), m_window(sf::VideoMode(m_config.windowWidth, m_config.widowHeight), m_config.windowName)
{
	m_window.setFramerateLimit(m_config.frameRate);
}

void GameSystem::run() {
	init();
	while (m_window.isOpen()) {
		handleInput();
		update();
		render();
	}
}

void GameSystem::init() {
	assignScene(std::make_unique<MainScene>(m_window));
}

void GameSystem::render() {
	m_window.clear();
	m_scene->render();
	m_window.display();
}

void GameSystem::handleInput() {
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window.close();
		// Let the current scene to handle user inputs
		m_scene->handleInput(event);
	}
}

void GameSystem::update() {
	m_scene->update();
}

void GameSystem::assignScene(std::unique_ptr<BaseScene>&& scene) {
	m_scene = std::move(scene);
}