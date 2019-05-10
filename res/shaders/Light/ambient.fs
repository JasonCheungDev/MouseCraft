#version 420 core 

in vec2 f_Uv; 

// Remember to set texture location via uniforms 
uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_ShadowMap;

out layout(location = 0) vec4 o_Col;

// Lighting 
uniform vec3 u_LightColor;  // should be final accumulated ambient addition

void main() 
{
    // retrieve data from gbuffer 
	vec3 pos = texture(u_PosTex, f_Uv).rgb;            // surface position
	vec3 nrm = normalize(texture(u_NrmTex, f_Uv).rgb); // surface normal
	vec3 col = texture(u_ColTex, f_Uv).rgb;            // surface color
    float depth = texture(u_DphTex, f_Uv).x;           // surface depth from camera
    float specularIntensity = texture(u_ColTex, f_Uv).a; // surface shininess
   
    if (depth >= 1.0)
    {
        discard;
    }

    vec3 lighting = (u_LightColor) * col; 
    o_Col = vec4(lighting, 1.0);
}
