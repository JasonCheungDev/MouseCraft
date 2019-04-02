// Special shader used for animated renderables

#version 330 core

const int MAX_BONES = 64;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in ivec4 bones;
layout(location = 4) in vec4 weights; 

uniform vec3 color;
uniform mat4 transform;
uniform mat4 invTransform;
uniform int textureID;
uniform mat4 boneTransforms[MAX_BONES];

out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragPos;

void main()
{
	mat4 trans = boneTransforms[bones[0]] * weights[0];
	trans += boneTransforms[bones[1]] * weights[1];
	trans += boneTransforms[bones[2]] * weights[2];
	trans += boneTransforms[bones[3]] * weights[3];

    vec4 pos = trans * transform * vec4(position, 1.0);
    fragPos = pos.xyz;
    fragNormal = (invTransform * vec4(normal, 1.0)).xyz;
    fragNormal = fragNormal + bones.xyz + weights.xyz;
	fragTexCoord = texCoord;
	fragTexCoord = fragTexCoord * boneTransforms[0][0][0];
    gl_Position = pos;
}