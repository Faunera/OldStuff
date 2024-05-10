#include "Mesh.h"
#include <kgfw/GLUtils.h>           // Include GLUtils for checkGLError
#include "shader.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) : GameObject(__FUNCTION__)
{
	indiceCount = indices.size();

	// Generate buffers and arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	checkGLError();

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind buffers and load data into them
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	checkGLError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	checkGLError();

	// vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	checkGLError();

	// vertex texCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	checkGLError();

	// vertex normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	checkGLError();

	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	indiceCount = 0;
}

void Mesh::render(Shader* shader, glm::vec3 lightPos, glm::vec3 viewPos, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, int texture, int texture2)
{
	shader->bind();

	//MVP
	shader->setUniform("MVP", projectionMatrix * glm::inverse(viewMatrix) * getModelMatrix());

	// Model matrix
	shader->setUniform("model", getModelMatrix());

	// lightPos and viewPos
	shader->setUniform("lightPos", lightPos.x, lightPos.y, lightPos.z);
	shader->setUniform("viewPos", viewPos.x, viewPos.y, viewPos.z);

	// Set texture, if set
	if (texture > 0) 
	{
		shader->setUniform("texture0", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		if (texture2 > 0)
		{
			shader->setUniform("texture1", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);
		}
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indiceCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}
