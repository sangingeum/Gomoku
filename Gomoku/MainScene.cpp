#include "MainScene.hpp"

MainScene::MainScene(sf::RenderWindow& window)
	: BaseScene(window)
{
	loadAndInit();
	reset();
}

void MainScene::render() {
	m_registry.view<CBackgroundRenderable>().each([this](const CBackgroundRenderable& cRenderable) {
		m_window.draw(cRenderable.vertexArray.get(), cRenderable.state);
		});
	m_registry.view<CRenderable>().each([this](const CRenderable& cRenderable) {
		m_window.draw(cRenderable.vertexArray.get(), cRenderable.state);
		});
	m_registry.view<CText>().each([this](const CText& cRenderable) {
		m_window.draw(cRenderable.text.get(), cRenderable.state);
		});
}


void MainScene::update() {
	if ((m_game.getTurn() && m_whiteAuto) || (!m_game.getTurn() && m_blackAuto)) {
		if (disableCriticalButtons()) {
			m_statusText.setString("Status: AI Playing");
			std::thread{ &MainScene::playAI, this }.detach();
		}
	}
}

bool MainScene::handleInput(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed) {
		auto buttonPos = event.mouseButton;
		sf::Vector2f buttonVec{ float(buttonPos.x), float(buttonPos.y) };
		if (!m_busy && m_boardRect.contains(buttonVec)) {
			if (((m_game.getTurn() && !m_whiteAuto) || (!m_game.getTurn() && !m_blackAuto))) {
				auto found = m_tree.findNearestNeighbor({ buttonPos.x, buttonPos.y });
				auto over = putPiece(found.second[0] + found.second[1] * SQRT_PIECE_NUM);
				if (over) {
					gameOver(over);
				}
			}
		}
		else {
			m_registry.view<CListener, CRenderable>().each([buttonVec](CListener& cListener, const CRenderable& cRenderable) {
				if (cRenderable.getBound().contains(buttonVec)) {
					cListener.listen();
				}
				});
		}
		return true;
	}
	return false;
}

