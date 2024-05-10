#pragma once
#include "gameobject.h"     // Include base class
#include <glad/gl.h>		// Include glad

class Shader;

class Teapot : public GameObject {
public:
	Teapot();
	~Teapot();

    void render(Shader* shader, const glm::mat4& viewMatrix = glm::mat4(1.0f), const glm::mat4& projectionMatrix = glm::mat4(1.0f), int texture = 0);

private:
	// decladre handles to the VAO, and VBOs to positions, normals and texture coordinates.
	GLuint positionsVbo;
	GLuint vao;
	GLuint texCoordsVbo;
	GLuint normals;
};
