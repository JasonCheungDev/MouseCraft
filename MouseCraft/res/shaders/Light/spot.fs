#version 420 core 

in vec2 f_Uv; 

// Remember to set texture location via uniforms 
uniform sampler2D u_PosTex; 
uniform sampler2D u_NrmTex; 
uniform sampler2D u_ColTex; 
uniform sampler2D u_DphTex;
uniform sampler2D u_ShadowMap;

out layout(location = 0) vec4 o_Col;

uniform vec3 u_ViewPosition;

// Lighting 
uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_LightColor;
uniform float u_LightIntensity;
uniform float u_LightRange;
uniform float u_LightAngle;         

float atten_linear = 0.1; 
float atten_quadratic = 0.1; 
float near = 0.1;
float far  = 50.0; 

// Shadows 
uniform float u_ShadowNear = 0.1;
uniform float u_ShadowFar  = 50.0; 
float shadowFadeThreshold = 0.95;
float minbias = 0.002;  // minimum shadow bias (for planes almost perpendicular to d.light) 
float maxbias = 0.002;   // maximum shadow bias (for planes with a steep slope to d.light)

// takes in non-normalized (raw) depth coordinates
float LinearizeDepth(float depth)
{
    // Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
    // Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * u_ShadowNear * u_ShadowFar) / (u_ShadowFar + u_ShadowNear - z * (u_ShadowFar - u_ShadowNear));	
}

void main() 
{
    // retrieve data from gbuffer 
	vec3 pos = texture(u_PosTex, f_Uv).rgb;            // surface position
	vec3 nrm = normalize(texture(u_NrmTex, f_Uv).rgb); // surface normal
	vec3 col = texture(u_ColTex, f_Uv).rgb;            // surface color
    float depth = texture(u_DphTex, f_Uv).x;           // surface depth from camera
   
    if (depth >= 1.0)
    {
        discard;
    }

    float distance = length(u_LightPos - pos);

    if (distance > u_LightRange)
    {
        discard;
    }

    // distance strength 
    float attenutation = 1.0 - min(1.0, max(0.0, distance / u_LightRange));

    vec3 lightToPos = normalize(pos - u_LightPos);

    // angle strength 
    float angleStrength = dot(u_LightDir, lightToPos) * 0.5 + 0.5;
    float angleLimit = cos(u_LightAngle) * 0.5 + 0.5;
    angleStrength = max(0.0, (angleStrength - angleLimit) / (1.0 - angleLimit));

    // diffuse strength
    float diffuseStrength = max(0.0, -dot(nrm, lightToPos));    
    
    vec3 lighting = (u_LightColor * attenutation * angleStrength * diffuseStrength); 
    o_Col = vec4(lighting, 1.0);
}

// Note: 
// vec3 reflection = u_LightDir - 2 * dot(u_LightDir, nrm) * nrm;