//Specifies the GLSL version (OpenGL Shading Language)
#version 330 core  

//3D texture coordinates used to sample from the cubemap (passed from the vertex shader)
in vec3 TexCoords;  

// The final color output that this fragment shader writes to the framebuffer

out vec4 FragColor;  

//Uniform representing the cubemap texture
uniform samplerCube skybox;  

void main()
{
    //Samples the cubemap using the direction vector 'TexCoords'
    //This returns the color of the skybox in that direction
    FragColor = texture(skybox, TexCoords);
}
