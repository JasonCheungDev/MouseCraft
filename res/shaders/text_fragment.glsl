#version 420 core

in vec2 f_TexCoords;
out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec3 u_TextColor;

void main()
{
	o_Color = vec4(u_TextColor, texture(u_Texture, f_TexCoords).r);
}