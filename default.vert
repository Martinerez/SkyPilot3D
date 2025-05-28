#version 330 core

// Vertex position attribute (location = 0)
layout (location = 0) in vec3 aPos;
// Vertex normal attribute (location = 1), not guaranteed to be normalized
layout (location = 1) in vec3 aNormal;
// Vertex color attribute (location = 2)
layout (location = 2) in vec3 aColor;
// Vertex texture coordinate attribute (location = 3)
layout (location = 3) in vec2 aTex;

// Passes the transformed vertex position to the Fragment Shader
out vec3 crntPos;
// Passes the normal vector to the Fragment Shader
out vec3 Normal;
// Passes the vertex color to the Fragment Shader
out vec3 color;
// Passes the texture coordinates to the Fragment Shader
out vec2 texCoord;

// Uniform matrix for camera view-projection transformation
uniform mat4 camMatrix;
// Uniform model transformation matrices
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	// Compute the world-space position of the vertex after applying model transformations
	crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));

	// Pass vertex normal directly to the Fragment Shader
	Normal = aNormal;

	// Pass vertex color directly to the Fragment Shader
	color = aColor;

	// Apply a 90-degree rotation to the texture coordinates and pass to the Fragment Shader
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

	// Calculate final position in clip space by multiplying the transformed vertex with the camera matrix
	gl_Position = camMatrix * vec4(crntPos, 1.0);
}
