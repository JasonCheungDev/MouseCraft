// this is the template for post-processing fragment shader. 

#version 420 core

in vec2 f_Uv;

uniform sampler2D u_FinTex;
uniform sampler2D u_BlmTex;

out vec4 o_Color;

void main()
{
	vec3 color = texture(u_FinTex, f_Uv).rgb;
	vec3 bloomColor = texture(u_BlmTex, f_Uv).rgb;
	
	// tone mapping
	// vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
	// result = pow(result, vec3(1.0 / gamma));

	o_Color = vec4(color + bloomColor, 0);
}