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

#include <string>

#include <SFML/Graphics.hpp>
#include <rsm/msg/message_handler.hpp>
#include <rsm/msg/message_dispatcher.hpp>

#include "shadereffect.hpp"
#include "controlwindow.hpp"

class TexCol 
	: public rsm::MessageHandler {
public:
	TexCol(const std::string& imagePath);

	bool isRunning() const;
	void update();
	void render();

private:
	void onMessage(const std::string& key, const rsm::Message& message) override;
	void reload();

private:
	std::string m_imagePath;
	sf::RenderWindow m_window;
	sf::Texture m_texture;
	sf::Clock m_imguiClock;
	sf::RenderTexture m_renderTexturePreShader;
	sf::RenderTexture m_renderTexturePostShader;
	rsm::MessageDispatcher m_dispatcher;
	ShaderEffect m_shader;
	ControlWindow m_controlWindow;
};

inline std::string convertPath(const std::string& path) {
	std::string imagePath = path;
	std::replace(imagePath.begin(), imagePath.end(), '\\', '/');

	return imagePath;
}
