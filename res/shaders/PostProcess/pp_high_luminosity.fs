// this is the template for post-processing fragment shader. 

#version 420 core

in vec2 f_Uv;

uniform sampler2D u_FinTex;

out vec4 o_Color;

const float threshold = 0.95;

void main()
{
	vec3 color = texture(u_FinTex, f_Uv).rgb;
	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	if (brightness < threshold)
		discard;
	else 
		o_Color = vec4(color, 1.0);
}