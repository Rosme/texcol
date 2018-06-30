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

#include "shadereffect.hpp"

ShaderEffect::ShaderEffect() {
	m_shader.loadFromMemory(fullpassVert, brightnessFrag);
}

void ShaderEffect::apply(const sf::RenderTexture& input, sf::RenderTarget& output) {
	m_shader.setUniform("source", input.getTexture());
	m_shader.setUniform("toreplace", sf::Glsl::Vec4(m_colorToReplace));
	m_shader.setUniform("newcolor", sf::Glsl::Vec4(m_newColor));

	const sf::Vector2f outputSize = static_cast<sf::Vector2f>(output.getSize());

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

void ShaderEffect::setColors(const sf::Color toReplace, const sf::Color newColor) {
	m_colorToReplace = toReplace;
	m_newColor = newColor;
}

void ShaderEffect::setColorToReplace(const sf::Color toReplace) {
	m_colorToReplace = toReplace;
}

void ShaderEffect::setNewColor(const sf::Color newColor) {
	m_newColor = newColor;
}

void ShaderEffect::resetColors() {
	m_colorToReplace = sf::Color();
	m_newColor = sf::Color();
}
