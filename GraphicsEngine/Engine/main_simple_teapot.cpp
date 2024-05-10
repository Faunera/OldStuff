#include <stdio.h>			// Include stdio.h, which contains printf-function
#include <vector>			// Include std::vector
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <kgfw/Object.h>	// Include kgfw::Object
#include <glad/gl.h>		// Include glad
#include <GLFW/glfw3.h>		// Include glfw
#include <kgfw/GLUtils.h>	// Include GLUtils for checkGLError
#include "glm/gtc/constants.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "shader.h"			// Include Shader-class.
#include "camera.h"			// Include Camera-class.
#include "teapot.h"			// Include Teapot-class.
#include "texture.h"		// Include Texture-class
#include "Mesh.h"

std::string readFile(const std::string& filePath)
{
	std::string defaultPath = "Assets/Shaders/";
	auto fileString = std::ostringstream{};
	std::ifstream input_file(defaultPath + filePath);
	if (!input_file.is_open())
	{
		std::cerr << "Could not open the file - '"
			<< filePath << "'\n";
		exit(EXIT_FAILURE);
	}
	fileString << input_file.rdbuf();
	input_file.close();
	return fileString.str();
}

// View translate commented out in render of teapot.cpp
class Application : public kgfw::Object
{
public:
	Application()
		: Object(__FUNCTION__)
		, shader(0) {

		idSlot = 0;
		/*	Shaders area loaded from files.
			Put them at the end of this file in comments.
		*/
		std::string vertex = readFile("Default.vert");
		std::string fragment = readFile("Default.frag");

		const char* vertexShaderSource = vertex.c_str();
		const char* fragmentShaderSource = fragment.c_str();

		lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		objectColor = glm::vec3(0.0f, 1.0f, 1.0f);
		lightPos = glm::vec3(-0.25f, 1.0f, 0.0f);

		// Build and compile our shader program
		shader = new Shader(vertexShaderSource, fragmentShaderSource);
		shader->bind();
		shader->setUniform("blinn", false);

		camera = new Camera(glm::half_pi<float>(), 640.0f, 480.0f, 0.1f, 100.0f);
		camera->setPosition(glm::vec3(0.0f, 0.4f, 0.4f));
		camera->setLookAt(glm::vec3(0.0f, 0.0f, -0.25f));

		// Teapots
		for (int i = 0; i < 1; i++)
		{
			teapot.push_back(new Teapot());
		}
		teapot[0]->setPosition(glm::vec3(-0.5f, 0.0f, 0.0f));
		teapot[0]->setScaling(glm::vec3(1.0f, 1.0f, 1.0f));

		// Create texture 

		texture.push_back(new Texture("Assets/Images/CrowOfJudgement.jpg"));
		texture.push_back(new Texture("Assets/Images/checkerboard.png"));
		texture.push_back(new Texture("Assets/Images/c1.png"));
		texture.push_back(new Texture("Assets/Images/c2.png"));
		texture.push_back(new Texture("Assets/Images/c3.png"));

		auto mesh = loadMeshes("Assets/Objects/grass.obj");
		meshes.push_back(mesh[0]);
		meshes[0]->setScaling(glm::vec3(0.5f));
		meshes[0]->setPosition(glm::vec3(0.0f, -5.0f, 0.0f));
		meshes[0]->setRotationZ(-glm::half_pi<float>());

		mesh = loadMeshes("Assets/Objects/torusknot.obj");
		meshes.push_back(mesh[0]);
		meshes[1]->setScaling(glm::vec3(0.02f));
		meshes[1]->setPosition(glm::vec3(-0.5f, 0.0f, -1.0f));

		/* Doesn't mesh properly */		
		mesh = loadMeshes("Assets/Objects/suzanne.obj");
		meshes.push_back(mesh[0]);
		meshes[2]->setScaling(glm::vec3(0.03f));
		meshes[2]->setPosition(glm::vec3(0.5f, 0.0f, 0.0f));
		
		mesh = loadMeshes("Assets/Objects/cylinder.obj");
		meshes.push_back(mesh[0]);
		meshes[3]->setScaling(glm::vec3(0.03f));
		meshes[3]->setPosition(glm::vec3(0.5f, 0.0f, -1.0f));

		mesh = loadMeshes("Assets/Objects/torus.obj");
		meshes.push_back(mesh[0]);
		meshes[4]->setScaling(glm::vec3(0.02f));
		meshes[4]->setPosition(glm::vec3(-3.0f, 0.0f, 1.0f));

		// Enable depth buffering
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	~Application() {
		// Delete shader
		delete shader;
		shader = 0;

		for (auto t : teapot)
		{
			delete t;
			t = 0;
		}

		for (auto m : meshes)
		{
			delete m;
			m = 0;
		}
		// Delete Camera
		delete camera;
		camera = 0;

		// Delete texture
		for (auto tex : texture)
		{
			delete tex;
			tex = 0;
		}
		
	}

	void render(GLFWwindow* window) 
	{
		shader->bind();
		shader->setUniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
		shader->setUniform("objectColor", objectColor.x, objectColor.y, objectColor.z);
		shader->setUniform("viewPos", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
		shader->setUniform("lightPos", lightPos.x, lightPos.y, lightPos.z);

		// Query the size of the framebuffer (window content) from glfw.
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		// Setup the opengl wiewport (i.e specify area to draw)
		glViewport(0, 0, width, height);
		checkGLError();
		// Set color to be used when clearing the screen
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		checkGLError();
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkGLError();

		
		for (Teapot* t : teapot)
		{
			if (idSlot >= texture.size())
			{			
				idSlot = 0;
			}
			t->render(shader, glm::inverse(camera->getModelMatrix()), camera->getProjectionMatrix(), texture[idSlot]->getTextureObject());
		}
		int i = 0;
		for (Mesh* m : meshes)
		{
			m->render(shader, lightPos, glm::vec3(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z), camera->getModelMatrix(), camera->getProjectionMatrix(), texture[i]->getTextureObject(), texture[idSlot]->getTextureObject());
			i++;
		}
	}


	Mesh* processMesh(aiMesh* mesh)
	{
		std::vector<Mesh::Vertex> vertices;
		std::vector<unsigned int> indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Mesh::Vertex vertex;
			// load data from assimp mesh to our containers
			if (&mesh->mVertices[i])
			{
				vertex.position.x = mesh->mVertices[i].x;
				vertex.position.y = mesh->mVertices[i].y;
				vertex.position.z = mesh->mVertices[i].z;
			}
			else
			{
				printf("No Vertices found\n");
				vertex.position = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			if (&mesh->mNormals[i])
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}
			else
			{
				printf("No Normals found\n");
				vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				printf("No texture coords\n");
				vertex.texCoords = { 0.0f, 0.0f };
			}

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		return new Mesh(vertices, indices);
	}

	void processNode(std::vector<Mesh*>* mesh, aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* meshs = scene->mMeshes[node->mMeshes[i]];
			mesh->push_back(processMesh(meshs));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(mesh, node->mChildren[i], scene);
		}
	}

	std::vector<Mesh*> loadMeshes(const std::string& path)
	{
		//TODO 1: create the container that will be returned by this function
		//read file with Assimp
		std::vector<Mesh*> mesh;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		//Check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			printf("Error loading model file \"%s\": \"%s\" ", path.c_str(), importer.GetErrorString());
			return mesh;
		}
		// retrieve the directory path of the filepath
		std::string dir = path.substr(0, path.find_last_of('/'));
		//TODO 2: process Assimp's root node recursively

		processNode(&mesh, scene->mRootNode, scene);

		return mesh;
	}

