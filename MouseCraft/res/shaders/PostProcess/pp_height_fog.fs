#version 420 core

in vec2 f_Uv;

uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_FinTex;
uniform vec3 u_FogColor;
uniform float u_FadeStart;	// height where fog starts 
uniform float u_FadeEnd;	// height where fog ends 

out vec4 o_Color;

void main()
{
	float height = texture(u_PosTex, f_Uv).y;
	vec3 color = texture(u_FinTex, f_Uv).rgb;
	float percent = min(1, max(0, (height - u_FadeStart) / (u_FadeEnd - u_FadeStart)));
	// o_Color = vec4(color + u_FogColor * percent, 1.0);
	o_Color = vec4(mix(color, u_FogColor, percent), 1.0);
}