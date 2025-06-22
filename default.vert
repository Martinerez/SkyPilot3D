#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

// Outputs to Fragment Shader
out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

// Uniform matrices
uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
    // Matriz completa de transformación
    mat4 transform = model * translation * rotation * scale;

    // Calculamos la posición final
    vec4 worldPosition = transform * vec4(aPos, 1.0f);
    crntPos = vec3(worldPosition);

    // Transformamos la normal correctamente
    mat3 normalMatrix = transpose(inverse(mat3(transform)));
    Normal = normalize(normalMatrix * aNormal);

    // Pasamos color y textura tal cual
    color = aColor;

    // Rotamos las coordenadas de textura 90 grados
    texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

    // Proyectamos en el espacio de la cámara
    gl_Position = camMatrix * worldPosition;
}
