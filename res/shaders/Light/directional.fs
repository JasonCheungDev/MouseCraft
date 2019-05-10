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
uniform vec3 u_ViewPosition;
uniform mat4 u_LightSpaceMatrix;    
uniform vec3 u_LightPos;
uniform vec3 u_LightDir;
uniform vec3 u_LightColor;
uniform float u_AmbientIntensity = 1.0;
// Shadows 
uniform float u_ShadowNear = 0.1;
uniform float u_ShadowFar  = 50.0; 
float shadowFadeThreshold = 0.95;
float minbias = 0.0005;  // minimum shadow bias (for planes almost perpendicular to d.light) 
float maxbias = 0.002;   // maximum shadow bias (for planes with a steep slope to d.light)
float bias = 0.002;

// takes in non-normalized (raw) depth coordinates
float LinearizeDepth(float depth)
{
    // Formula: https://learnopengl.com/Advanced-OpenGL/Depth-testing
    // Detailed: https://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * u_ShadowNear * u_ShadowFar) / (u_ShadowFar + u_ShadowNear - z * (u_ShadowFar - u_ShadowNear));	
}

float ShadowCalculation(vec4 lightSpaceFrag)
{
    // perform perspective divide
    vec3 projCoords = lightSpaceFrag.xyz / lightSpaceFrag.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).x; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float bias = 0.002;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // handle steep angles 
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

	// soften shadows if near shadow map limit
	vec3 dist = abs(projCoords - vec3(0.5));
	float fade = max(max(dist.x, dist.y), dist.z) * 2.0;	// times 2 to bring range from 0-0.5 to 0-1.0
	if (fade > shadowFadeThreshold)
	{
		float percent = (fade - shadowFadeThreshold) / (1.0 - shadowFadeThreshold);
		shadow *= (1.0 - percent);
	}

	if (projCoords.z > 1.0)
		shadow = 0.0;   // out of frustrum far plane
		
    return shadow;
}

float ShadowCalcFade(vec4 lightSpaceFrag, float bias, float camDepth)
{
    // perform perspective divide
    vec3 projCoordsRaw = lightSpaceFrag.xyz / lightSpaceFrag.w;
    // transform to [0,1] range
    vec3 projCoords = projCoordsRaw * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).x; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // ----- hard -----
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
    // ----- soft -----
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // ----- fade -----
    // length(projCoordsRaw)    == fade values close to very edge of the shadow map (including out)
    // abs(closest * 2.0 - 1.0) == fade values close to near/far plane (including out)
    float fade = max(length(projCoordsRaw), closestDepth);
    if (fade > shadowFadeThreshold)
	{
		float percent = (fade - shadowFadeThreshold) / (1.0 - shadowFadeThreshold);
		shadow *= (1.0 - clamp(percent, 0, 1));
	}

    return shadow;
}


float ShadowCalculationSoft(vec4 lightSpaceFrag)
{
    // perform perspective divide
    vec3 projCoords = lightSpaceFrag.xyz / lightSpaceFrag.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow + smooth out 
    float bias = 0.005;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;   // out of frustrum far plane

    return min(1.0, max(0.0, shadow));
}


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

    // fake lighting 
    vec3 ambient = u_AmbientIntensity * u_LightColor; 
    
    // measure of how directly light is hitting the surface (eg. how well it scatters)
    float diffuseStrength = -dot(nrm, u_LightDir);    
    vec3 diffuse = u_LightColor * max(0, diffuseStrength);

    // shininess when light bounces directly into our eyes
    vec3 viewDir = normalize(u_ViewPosition - pos); // reversed (to prevent negative sign later)
    vec3 reflectDir = reflect(u_LightDir, nrm);
    float specPower = pow(max(0, dot(viewDir, reflectDir)), 32);
    vec3 specular = specularIntensity * specPower * u_LightColor; 

    // shadow 
    float scaledBias = max(maxbias * (1.0 - dot(nrm, u_LightDir)), minbias);
    float shadow = ShadowCalcFade(u_LightSpaceMatrix * vec4(pos, 1.0), scaledBias, LinearizeDepth(depth));
    // float shadow = ShadowCalculation(u_LightSpaceMatrix * vec4(pos, 1.0));
    //vec3 lighting = (1.0 - shadow) * (ambient + diffuse + specular) * col.rgb;  //* col.rgb;   // not sure what this last * col is for. 

    vec3 lighting = (ambient + diffuse + specular) * col * (1.0 - shadow); 
    o_Col = vec4(lighting, 1.0);
}

// Note: 
// vec3 reflection = u_LightDir - 2 * dot(u_LightDir, nrm) * nrm;