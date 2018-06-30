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

#pragma once

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <rsm/msg/message_dispatcher.hpp>

class ControlWindow {
public:
	ControlWindow(rsm::MessageDispatcher& dispatcher);

	void toggleRendering();

	void render();

	void setImagePickedColor(const sf::Color imagePickedColor);

	bool isNewColorSet() const;

	sf::Color getNewColor() const;

	sf::Color getPickedColor() const;

	void newColorSetted();

private:
	bool m_render;
	sf::Color m_imagePickedColor;
	sf::Image m_image;
	sf::Texture m_texture;
	sf::Color m_newColor;
	bool m_isNewColorSet;
	//TextureColorEffect& m_textureColorEffect;
	rsm::MessageDispatcher& m_dispatcher;
	char m_imagePathBuffer[256]= "";
};