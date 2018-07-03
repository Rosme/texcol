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

#include "controlwindow.hpp"

ControlWindow::ControlWindow(rsm::MessageDispatcher& dispatcher)
	: m_render(false)
	, m_isNewColorSet(false)
	, m_dispatcher(dispatcher) {
	setImagePickedColor(sf::Color::Black);
}

void ControlWindow::toggleRendering() {
	m_render = !m_render;
}

void ControlWindow::render() {
	if (!m_render) {
		return;
	}

	static float newColor[3]{ 0.f, 0.f, 0.f };

	ImGui::Begin("Color Control", 0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::InputText("New Image Path", m_imagePathBuffer, 256);
	if (ImGui::Button("Load New Image")) {
		m_dispatcher.pushMessage("new_image", m_imagePathBuffer);
	}

	if (ImGui::Button("Save To File")) {
		m_dispatcher.pushMessage("save");
	}

	ImGui::Separator();

	ImGui::Text("Image Picked Color");
	ImGui::Image(m_texture);
	ImGui::Separator();
	ImGui::Text("New Color");
	ImGui::ColorEdit3("Pick New Color", newColor);

	if (ImGui::Button("Apply new color")) {
		m_newColor.r = static_cast<sf::Uint8>(newColor[0] * 255.f);
		m_newColor.g = static_cast<sf::Uint8>(newColor[1] * 255.f);
		m_newColor.b = static_cast<sf::Uint8>(newColor[2] * 255.f);

		m_dispatcher.pushMessage("replacement_color", m_imagePickedColor);
		m_dispatcher.pushMessage("new_color", m_newColor);
	}

	if (ImGui::Button("Reset Colors")) {
		m_dispatcher.pushMessage("reset_colors");
	}

	ImGui::Separator();

	static float backgroundColor[3]{ 0.f, 0.f, 0.f };
	ImGui::ColorEdit3("Pick Background Color", backgroundColor);
	if(ImGui::Button("Change Background Color")) {
		sf::Color newBackgroundColor;
		newBackgroundColor.r = static_cast<sf::Uint8>(backgroundColor[0] * 255.f);
		newBackgroundColor.g = static_cast<sf::Uint8>(backgroundColor[1] * 255.f);
		newBackgroundColor.b = static_cast<sf::Uint8>(backgroundColor[2] * 255.f);

		m_dispatcher.pushMessage("background_color", newBackgroundColor);
	}

	ImGui::End();
}

void ControlWindow::setImagePickedColor(const sf::Color imagePickedColor) {
	m_imagePickedColor = imagePickedColor;
	m_image.create(24, 24, m_imagePickedColor);
	m_texture.loadFromImage(m_image);
}

bool ControlWindow::isNewColorSet() const {
	return m_isNewColorSet;
}

sf::Color ControlWindow::getNewColor() const {
	return m_newColor;
}

sf::Color ControlWindow::getPickedColor() const {
	return m_imagePickedColor;
}

void ControlWindow::newColorSetted() {
	m_isNewColorSet = false;
}