void MainScene::loadAndInit() {
	// variables
	m_depth = 3;
	m_depthText.setString("Depth: 3");

	// Asset
	if (!m_floorTexture.loadFromFile("resources/texture/dark_wood_diff_2k.jpg"))
		exit(-1);
	if (!m_font.loadFromFile("resources/font/Roboto-Medium.ttf"))
		exit(-1);
	m_floorTexture.setSmooth(true);

	//m_registry.storage<entt::entity>().reserve(500);
	// Button
	m_buttonRect = makeRect(150, 50, sf::Color::White);
	m_disabledButtonRect = makeRect(150, 50, sf::Color(150, 150, 150));
	m_enabledButtonRect = makeRect(150, 50, sf::Color::Green);
	m_blackAutoButton = m_registry.create();
	m_registry.emplace<CRenderable>(m_blackAutoButton, 50, 100, m_buttonRect);
	m_registry.emplace<CListener>(m_blackAutoButton,
		[this](void) {m_blackAuto = true; m_registry.get<CRenderable>(m_blackAutoButton).vertexArray = m_enabledButtonRect; },
		[this](void) {m_blackAuto = false; m_registry.get<CRenderable>(m_blackAutoButton).vertexArray = m_buttonRect; });

	m_whiteAutoButton = m_registry.create();
	m_registry.emplace<CRenderable>(m_whiteAutoButton, 50, 200, m_buttonRect);
	m_registry.emplace<CListener>(m_whiteAutoButton,
		[this](void) {m_whiteAuto = true; m_registry.get<CRenderable>(m_whiteAutoButton).vertexArray = m_enabledButtonRect; },
		[this](void) {m_whiteAuto = false; m_registry.get<CRenderable>(m_whiteAutoButton).vertexArray = m_buttonRect; });

	m_gameResetButton = m_registry.create();
	m_registry.emplace<CRenderable>(m_gameResetButton, 50, 300, m_buttonRect);
	m_registry.emplace<CListener>(m_gameResetButton, [this](void) {	reset(); });

	m_depthButton = m_registry.create();
	m_registry.emplace<CRenderable>(m_depthButton, 50, 400, m_buttonRect);
	m_registry.emplace<CListener>(m_depthButton,
		[this](void) {
			m_depth = m_depth >= 4 ? 1 : m_depth + 1;
			m_depthText.setString("Depth: " + std::to_string(m_depth));
		});

	m_hintButton = m_registry.create();
	m_registry.emplace<CRenderable>(m_hintButton, 50, 500, m_buttonRect);
	m_registry.emplace<CListener>(m_hintButton, [this](void) {
		if (disableCriticalButtons()) {
			m_statusText.setString("Status: Hint");
			std::thread{ &MainScene::createHint, this }.detach();
		}
		});

	// Labels
	m_blackAutoText.setFillColor(sf::Color::Black);
	m_whiteAutoText.setFillColor(sf::Color::Black);
	m_gameResetText.setFillColor(sf::Color::Black);
	m_depthText.setFillColor(sf::Color::Black);
	m_hintText.setFillColor(sf::Color::Black);
	m_statusText.setFillColor(sf::Color::White);
	m_registry.emplace<CText>(m_registry.create(), 53, 105, m_blackAutoText);
	m_registry.emplace<CText>(m_registry.create(), 53, 205, m_whiteAutoText);
	m_registry.emplace<CText>(m_registry.create(), 73, 305, m_gameResetText);
	m_registry.emplace<CText>(m_registry.create(), 65, 405, m_depthText);
	m_registry.emplace<CText>(m_registry.create(), 80, 505, m_hintText);
	m_registry.emplace<CText>(m_registry.create(), 23, 605, m_statusText);

	// Board
	m_flootRect = makeRect(40, 40, sf::Color::White, 2048);
	m_whiteCircle = makeCircle(20, 30, sf::Color::White);
	m_blackCircle = makeCircle(20, 30, sf::Color::Black);
	m_yellowCircle = makeCircle(20, 30, sf::Color::Yellow);
	for (int i = 0; i < 15; ++i) {
		for (int j = 0; j < 15; ++j) {
			auto entity = m_registry.create();
			m_registry.emplace<CBackgroundRenderable>(entity, xOffset + i * xIncrement, yOffset + j * yIncrement, m_flootRect, &m_floorTexture);
		}
	}
	std::vector<PointPair> treeData;
	treeData.reserve(196); // 14*14
	for (int i = 1; i <= SQRT_PIECE_NUM; ++i) {
		for (int j = 1; j <= SQRT_PIECE_NUM; ++j) {
			treeData.push_back(std::make_pair(Point({ xOffset + i * xIncrement, yOffset + j * yIncrement }), Point({ i - 1, j - 1 })));
		}
	}
	m_tree.buildTree(treeData);

	reset();
}

bool MainScene::disableCriticalButtons() {
	if (m_busy)
		return false;
	m_busy = true;
	for (entt::entity button : {m_gameResetButton, m_hintButton}) {
		auto [cListener, cRenderable] = m_registry.get<CListener, CRenderable>(button);
		cListener.disabled = true;
		cRenderable.vertexArray = m_disabledButtonRect;
	}
	return true;
}

void MainScene::enableCriticalButtons() {
	for (entt::entity button : {m_gameResetButton, m_hintButton}) {
		auto [cListener, cRenderable] = m_registry.get<CListener, CRenderable>(button);
		cListener.disabled = false;
		cRenderable.vertexArray = m_buttonRect;
	}
	m_statusText.setString("Status: Idle");
	m_busy = false;
}

// Diable buttons and set game over flag
void MainScene::gameOver(int over) {
	if (over == 1) {
		m_statusText.setString("Status: White Won");
	}
	else if (over == -1) {
		m_statusText.setString("Status: Black Won");
	}
	else {
		return;
	}
	// disable buttons
	for (entt::entity button : {m_hintButton, m_blackAutoButton, m_whiteAutoButton}) {
		auto [cListener, cRenderable] = m_registry.get<CListener, CRenderable>(button);
		cListener.disabled = true;
		cRenderable.vertexArray = m_disabledButtonRect;
	}
	m_busy = true;
	m_over = true;
}


