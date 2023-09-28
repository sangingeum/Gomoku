#pragma once
#include <SFML/Graphics.hpp>

struct CRenderable {
	sf::VertexArray& vertexArray;
	sf::RenderStates state;
	CRenderable(float x, float y, sf::VertexArray& arr)
		: vertexArray(arr)
	{
		state.transform.translate({ x, y });
	}
	CRenderable(float x, float y, sf::VertexArray& arr, sf::Texture* texture)
		: vertexArray(arr), state(texture)
	{
		state.transform.translate({ x, y });
	}
};

struct CBackgroundRenderable {
	sf::VertexArray& vertexArray;
	sf::RenderStates state;
	CBackgroundRenderable(float x, float y, sf::VertexArray& arr)
		: vertexArray(arr)
	{
		state.transform.translate({ x, y });
	}
	CBackgroundRenderable(float x, float y, sf::VertexArray& arr, sf::Texture* texture)
		: vertexArray(arr), state(texture)
	{
		state.transform.translate({ x, y });
	}
};
