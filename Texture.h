#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "shaderClass.h"

//Class to handle texture creation, binding, and management
class Texture
{
public:
	GLuint ID;           //ID reference for the texture object
	const char* type;
	GLuint unit;

	//Constructor: loads an image and generates a texture from it
	Texture(const char* image, const char* texType, GLuint slot);

	//Assigns a texture unit to a shader uniform
	void texUnit(Shader& shader, const char* uniform, GLuint unit);

	//Binds the texture so it can be used in rendering
	void Bind();

	//Unbinds the texture (optional cleanup step)
	void Unbind();

	//Deletes the texture from GPU memory
	void Delete();
};

#endif
