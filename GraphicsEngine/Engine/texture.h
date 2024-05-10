#pragma once
#include <string>
#include <kgfw/Object.h>	// Include kgfw::Object to be used as a base class
#include <glad/gl.h>		// Include glad


class Texture : public kgfw::Object {
public:
	Texture(std::string texturePath);
	~Texture();

	GLuint getTextureId() const;
	unsigned int getTextureObject() const;
private:
	GLuint				m_textureId;	// Texture id
	unsigned int textureObject;
	int width;
	int height; 
	int numberOfChannels; 
	unsigned char* data; 
};
