#version 420 core 

in vec3 f_Pos; 
in vec2 f_Uv; 
in vec3 f_Nrm; 
in mat3 f_TBN;

out layout(location = 0) vec3 o_Pos; 
out layout(location = 1) vec3 o_Nrm; 
out layout(location = 2) vec4 o_Col; 

// opengl 4.2 you can bind this texture 
uniform sampler2D u_TexDiffuse;
uniform sampler2D u_TexNormal;
uniform vec3 u_Diffuse;
uniform vec3 u_Specular;
uniform vec3 u_Ambient;
uniform bool u_NoTexture;	// should probably just send in a white texture
uniform float u_SpecularIntensity = 1.0f;

void main() 
{
	// position 
    o_Pos = f_Pos;

	// normals 
	if (textureSize(u_TexNormal, 0) == vec2(1,1))
	{
		// no texture is bound 
		o_Nrm = f_Nrm;
	}
	else 
	{
		// normal map is bound
		vec3 normal = texture(u_TexNormal, f_Uv).xyz;
		normal = normalize(normal * 2.0 - 1.0); // convert 0...1 to -1...1
		normal = normalize(f_TBN * normal);
		o_Nrm = normal;
	}
	
	// albedo 
	vec3 color = texture(u_TexDiffuse, f_Uv).rgb * u_Diffuse;
	o_Col = vec4(color, u_SpecularIntensity);
}