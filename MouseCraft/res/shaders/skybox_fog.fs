#version 420 core

in vec3 f_TexCoords;

out vec4 FragColor;

uniform samplerCube u_TexCubemap;
uniform float u_FogAngleStart;
uniform float u_FogAngleEnd;
uniform vec3 u_FogColor;

void main()
{  
    vec3 color = texture(u_TexCubemap, f_TexCoords).rgb;
    float angle = dot(normalize(f_TexCoords), vec3(0,-1,0));
    float angleLimit = cos(u_FogAngleStart);
    float angleLimitEnd = cos(u_FogAngleEnd);
    float percent = min(1, max(0, (angle - angleLimit) / (angleLimitEnd - angleLimit)));
    FragColor = vec4(mix(color, u_FogColor, percent), 1.0);
    //FragColor = vec4(color + u_FogColor * percent, 1.0);
}