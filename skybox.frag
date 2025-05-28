#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float brightness;

void main()
{
    vec4 texColor = texture(skybox, TexCoords);
    FragColor = vec4(texColor.rgb * brightness, texColor.a);
}
