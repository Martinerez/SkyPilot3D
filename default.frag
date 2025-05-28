#version 330 core

out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

// Uniforms para el modelo
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform float alpha;

// Uniform para el skybox
uniform samplerCube skybox;
uniform bool renderSkybox; // TRUE: dibujar skybox, FALSE: dibujar modelo

// Color base del modelo (doradito del glTF)
const vec4 baseColorFactor = vec4(1.0, 0.6376, 0.1022, 1.0);

vec4 calculateModelLighting()
{
    float ambient = 0.20f;

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.50f;
    vec3 viewDirection = normalize(camPos - crntPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;

    vec4 lightResult = (baseColorFactor * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
    lightResult.a *= alpha; // Aplicar efecto fade-in

    return lightResult;
}

void main()
{
    if (renderSkybox)
    {
        // Usamos la posición como dirección para samplear el cubemap
        FragColor = texture(skybox, crntPos);
    }
    else
    {
        FragColor = calculateModelLighting();
    }
}