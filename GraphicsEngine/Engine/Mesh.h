#pragma once
#include "gameobject.h"     // Include base class
#include <glad/gl.h>		// Include glad
#include <vector>

class Shader;

class Mesh : public GameObject {
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 texCoords;	
		glm::vec3 normal;
		
	};

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
	~Mesh();

	void render(Shader* shader, glm::vec3 lightPos, glm::vec3 viewPos, const glm::mat4& viewMatrix = glm::mat4(1.0f), const glm::mat4& projectionMatrix = glm::mat4(1.0f), int texture = 0, int texture2 = 0);

private:
	// decladre handles to the VAO, and VBOs to positions, normals and texture coordinates.
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	size_t indiceCount;
};
