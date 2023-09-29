#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

struct CRenderable {
	std::reference_wrapper<sf::VertexArray> vertexArray;
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
	sf::FloatRect getBound() const {
		return state.transform.transformRect(vertexArray.get().getBounds());
	}
};

struct CBackgroundRenderable {
	std::reference_wrapper<sf::VertexArray> vertexArray;
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

struct CListener {
	std::function<void()> mainListener;
	std::function<void()> subListener;
	bool disabled{ false };
	bool useToggle{ false };
	bool toggle{ true }; // true->main, false->sub
	CListener(std::function<void()> listener_)
		: mainListener(listener_)
	{}
	CListener(std::function<void()> mainListener_, std::function<void()> subListener_)
		: mainListener(mainListener_), subListener(subListener_), useToggle(true)
	{}
	void listen() {
		if (!disabled) {
			if (useToggle) {
				if (toggle) {
					mainListener();
					toggle = false;
				}
				else {
					subListener();
					toggle = true;
				}
			}
			else {
				mainListener();
			}
		}
	}
};