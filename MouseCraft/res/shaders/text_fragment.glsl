#version 420 core

in vec2 f_TexCoords;
out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec4 u_TextColor;

void main()
{
	o_Color =  u_TextColor * vec4(1,1,1,texture(u_Texture, f_TexCoords).r);
}