	void update(float deltaTime) 
	{
		for (Teapot * t : teapot)
		{
		//	t->setRotationZ(t->getRotationZ() + deltaTime);
		}
		
	}
	void updateInput(GLFWwindow* window)
	{
		// Camera movement
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		camera->setLookAt(glm::vec3(xpos / 1000, -2*ypos / 1000, 0.0f));

		float speed = 0.01f;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			speed *= 10;
		}
		else
		{
			speed = 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera->setPosition(glm::vec3(camera->getPosition().x , camera->getPosition().y, camera->getPosition().z+ speed));
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera->setPosition(glm::vec3(camera->getPosition().x , camera->getPosition().y, camera->getPosition().z- speed));
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera->setPosition(glm::vec3(camera->getPosition().x + speed, camera->getPosition().y, camera->getPosition().z));
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera->setPosition(glm::vec3(camera->getPosition().x- speed, camera->getPosition().y, camera->getPosition().z ));
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			camera->setPosition(glm::vec3(camera->getPosition().x, camera->getPosition().y+ speed, camera->getPosition().z ));
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			camera->setPosition(glm::vec3(camera->getPosition().x, camera->getPosition().y- speed, camera->getPosition().z ));
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			camera->setPosition(glm::vec3(0.0f, 0.4f, 0.4f));
		}
	
