
#version 330 core

#define MAX_POINT_LIGHTS 100

in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D roadTexture;

uniform vec3 ambientColor;

uniform int pointLightCount;

uniform vec4 pointLightPositionRadius[MAX_POINT_LIGHTS];
uniform vec4 pointLightColorIntensity[MAX_POINT_LIGHTS];

void main()
{
    vec3 albedo = texture(roadTexture, TexCoord).rgb;

    vec3 lighting = ambientColor;

    for (int i = 0; i < pointLightCount; i++)
    {
        vec3 lightPosition = pointLightPositionRadius[i].xyz;
        float radius = pointLightPositionRadius[i].w;

        vec3 lightColor = pointLightColorIntensity[i].rgb;
        float intensity = pointLightColorIntensity[i].w;

        float distanceToLight = length(FragPos - lightPosition);

        float attenuation = max(
            1.0 - distanceToLight / radius,
            0.0
        );

        attenuation *= attenuation;

        lighting += lightColor * intensity * attenuation;
    }

    vec3 result = albedo * lighting;

    FragColor = vec4(result, 1.0);
}
