#version 330 core

// Output color in RGBA format
out vec4 FragColor;

// Interpolated fragment position from the Vertex Shader
in vec3 crntPos;
// Interpolated normal vector from the Vertex Shader
in vec3 Normal;
// Interpolated color from the Vertex Shader
in vec3 color;
// Interpolated texture coordinates from the Vertex Shader
in vec2 texCoord;

// Texture samplers for diffuse and specular maps
uniform sampler2D diffuse0;
uniform sampler2D specular0;
// Light color provided by the application
uniform vec4 lightColor;
// Position of the light source
uniform vec3 lightPos;
// Position of the camera/viewer
uniform vec3 camPos;

vec4 pointLight()
{
	// Vector from the fragment to the light source
	vec3 lightVec = lightPos - crntPos;

	// Light attenuation based on distance
	float dist = length(lightVec);
	float a = 3.0;
	float b = 0.7;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// Ambient lighting component
	float ambient = 0.20f;

	// Diffuse lighting using Lambert's cosine law
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// Specular lighting using Phong reflection model
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// Combine ambient, diffuse, and specular lighting with textures
	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

vec4 direcLight()
{
	// Constant ambient lighting
	float ambient = 0.50f;

	// Diffuse lighting with a fixed light direction
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, -2.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// Specular lighting computation
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// Final color using textures and light contribution
	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
	// Outer and inner cone angles for spotlight intensity
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// Ambient lighting component
	float ambient = 0.20f;

	// Diffuse lighting based on spotlight direction
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// Specular lighting component
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// Spotlight intensity based on angle from spotlight center
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	// Combine all lighting components with texture sampling
	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}

void main()
{
	// Final fragment color using directional light
	FragColor = direcLight();
}
