#version 400

in vec2 texCoords;
out vec4 Colour;
uniform sampler2D tex;

void main(void)
{		
	vec4 texel = texture2D(tex, texCoords);
	//Colour = vec4(1.0, 0.0, 0.0, 1.0);
	Colour = texel;
}