// Thread function // disableCriticalButtons should be called before this
void MainScene::playAI() {
	auto over = putPiece(m_game.minimax(m_depth));
	enableCriticalButtons();
	if (over) {
		gameOver(over);
	}
}

// Thread function // disableCriticalButtons should be called before this
void MainScene::createHint() {
	auto view = m_registry.view<CRenderable, CToRemove>();
	if (!m_registry.valid(view.front())) {
		auto entity = m_registry.create();
		uint8_t index = m_game.minimax(m_depth);
		int x = xOffset + (index % SQRT_PIECE_NUM + 1) * xIncrement;
		int y = yOffset + (index / SQRT_PIECE_NUM + 1) * yIncrement;
		m_registry.emplace<CRenderable>(entity, x, y, m_yellowCircle);
		m_registry.emplace<CToRemove>(entity, 0);
	}
	enableCriticalButtons();
}

// Try destroying a hint if any
void MainScene::destoryHint() {
	auto view = m_registry.view<CRenderable, CToRemove>();
	if (m_registry.valid(view.front())) {
		m_registry.destroy(view.front());
	}
}

// Reset the game
void MainScene::reset() {
	if (!m_busy || m_over) {
		m_game.reset();
		auto view = m_registry.view<CRenderable>(entt::exclude<CListener>);
		m_registry.destroy(view.begin(), view.end());
		for (entt::entity button : {m_gameResetButton, m_hintButton, m_blackAutoButton, m_whiteAutoButton, m_depthButton}) {
			auto [cListener, cRenderable] = m_registry.get<CListener, CRenderable>(button);
			cListener.disabled = false;
			cListener.toggle = true;
			cRenderable.vertexArray = m_buttonRect;
		}
		m_statusText.setString("Status: Idle");
		m_blackAuto = false;
		m_whiteAuto = false;
		m_busy = false;
		m_over = false;
	}
}

// Place a piece at 'index' position
int MainScene::putPiece(uint8_t index) {
	destoryHint();
	auto put = m_game.putPiece(index);
	if (put) {
		int x = xOffset + (index % SQRT_PIECE_NUM + 1) * xIncrement;
		int y = yOffset + (index / SQRT_PIECE_NUM + 1) * yIncrement;
		auto entity = m_registry.create();
		if (m_game.getTurn())
			m_registry.emplace<CRenderable>(entity, x, y, m_blackCircle);
		else
			m_registry.emplace<CRenderable>(entity, x, y, m_whiteCircle);
	}
	int over = m_game.isOver(index);
	return over;
}


// Make a vertex array that represents a circle 
sf::VertexArray MainScene::makeCircle(float radius, uint32_t sides, sf::Color color) const {
	sf::VertexArray circle{ sf::PrimitiveType::TriangleFan, sides + 2 };
	auto pi = Config::instance().pi;
	circle[0].position = { 0, 0 };
	circle[0].color = color;
	auto step = (2 * pi) / sides;
	for (uint32_t i = 1; i <= sides + 1; ++i) {
		circle[i].position = { radius * cos(step * i), radius * sin(step * i) };
		circle[i].color = color;
	}
	return circle;
}

// Make a vertex array that represents a rectangle 
sf::VertexArray MainScene::makeRect(float width, float height, sf::Color color, float textCoord ) const {
	sf::VertexArray arr{ sf::PrimitiveType::Quads, 4 };
	arr[0].position = { 0, 0 };
	arr[1].position = { width, 0 };
	arr[2].position = { width, height };
	arr[3].position = { 0, height };
	arr[0].color = color;
	arr[1].color = color;
	arr[2].color = color;
	arr[3].color = color;

	if (textCoord != 0) {
		arr[0].texCoords = { 0, 0 };
		arr[1].texCoords = { textCoord, 0 };
		arr[2].texCoords = { textCoord, textCoord };
		arr[3].texCoords = { 0, textCoord };
	}
	return arr;
}