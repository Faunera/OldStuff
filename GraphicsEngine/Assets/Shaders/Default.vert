#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_texCoord;
layout (location = 2) in vec3 in_normal;

uniform mat4 MVP;
uniform mat4 model;

out vec3 normal;
out vec2 texCoord;
out vec3 FragPos;

void main()
{
	normal = inverse(transpose(mat3(MVP))) * normalize(in_normal);
	//normal = in_normal;
	texCoord = in_texCoord;
	gl_Position = MVP * vec4(in_position, 1.0f);
	FragPos = vec3(model * vec4(in_position, 1.0f));
}