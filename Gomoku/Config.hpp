#pragma once
#include <SFML/Graphics.hpp>

// Singleton config
class Config
{
	Config() {}
	Config(const Config&) = delete;
	void operator=(const Config&) = delete;
	~Config() = default;
public:

	static Config& instance() {
		static Config config;
		return config;
	}
	unsigned frameRate{ 144 };
	int windowWidth{ 1280 };
	int widowHeight{ 720 };
	const float pi{ 3.14159265358979323846f };
	sf::String windowName{ "Gomoku" };
};


