#include "shader.h"			// Include class header
#include <stdio.h>			// Include stdio.h, which contains printf-function
#include <kgfw/GLUtils.h>	// Include GLUtils for checkGLError
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* const vertexShaderString, const char* const fragmentShaderString) 
	: Object(__FUNCTION__)
	, m_shaderProgram(0) {
	// Create and compile vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	checkGLError();
	glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
	checkGLError();
	glCompileShader(vertexShader);
	checkGLError();

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	checkGLError();
	if (!success) {
		// If failed, get error string using glGetShaderInfoLog-function.
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		checkGLError();
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
	}

	// Create and compile fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	checkGLError();
	glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
	checkGLError();
	glCompileShader(fragmentShader);
	checkGLError();
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	checkGLError();
	if (!success) {
		// If failed, get error string using glGetShaderInfoLog-function.
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		checkGLError();
		printf("ERROR: Shader compilation failed: \"%s\"\n", infoLog);
	}

	// link shaders
	m_shaderProgram = glCreateProgram();
	checkGLError();
	glAttachShader(m_shaderProgram, vertexShader);
	checkGLError();
	glAttachShader(m_shaderProgram, fragmentShader);
	checkGLError();
	glLinkProgram(m_shaderProgram);
	checkGLError();
	// check for linking errors
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	checkGLError();
	if (!success) {
		// If failed, get error string using glGetProgramInfoLog-function.
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
		checkGLError();
		printf("ERROR: Shader link failed: \"%s\"\n", infoLog);
	}

	// After linking, the shaders can be deleted.
	glDeleteShader(vertexShader);
	checkGLError();
	glDeleteShader(fragmentShader);
	checkGLError();
}

Shader::~Shader() {
	// Delete shader program
	glDeleteProgram(m_shaderProgram);
	checkGLError();
}

void Shader::bind() {
	glUseProgram(m_shaderProgram);
	checkGLError();
}


void Shader::setUniform(const std::string& name, float x, float y, float z) {
	GLint loc = glGetUniformLocation(m_shaderProgram, name.c_str());
	if (loc < 0) {
		return; // Don't set the uniform value, if it not found
	}
	glUniform3f(loc, x, y, z);
	checkGLError();
}

void Shader::setUniform(const std::string& name, float x, float y, float z, float w) {
	GLint loc = glGetUniformLocation(m_shaderProgram, name.c_str());
	if (loc < 0) {
		return; // Don't set the uniform value, if it not found
	}
	glUniform4f(loc, x, y, z, w);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		glUniform3f(loc, x, y, z); // Set as 3 component value in case of 4 component set failed.
	}

	checkGLError();
}

void Shader::setUniform(const std::string& name, const glm::mat4& m) {
    GLint loc = glGetUniformLocation(m_shaderProgram, name.c_str());
    if (loc < 0) {
        return; // Don't set the uniform value, if it not found
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
    checkGLError();
}

void Shader::setUniform(const std::string& name, int value) {
	GLint loc = glGetUniformLocation(m_shaderProgram, name.c_str());
	if (loc < 0) {
		return; // Don't set the uniform value, if it not found
	}
	glUniform1i(loc, value);
	checkGLError();
}

