#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

const std::string convertPath(const std::string& path) {
	std::string imagePath = path;
	std::replace(imagePath.begin(), imagePath.end(), '\\', '/');

	return imagePath;
}

class ImGuiWindow {
public:
	ImGuiWindow() 
		: m_render(false)
	{}

	void toggleRendering() {
		m_render = !m_render;
	}

	void render() {
		if (!m_render) {
			return;
		}

		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button");
		ImGui::End();

	}

private:
	bool m_render;
};

int main(int argc, char* argv[]) {
	if (argc == 1) {
		return -1;
	}

	const std::string imagePath = convertPath(argv[1]);

	sf::Texture texture;
	if (!texture.loadFromFile(imagePath)) {
		return -2;
	}
	auto size = texture.getSize();
	sf::Sprite sprite(texture);

	sf::RenderWindow window(sf::VideoMode(size.x, size.y, 32), "TexCol");
	ImGui::SFML::Init(window);
	
	ImGuiWindow imguiWindow;
	
	sf::Clock deltaClock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (event.type == sf::Event::KeyReleased) {
				switch (event.key.code) {
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::F1:
					imguiWindow.toggleRendering();
					break;
				}
			}
		}
		ImGui::SFML::Update(window, deltaClock.restart());

		imguiWindow.render();

		window.clear();
		window.draw(sprite);
		ImGui::SFML::Render(window);
		window.display();
	}


	ImGui::SFML::Shutdown();

	return 0;
}