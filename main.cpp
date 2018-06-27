#include <string>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <functional>
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
		: m_render(false)
		, m_isNewColorSet(false)
	{}

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

			m_isNewColorSet = true;
		}

		if(ImGui::Button("Save To File")) {
			if(!m_saveFunction()) {
				sf::err() << "Failed to save image" << std::endl;
			} else {
				sf::err() << "Image has been saved" << std::endl;
			}
		}

		ImGui::End();
	}

	void setImagePickedColor(const sf::Color imagePickedColor) {
		m_imagePickedColor = imagePickedColor;
		m_image.create(24, 24, m_imagePickedColor);
		m_texture.loadFromImage(m_image);
	}

	bool isNewColorSet() const {
		return m_isNewColorSet;
	}

	sf::Color getNewColor() const {
		return m_newColor;
	}

	sf::Color getPickedColor() const {
		return m_imagePickedColor;
	}

	void newColorSetted() {
		m_isNewColorSet = false;
	}

	void setSaveFunction(const std::function<bool()>& saveFunction) {
		m_saveFunction = saveFunction;
	}

private:
	bool m_render;
	sf::Color m_imagePickedColor;
	sf::Image m_image;
	sf::Texture m_texture;
	sf::Color m_newColor;
	bool m_isNewColorSet;
	std::function<bool()> m_saveFunction;
};

class TextureColorEffect {

public:
	TextureColorEffect() {
		m_shader.loadFromMemory(fullpassVert, brightnessFrag);
	}

	void apply(const sf::RenderTexture& input, sf::RenderTarget& output) {
		m_shader.setParameter("source", input.getTexture());

		sf::Vector2f outputSize = static_cast<sf::Vector2f>(output.getSize());

		sf::VertexArray vertices(sf::TrianglesStrip, 4);
		vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 1));
		vertices[1] = sf::Vertex(sf::Vector2f(outputSize.x, 0), sf::Vector2f(1, 1));
		vertices[2] = sf::Vertex(sf::Vector2f(0, outputSize.y), sf::Vector2f(0, 0));
		vertices[3] = sf::Vertex(sf::Vector2f(outputSize), sf::Vector2f(1, 0));

		sf::RenderStates states;
		states.shader = &m_shader;
		states.blendMode = sf::BlendNone;

		output.draw(vertices, states);
	}

private:
	sf::Shader m_shader;
	std::string fullpassVert = R"(
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
)";
	std::string brightnessFrag = R"(
uniform sampler2D source;

void main()
{
	gl_FragColor = texture2D(source, gl_TexCoord[0].xy);
}
)";
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
	sf::err() << imagePath << " loaded" << std::endl;
	auto size = texture.getSize();
	sf::Sprite sprite(texture);
	sprite.setPosition(size.x / 2, size.y / 2);

	sf::RenderWindow window(sf::VideoMode(size.x*2, size.y*2, 32), "TexCol");
	ImGui::SFML::Init(window);
	sf::RenderTexture renderTexture;
	renderTexture.create(window.getSize().x, window.getSize().y);

	sf::RenderTexture renderTextureWithShader;
	renderTextureWithShader.create(renderTexture.getSize().x, renderTexture.getSize().y);

	TextureColorEffect textureColorEffect;

	ImguiWindow imguiWindow;

	sf::Clock deltaClock;

	auto saveFunction = [&texture, &imagePath]() {
		const auto index = imagePath.find_last_of('.');

		if (index != std::string::npos) {

			const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			const auto localtime = std::localtime(&now);
			auto sub = imagePath.substr(0, index);
			sub += '-';
			sub += std::to_string(localtime->tm_hour) + "-";
			sub += std::to_string(localtime->tm_min) + "-";
			sub += std::to_string(localtime->tm_sec);
			sub += imagePath.substr(index);

			return texture.copyToImage().saveToFile(sub);
		}
		return false;
	};

	imguiWindow.setSaveFunction(saveFunction);
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
				case sf::Keyboard::S:
					if (!saveFunction()) {
						sf::err() << "Failed to save image" << std::endl;
					}
					else {
						sf::err() << "Image has been saved" << std::endl;
					}
				default:
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

		if(imguiWindow.isNewColorSet()) {
			
			auto textureImage = texture.copyToImage();
			auto newColor = imguiWindow.getNewColor();
			auto pickedColor = imguiWindow.getPickedColor();

			for(auto i = 0u; i < size.x; ++i) {
				for(auto j = 0u; j < size.y; ++j) {
					if(textureImage.getPixel(i, j) == pickedColor) {
						textureImage.setPixel(i, j, newColor);
					}
				}
			}

			if(!texture.loadFromImage(textureImage)) {
				sf::err() << "Couldn't reload texture from image" << std::endl;
			}
			sprite.setTexture(texture);

			imguiWindow.newColorSetted();
		}

		imguiWindow.render();

		renderTexture.clear();
		renderTexture.draw(sprite);
		renderTexture.display();

		renderTextureWithShader.clear();
		textureColorEffect.apply(renderTexture, renderTextureWithShader);
		renderTextureWithShader.display();

		window.clear();
		window.draw(sf::Sprite(renderTextureWithShader.getTexture()));
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
