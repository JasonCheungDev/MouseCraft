#version 420 core 

layout (location = 0) in vec3 i_Pos;	// position
layout (location = 1) in vec2 i_Uv;		// texture coordinates 
layout (location = 2) in vec3 i_Nrm;	// normal 
layout (location = 3) in vec3 i_Tan;	// tangent 
layout (location = 4) in vec3 i_Btn;	// bitangent

out vec3 f_Pos; 
out vec2 f_Uv; 
out vec3 f_Nrm; 
out mat3 f_TBN; // tangent to world-space change-of-basis matrix

// world space frame of reference 
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() 
{
    // slightly incorrect as u_Model contains translation and scale
    vec3 T = normalize(vec3(u_Model * vec4(i_Tan,   0.0)));
    vec3 B = normalize(vec3(u_Model * vec4(i_Btn,   0.0)));
    vec3 N = normalize(vec3(u_Model * vec4(i_Nrm,   0.0)));
    f_TBN = mat3(T, B, N);

    f_Pos = (u_Model * vec4(i_Pos, 1.0)).xyz;
    f_Uv  = i_Uv;
    f_Nrm = transpose(inverse(mat3(u_Model))) * i_Nrm;    // decompose matrix to get rotation 
    gl_Position = u_Projection * u_View * u_Model * vec4(i_Pos, 1.0);
}