		// Lights and objects
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			teapot.push_back(new Teapot());
			teapot.back()->setPosition(glm::vec3(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z));
		}
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		{
			lightPos = glm::vec3(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
		}
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		{
			lightColor = camera->getPosition();
		}
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		{
			objectColor = camera->getPosition();
		}
		if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		{
			idSlot++;
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			shader->bind();
			shader->setUniform("blinn", true);
		}
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		{
			shader->bind();
			shader->setUniform("blinn", false);
		}

	}

private:
	int idSlot;
	Shader* shader;		// Pointer to the Shader object
	Camera* camera;		// Camera.
	std::vector<Texture*> texture;
	std::vector<Teapot*> teapot;
	std::vector<Mesh*> meshes;

	glm::vec3 lightColor;
	glm::vec3 objectColor;
	glm::vec3 lightPos;
};

// Global pointer to the application
Application* g_app = 0;

int main(void) {
	// Set c++-lambda as error call back function for glfw.
	glfwSetErrorCallback([](int error, const char* description) {
		fprintf(stderr, "Error %d: %s\n", error, description);
	});

	// Try to initialize glfw
	if (!glfwInit()) {
		
		return -1;
	}

	// Create window and check that creation was succesful.
	GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL window", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Set current context
	glfwMakeContextCurrent(window);
	// Load GL functions using glad
	gladLoadGL(glfwGetProcAddress);

	// Create application
	g_app = new Application();
	
	// Specify the key callback as c++-lambda to glfw
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		// Close window if escape is pressed by the user.
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	});

	// Get time using glfwGetTime-function, for delta time calculation.
	float prevTime = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		// Render the game frame and swap OpenGL back buffer to be as front buffer.
		g_app->render(window);
		glfwSwapBuffers(window);

		// Poll other window events.
		glfwPollEvents();

		// Compute application frame time (delta time) and update application
		float curTime = (float)glfwGetTime();
		float deltaTime = curTime - prevTime;
		prevTime = curTime;
		g_app->update(deltaTime);
		g_app->updateInput(window);
	}

	// Delete application
	delete g_app;
	g_app = 0;

	// Destroy window
	glfwDestroyWindow(window);

	// Terminate glfw
	glfwTerminate();

	return 0;
}

/* Vertex
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
		texCoord = in_texCoord;
		gl_Position = MVP * vec4(in_position, 1.0f);
		FragPos = vec3(model * vec4(in_position, 1.0f));
	}
*/
/* Fragment
	#version 330 core

	uniform sampler2D texture0;
	uniform sampler2D texture1;

	uniform vec3 objectColor;
	uniform vec3 lightColor;
	uniform vec3 lightPos;

	uniform vec3 viewPos;
	uniform bool blinn;

	in vec3 normal;
	in vec3 FragPos;
	in vec2 texCoord;

	out vec4 FragColor;

	void main()
	{
		float ambientStr = 0.1f;
		vec3 ambient = ambientStr * lightColor;

		vec3 norm = normalize(normal);
		vec3 lightDir = normalize(lightPos - FragPos);  
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;
	
		float specularStr = 0.1f;
	
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 16);
		if (blinn)
		{
			vec3 halfwayDir = normalize(lightDir + viewDir);
			spec = pow(max(dot(viewDir, halfwayDir), 0.0f), 16);
		}
	
		vec3 specular = specularStr * spec * lightColor;

		vec3 result = (ambient + diffuse + specular)* objectColor;
	
		vec4 texel0 = texture2D(texture0, texCoord);
		vec4 texel1 = texture2D(texture1, texCoord);
		vec4 texResult = mix( texel0, texel1, 0.2); 
	
		FragColor = texResult + vec4(result, 1.0f);
	
	  //  FragColor = texture2D(texture0, texCoord) + vec4(result, 1.0f);
	}
*/
