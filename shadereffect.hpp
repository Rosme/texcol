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

#include <SFML/Graphics.hpp>

class ShaderEffect {
public:
	ShaderEffect();

	void apply(const sf::RenderTexture& input, sf::RenderTarget& output);

	void setColors(const sf::Color toReplace, const sf::Color newColor);
	void setColorToReplace(const sf::Color toReplace);
	void setNewColor(const sf::Color newColor);

	void resetColors();

private:
	sf::Color m_colorToReplace;
	sf::Color m_newColor;
	sf::Shader m_shader;
	const std::string fullpassVert = R"(
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
)";
	const std::string brightnessFrag = R"(
uniform sampler2D source;
uniform vec4 toreplace;
uniform vec4 newcolor;

void main()
{
	vec4 color = texture2D(source, gl_TexCoord[0].xy);
	if(color.r == toreplace.r && color.g == toreplace.g && color.b == toreplace.b) {
		gl_FragColor = newcolor;
	} else {
		gl_FragColor = color;
	}
}
)";

};