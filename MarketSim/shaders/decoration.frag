
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

// Directional light
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 ambientColor;

uniform int isLamp;
uniform vec3 emissiveColor;
uniform float emissiveStrength;

// Camera
uniform vec3 viewPosition;

// Material
uniform sampler2D diffuseTexture;
uniform vec3 materialDiffuse;
uniform float materialSpecular;
uniform float materialShininess;
uniform float materialAlpha;

// Shadow mapping
uniform sampler2D shadowMap;

// Point lights
#define MAX_POINT_LIGHTS 100

uniform int pointLightCount;
uniform vec4 pointLightPositionRadius[MAX_POINT_LIGHTS];
uniform vec4 pointLightColorIntensity[MAX_POINT_LIGHTS];


// --------------------------------------------------
// Shadow calculation (PCF 3x3)
// --------------------------------------------------

float CalculateShadow(vec3 normal, vec3 lightDir)
{
    vec3 projCoords = FragPosLightSpace.xyz /
                      FragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;

    // Outside shadow map
    if (projCoords.z > 1.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 0.0;
    }

    float currentDepth = projCoords.z;

    float bias = max(
        0.005 * (1.0 - dot(normal, lightDir)),
        0.0005
    );

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float shadow = 0.0;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(
                shadowMap,
                projCoords.xy + vec2(x, y) * texelSize
            ).r;

            shadow += currentDepth - bias > closestDepth
                      ? 1.0
                      : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;
}


// --------------------------------------------------
// Directional light (Blinn-Phong)
// --------------------------------------------------

vec3 CalculateDirectionalLight(
    vec3 normal,
    vec3 albedo,
    vec3 viewDir
)
{
    vec3 lightDir = normalize(-lightDirection);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(
        max(dot(normal, halfwayDir), 0.0),
        max(materialShininess, 1.0)
    );

    vec3 diffuse = lightColor * diff * albedo;

    vec3 specular = lightColor *
                    spec *
                    materialSpecular;

    return diffuse + specular;
}


// --------------------------------------------------
// Point lights (street lamps)
// --------------------------------------------------

vec3 CalculatePointLights(
    vec3 normal,
    vec3 albedo,
    vec3 viewDir
)
{
    vec3 result = vec3(0.0);

    for (int i = 0; i < pointLightCount; ++i)
    {
        vec3 lightPos = pointLightPositionRadius[i].xyz;

        float radius = pointLightPositionRadius[i].w;

        vec3 color = pointLightColorIntensity[i].rgb;

        float intensity = pointLightColorIntensity[i].w;

        vec3 lightVector = lightPos - FragPos;

        float distanceToLight = length(lightVector);

        if (distanceToLight >= radius)
            continue;

        vec3 lightDir = normalize(lightVector);

        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);

        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float spec = pow(
            max(dot(normal, halfwayDir), 0.0),
            max(materialShininess, 1.0)
        );

        // Distance attenuation
        float attenuation = 1.0 /
            (1.0 +
             0.09 * distanceToLight +
             0.032 * distanceToLight * distanceToLight);

        // Smooth radius fade
        float fade = 1.0 - smoothstep(
            radius * 0.7,
            radius,
            distanceToLight
        );

        result += color *
                  intensity *
                  attenuation *
                  fade *
                  (
                      albedo * diff +
                      materialSpecular * spec                  
                  );
    }

    return result;
}


// --------------------------------------------------
// Main
// --------------------------------------------------

void main()
{
    // Texture
    vec4 texColor = texture(diffuseTexture, TexCoord);

    vec3 albedo = texColor.rgb * materialDiffuse;

    float alpha = texColor.a * materialAlpha;

    // Normal
    vec3 normal = normalize(Normal);

    // Camera direction
    vec3 viewDir = normalize(viewPosition - FragPos);

    // Directional light
    vec3 directionalLighting = CalculateDirectionalLight(
        normal,
        albedo,
        viewDir
    );

    // Shadows affect directional light
    vec3 lightDir = normalize(-lightDirection);

    float shadow = CalculateShadow(normal, lightDir);

    // Point lights
    vec3 pointLighting = CalculatePointLights(
        normal,
        albedo,
        viewDir
    );

    // Ambient
    vec3 ambient = ambientColor * albedo;

    // Final lighting
    vec3 result =
        ambient +
        (1.0 - shadow) * directionalLighting +
        pointLighting;


	if (isLamp == 1) {
    		result += albedo * emissiveColor * emissiveStrength;
	}

    FragColor = vec4(result, alpha);
}
