#version 420 core

in vec2 f_Uv;

out vec4 o_Color;

uniform sampler2D u_Texture;
uniform vec4 u_Tint;			// color tint and opacity
// uniform float u_Opacity;

void main()
{
	// color.a = 1.0f; //color.a * u_Opacity;
	o_Color = texture(u_Texture, f_Uv) * u_Tint;
}