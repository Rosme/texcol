#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

std::ostream& operator<<(std::ostream& out, const sf::Color& color) {
	out << "R: " << static_cast<int>(color.r) << " G: " << static_cast<int>(color.g) << " B: " << static_cast<int>(color.b);
	return out;
}

std::string convertPath(const std::string& path) {
	std::string imagePath = path;
	std::replace(imagePath.begin(), imagePath.end(), '\\', '/');

	return imagePath;
}

class ImguiWindow {
public:
	ImguiWindow() 
		: m_render(false) {}

	void toggleRendering() {
		m_render = !m_render;
	}

	void render() {
		if (!m_render) {
			return;
		}

		static float color[3] { 0.f, 0.f, 0.f };

		ImGui::Begin("Color Control", 0, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("Image Picked Color");
		ImGui::Image(m_texture);
		ImGui::Separator();
		ImGui::Text("New Color");
		ImGui::ColorEdit3("Pick New Color", color);

		if(ImGui::Button("Apply new color")) {
			m_newColor.r = static_cast<sf::Uint8>(color[0] * 255.f);
			m_newColor.g = static_cast<sf::Uint8>(color[1] * 255.f);
			m_newColor.b = static_cast<sf::Uint8>(color[2] * 255.f);

			sf::err() << "Applying new color: " << m_newColor << std::endl;
		}

		ImGui::End();
	}

	void setImagePickedColor(const sf::Color imagePickedColor) {
		m_imagePickedColor = imagePickedColor;
		m_image.create(24, 24, m_imagePickedColor);
		m_texture.loadFromImage(m_image);
	}

private:
	bool m_render;
	sf::Color m_imagePickedColor;
	sf::Image m_image;
	sf::Texture m_texture;
	sf::Color m_newColor;
};

int main(int argc, char* argv[]) {
	if (argc == 1) {
		sf::err() << "Require image path passed as argument" << std::endl;
		return -1;
	}

	const std::string imagePath = convertPath(argv[1]);

	sf::Texture texture;
	if (!texture.loadFromFile(imagePath)) {
		sf::err() << "Couldn't load image" << std::endl;
		return -2;
	}
	auto size = texture.getSize();
	sf::Sprite sprite(texture);
	sprite.setPosition(size.x / 2, size.y / 2);

	sf::RenderWindow window(sf::VideoMode(size.x*2, size.y*2, 32), "TexCol");
	ImGui::SFML::Init(window);
	
	ImguiWindow imguiWindow;
	
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

			if (event.type == sf::Event::MouseButtonReleased) {
				const sf::Vector2u pos(event.mouseButton.x, event.mouseButton.y);
				if(pos.x >= size.x/2 && pos.x <= size.x + size.x/2
					&& pos.y >= size.y/2 && pos.y <= size.y + size.y/2
					&& !ImGui::IsAnyWindowHovered()
					&& !ImGui::IsAnyItemActive()) {

					imguiWindow.setImagePickedColor(texture.copyToImage().getPixel(event.mouseButton.x - (size.x/2), event.mouseButton.y - (size.y/2)));
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