
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 viewPosition;

uniform vec3 materialDiffuse;
uniform float materialSpecular;
uniform float materialShininess;
uniform float materialAlpha;

float CalculateShadow(vec4 lightSpacePosition, vec3 normal)
{
    vec3 projCoords = lightSpacePosition.xyz /
                      lightSpacePosition.w;

    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float bias = max(
        0.0005 * (1.0 - dot(normal, -lightDirection)),
        0.00005
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

            shadow += projCoords.z - bias > closestDepth
                      ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-lightDirection);
    vec3 viewDir = normalize(viewPosition - FragPos);

    vec3 texColor = texture(diffuseTexture, TexCoord).rgb;
    vec3 albedo = texColor * materialDiffuse;

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(
        max(dot(normal, halfwayDir), 0.0),
        max(materialShininess, 1.0)
    );

    float shadow = CalculateShadow(FragPosLightSpace, normal);

    vec3 ambient = ambientColor * albedo;

    vec3 diffuse = lightColor * diff * albedo;

    vec3 specular = lightColor * spec * materialSpecular;

    vec3 result = ambient +
                  (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(result, materialAlpha);
}
