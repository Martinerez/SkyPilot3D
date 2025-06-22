#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

uniform sampler2D specular0; // Brillo especular
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

// Uniform para el efecto fade-in
uniform float alpha;

// Color base del modelo (doradito del glTF)
const vec4 baseColorFactor = vec4(1.0, 0.6376, 0.1022, 1.0);

vec4 direcLight()
{
    float ambient = 0.40f;

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 1.0f;
    vec3 viewDirection = normalize(camPos - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    vec4 lightResult = (baseColorFactor * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;

    // Ajuste de intensidad global
    lightResult *= 1.3;

    // Efecto de luz cálida (opcional)
    lightResult *= vec4(1.0, 0.95, 0.7, 1.0);

    // Aplicamos alpha (fade-in)
    lightResult.a *= alpha;

    return lightResult;
}


void main()
{
    FragColor = direcLight();
}
