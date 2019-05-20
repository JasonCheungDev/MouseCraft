#version 420 core

in vec2 f_Uv;

uniform sampler2D u_FinTex;
uniform sampler2D u_StencilTex;
uniform float u_Strength = 1.0;

out vec4 o_Color;

#define SAMPLE_SIZE 5
float weight[SAMPLE_SIZE] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

// https://learnopengl.com/Advanced-Lighting/Bloom
void main()
{
	vec2 tex_offset = 1.0 / textureSize(u_FinTex, 0);
	vec3 result = texture(u_FinTex, f_Uv).rgb * weight[0];
    float stencil = texture(u_StencilTex, f_Uv).a;

    for(int i = 1; i < SAMPLE_SIZE; ++i)
    {
        result += texture(u_FinTex, f_Uv + vec2(tex_offset.x * i, 0.0) * u_Strength * stencil).rgb * weight[i];
        result += texture(u_FinTex, f_Uv - vec2(tex_offset.x * i, 0.0) * u_Strength * stencil).rgb * weight[i];
    }

	o_Color = vec4(result, 1.0);
}