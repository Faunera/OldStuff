#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"
#include <kgfw/GLUtils.h>	// Include GLUtils for checkGLError

#include "../ext/stb-master/stb_image.h"

Texture::Texture(std::string texturePath) : Object(__FUNCTION__) {
	
	// Load texture data
	data = stbi_load(texturePath.c_str(), &width, &height, &numberOfChannels, 0);

	// Create texture object
	glGenTextures(1, &textureObject);

	// Bind texture
	glBindTexture(GL_TEXTURE_2D, textureObject);

	// Set wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Generate texture
	if (numberOfChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (numberOfChannels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	// Free image memory
	stbi_image_free(data);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_textureId);
}

GLuint Texture::getTextureId() const {
	return m_textureId;
}

unsigned int Texture::getTextureObject() const
{
	return textureObject;
}
