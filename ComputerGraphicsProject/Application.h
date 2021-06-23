#pragma once

#include <memory>
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "model.h"
#include <glm/glm.hpp>
#include <list>

struct GLFWwindow;

class Application
{
public:
	void Run();

private:
	void Init();
	void Update(float dt);
	void Render();

	void ProcessInput(GLFWwindow* window, float dt);

	friend void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	std::list<glm::vec3> _objCoordinates;
	std::unique_ptr<Model> _ba2;
	std::unique_ptr<Model> _bed;
	std::unique_ptr<Model> _centralLamp;
	std::unique_ptr<Model> _modernChair;
	std::unique_ptr<Model> _sofa;
	std::unique_ptr<Model> _roomBox;
	std::unique_ptr<Model> _sun;
	std::unique_ptr<Model> _nightLamp;
	std::unique_ptr<Model> _table;
	std::unique_ptr<Model> _wardrobe;
	std::unique_ptr<Model> _nightstand;
	std::unique_ptr<Model> _table2;
	std::unique_ptr<Model> _hercules;
	std::unique_ptr<Model> _cube;

	std::unique_ptr<Shader> _shader;

	glm::vec3 _lightPos = glm::vec3(0.0f, 100.0f, 10.0f);
	glm::vec3 _lightPos2 = glm::vec3(0.0f, 19.5f, 0.0f);
	glm::vec3 _lightPos3 = glm::vec3(-30.0f, 5.0f, 29.0f);
	glm::vec3 _lightColor = glm::vec3(1.f, 1.f, 1.f);
	glm::vec3 _directionalDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	glm::vec3 _directionalSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 _centralLampDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 _centralLampSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 _nightLampDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 _nightLampSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 _cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 _cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 _cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float _yaw = -90.0f;
	float _pitch = 0.0f;
	float _fov = 45.f;
};
