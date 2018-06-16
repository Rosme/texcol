#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>

const std::string convertPath(const std::string& path) {
	std::string imagePath = path;
	std::replace(imagePath.begin(), imagePath.end(), '\\', '/');

	return imagePath;
}

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
	
	sf::RenderWindow window(sf::VideoMode(size.x, size.y, 32), "TexCol");
	
	sf::Sprite sprite(texture);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				window.close();
			}
		}

		window.clear();
		window.draw(sprite);
		window.display();
	}

	return 0;
}