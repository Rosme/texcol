/*
* Copyright (c) 2018 Jean-Sébastien Fauteux
*
* This software is provided 'as-is', without any express or implied warranty.
* In no event will the authors be held liable for any damages arising from
* the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it freely,
* subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not claim
*    that you wrote the original software. If you use this software in a product,
*    an acknowledgment in the product documentation would be appreciated but is
*    not required.
*
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "texcol.hpp"

#include <sstream>
#include <chrono>
#include <ctime>

#include <imgui.h>
#include <imgui-SFML.h>
#include <rsm/log/logger.hpp>

TexCol::TexCol(const std::string& imagePath)
	: m_imagePath(convertPath(imagePath))
	, m_controlWindow(m_dispatcher)
{
	m_dispatcher.registerHandler("close", *this);
	m_dispatcher.registerHandler("save", *this);
	m_dispatcher.registerHandler("reset_colors", *this);
	m_dispatcher.registerHandler("toggle_control_window", *this);
	m_dispatcher.registerHandler("new_color", *this);
	m_dispatcher.registerHandler("replacement_color", *this);
	m_dispatcher.registerHandler("new_image", *this);

	reload();
}

bool TexCol::isRunning() const
{
	return m_window.isOpen();
}

void TexCol::update()
{
	sf::Event event;
	while(m_window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed) {
			m_dispatcher.pushMessage("close");
		}

		if (event.type == sf::Event::KeyReleased) {
			switch (event.key.code) {
			case sf::Keyboard::Escape:
				m_dispatcher.pushMessage("close");
				break;
			case sf::Keyboard::F1:
				m_dispatcher.pushMessage("toggle_control_window");
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased) {
			const sf::Vector2u pos(event.mouseButton.x, event.mouseButton.y);
			const auto size = m_texture.getSize();
			if (pos.x >= size.x / 2 && pos.x <= size.x + size.x / 2
				&& pos.y >= size.y / 2 && pos.y <= size.y + size.y / 2
				&& !ImGui::IsAnyWindowHovered()
				&& !ImGui::IsAnyItemActive()) {

				m_controlWindow.setImagePickedColor(m_texture.copyToImage().getPixel(event.mouseButton.x - (size.x / 2), event.mouseButton.y - (size.y / 2)));
			}
		}
	}

	ImGui::SFML::Update(m_window, m_imguiClock.restart());

	m_dispatcher.dispatch();
}

void TexCol::render()
{
	m_controlWindow.render();

	const auto size = m_texture.getSize();
	sf::Sprite sprite(m_texture);
	sprite.setPosition(size.x / 2.f, size.y / 2.f);
	m_renderTexturePreShader.clear();
	m_renderTexturePreShader.draw(sprite);
	m_renderTexturePreShader.display();

	m_renderTexturePostShader.clear();
	m_shader.apply(m_renderTexturePreShader, m_renderTexturePostShader);
	m_renderTexturePostShader.display();

	m_window.clear();
	m_window.draw(sf::Sprite(m_renderTexturePostShader.getTexture()));
	ImGui::SFML::Render(m_window);
	m_window.display();
}

void TexCol::onMessage(const std::string& key, const rsm::Message& message)
{
	if(key == "close") {
		m_window.close();
	}

	if(key == "save") {
		const auto index = m_imagePath.find_last_of('.');

		bool savedSuccesfully = false;
		if (index != std::string::npos) {

			const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			const auto localtime = std::localtime(&now);
			auto sub = m_imagePath.substr(0, index);
			sub += '-';
			sub += std::to_string(localtime->tm_hour) + "-";
			sub += std::to_string(localtime->tm_min) + "-";
			sub += std::to_string(localtime->tm_sec);
			sub += m_imagePath.substr(index);

			savedSuccesfully = m_renderTexturePostShader.getTexture().copyToImage().saveToFile(sub);
		}

		if(savedSuccesfully) {
			rsm::Logger::info("Image has been saved");
		} else {
			rsm::Logger::error("Failed to save image");
		}
	}

	if(key == "reset_colors") {
		m_shader.resetColors();
	}

	if(key == "toggle_control_window") {
		m_controlWindow.toggleRendering();
	}

	if(key == "replacement_color") {
		m_shader.setColorToReplace(message.getContent().get<sf::Color>());
	}

	if(key == "new_color") {
		m_shader.setNewColor(message.getContent().get<sf::Color>());
	}

	if(key == "new_image") {
		m_imagePath = convertPath(message.getContent().get<char*>());
		reload();
	}
}

void TexCol::reload() {
	if (!m_imagePath.empty() && !m_texture.loadFromFile(m_imagePath)) {
		throw std::runtime_error("Couldn't load image " + m_imagePath);
	}

	std::ostringstream sstr;
	sstr << m_imagePath << " loaded";
	rsm::Logger::debug(sstr.str());

	const auto size = m_texture.getSize();

	if(!m_window.isOpen()) {
		if(m_imagePath.empty()) {
			m_window.create(sf::VideoMode(500, 500, 32), "TexCol");
			m_dispatcher.pushMessage("toggle_control_window");
		} else {
			m_window.create(sf::VideoMode(size.x * 2, size.y * 2, 32), "TexCol");
		}

		ImGui::SFML::Init(m_window);
	} else {
		m_window.create(sf::VideoMode(size.x * 2, size.y * 2, 32), "TexCol");
	}

	m_renderTexturePreShader.create(m_window.getSize().x, m_window.getSize().y);
	m_renderTexturePostShader.create(m_renderTexturePreShader.getSize().x, m_renderTexturePreShader.getSize().y);
}
