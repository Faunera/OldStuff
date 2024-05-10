#include "teapot.h"
#include <kgfw/GLUtils.h>           // Include GLUtils for checkGLError
#include "shader.h"
#include <glm/gtx/transform.hpp>	// glm transform functions.
#include <vector>
#include "teapot_data.h"


Teapot::Teapot()
    : GameObject(__FUNCTION__) {

	// TODO:
	// 1. Create VAO
	glGenVertexArrays(1, &vao);
	checkGLError;
	// 2. Create VBO for positions. Use TEAPOT_POSITIONS -variable (declader in teapot_data.h) for positions data.
	glGenBuffers(1, &positionsVbo);
	checkGLError;

	glBindVertexArray(vao);
	checkGLError;

	glBindBuffer(GL_ARRAY_BUFFER, positionsVbo);
	checkGLError;
	glBufferData(GL_ARRAY_BUFFER, sizeof(TEAPOT_POSITIONS), TEAPOT_POSITIONS, GL_STATIC_DRAW);
	checkGLError;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	checkGLError;
	glEnableVertexAttribArray(0);

	// 3. Create VBO for texture coordinates Use TEAPOT_TEXTURE_COORDINATES -variable (declader in teapot_data.h) 
	//    for texture coordinates data.
	glGenBuffers(1, &texCoordsVbo);
	checkGLError;
	glBindBuffer(GL_ARRAY_BUFFER, texCoordsVbo);
	checkGLError;
	glBufferData(GL_ARRAY_BUFFER, sizeof(TEAPOT_TEXTURE_COORDINATES), TEAPOT_TEXTURE_COORDINATES, GL_STATIC_DRAW);
	checkGLError;
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	checkGLError();
	glEnableVertexAttribArray(1);
	checkGLError;
	
	// 4. Create VBO for normals. Use TEAPOT_NORMALS variable (declader in teapot_data.h) 
	//    for normals data.
	glGenBuffers(1, &normals);
	checkGLError;
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	checkGLError;
	glBufferData(GL_ARRAY_BUFFER, sizeof(TEAPOT_NORMALS), TEAPOT_NORMALS, GL_STATIC_DRAW);
	checkGLError;
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	checkGLError;
	glEnableVertexAttribArray(2);
	checkGLError;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	checkGLError;
	glBindVertexArray(0);
	checkGLError;
}

Teapot::~Teapot() {
	// TODO: Delete created VAO and VBOs here.
	glDeleteBuffers(1, &vao);
	glDeleteBuffers(1, &positionsVbo);
	glDeleteBuffers(1, &texCoordsVbo);
	glDeleteBuffers(1, &normals);
}

void Teapot::render(Shader* shader, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, int texture) {

	// TODO: Bind the shader
	shader->bind();

	//shader->setUniform("MVP", viewMatrix * projectionMatrix * getModelMatrix());

	//  Set texture uniform to the shader
	if (texture > 0) {
		shader->setUniform("texture0", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	glm::mat4 view = viewMatrix;
	//view = glm::translate(view, getPosition());
	glm::mat4 projection = projectionMatrix;
	glm::mat4 model = getModelMatrix();

	// Set uniform values to the shader (MVP Matrix)

	// Bind VAO.

	shader->setUniform("MVP", projection * view * model);
	glBindVertexArray(vao);

	// Draw teapot using glDrawArrays with TEAPOT_NUM_VERTICES.
	glDrawArrays(GL_TRIANGLES, 0, TEAPOT_NUM_VERTICES);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

