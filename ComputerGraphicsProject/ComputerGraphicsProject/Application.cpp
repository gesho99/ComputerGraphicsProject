#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstMouse = true;
    static double lastX;
    static double lastY;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if (state != GLFW_PRESS)
        return;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    auto* app = (Application*)glfwGetWindowUserPointer(window);

    app->_yaw += xoffset;
    app->_pitch += yoffset;

    if (app->_pitch > 89.0f)
        app->_pitch = 89.0f;

    if (app->_pitch < -89.0f)
        app->_pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(app->_yaw)) * cos(glm::radians(app->_pitch));
    direction.y = sin(glm::radians(app->_pitch));
    direction.z = sin(glm::radians(app->_yaw)) * cos(glm::radians(app->_pitch));

    app->_cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* app = (Application*)glfwGetWindowUserPointer(window);

    app->_fov -= (float)yoffset;

    if (app->_fov < 1.0f)
        app->_fov = 1.0f;

    if (app->_fov > 45.0f)
        app->_fov = 45.0f;
}

void Application::ProcessInput(GLFWwindow* window, float dt)
{
    const float cameraSpeed = 6.5f * dt; // adjust accordingly

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        _cameraPos += cameraSpeed * _cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        _cameraPos -= cameraSpeed * _cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        _cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        _cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
}

void Application::Run()
{
    GLFWwindow* window;

    /* Initialize the glfw */
    if (!glfwInit())
        return;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, 640, 480);

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Init();

    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window, deltaTime);
        Update(deltaTime);
        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void Application::Init()
{
    //_ba2 = std::make_unique<Model>("res\\models\\ba2\\ba2.obj");
    _bed = std::make_unique<Model>("res\\models\\bed2\\bed.obj");
    _centralLamp = std::make_unique<Model>("res\\models\\central-lamp\\1.obj");
    _modernChair = std::make_unique<Model>("res\\models\\modern-chair\\modern-chair.obj");
    _sofa = std::make_unique<Model>("res\\models\\sofa\\sofa.obj");
    _roomBox = std::make_unique<Model>("res\\models\\room-box\\untitled.obj");
    _sun = std::make_unique<Model>("res\\models\\sun\\sun.obj");
    _nightLamp = std::make_unique<Model>("res\\models\\night-lamp\\night-lamp.obj");

    _shader = std::make_unique<Shader>("res\\lighting.vert.glsl", "res\\lighting.frag.glsl");
    _secondLightShader = std::make_unique<Shader>("res\\secondLight.vert.glsl", "res\\secondLight.frag.glsl");

    glEnable(GL_DEPTH_TEST);
}

void Application::Update(float dt)
{
    const auto view = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
    const auto projection = glm::perspective(glm::radians(_fov), 800.0f / 600.0f, 0.1f, 100.0f);

    _shader->use();
    _shader->setFloatMat4("view", view);
    _shader->setFloatMat4("projection", projection);

    _secondLightShader->use();
    _secondLightShader->setFloatMat4("view", view);
    _secondLightShader->setFloatMat4("projection", projection);
}

void Application::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::mat4(1.0f);

    glm::vec3 pointLightPositions[] = {
        _lightPos2,
        _lightPos3,
    };

    _shader->use();
    _shader->setFloat3("viewPos", _cameraPos);

    _shader->setFloat("material.shininess", 16.f);

    //_shader->setFloat3("dirLight.direction", -(_lightPos));
    //_shader->setFloat3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    //_shader->setFloat3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
    //_shader->setFloat3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    _shader->setFloat3("pointLights[0].position", pointLightPositions[0]);
    _shader->setFloat3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    _shader->setFloat3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    _shader->setFloat3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    _shader->setFloat("pointLights[0].constant", 1.0f);
    _shader->setFloat("pointLights[0].linear", 0.007);
    _shader->setFloat("pointLights[0].quadratic", 0.0002);

    _shader->setFloat3("pointLights[1].position", pointLightPositions[1]);
    _shader->setFloat3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    _shader->setFloat3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    _shader->setFloat3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
    _shader->setFloat("pointLights[1].constant", 1.0f);
    _shader->setFloat("pointLights[1].linear", 0.09);
    _shader->setFloat("pointLights[1].quadratic", 0.032);

    _shader->setFloatMat4("scale", scale);
    _shader->setFloatMat4("scale", rotate);

    float angle = -180.0f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(6.5f, 0.0f, -28.8f));
    model = glm::scale(model, glm::vec3(3.0f));
    _shader->setFloatMat4("model", model);
    _bed->Draw(*_shader);

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f, 2.0f, 5.0f));
    _shader->setFloatMat4("model", model);
    _roomBox->Draw(*_shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 18.8f, 0.0f));
    model = glm::scale(model, glm::vec3(0.8f, 0.5f, 0.8f));
    _shader->setFloatMat4("model", model);

    _centralLamp->Draw(*_shader);

    angle = 90.0f;
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(17.0f, -0.3f, -9.0f));
    model = glm::scale(model, glm::vec3(1.3f));
    _shader->setFloatMat4("model", model);

    _modernChair->Draw(*_shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, -0.3f, -27.2f));
    model = glm::scale(model, glm::vec3(0.4f, 0.5f, 0.5f));
    _shader->setFloatMat4("model", model);

    _sofa->Draw(*_shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, _lightPos);
    model = glm::scale(model, glm::vec3(0.1f));
    _shader->setFloatMat4("model", model);
    _sun->Draw(*_shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, _lightPos2);
    model = glm::scale(model, glm::vec3(0.050f));
    _shader->setFloatMat4("model", model);
    _sun->Draw(*_shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 29.0f));
    model = glm::scale(model, glm::vec3(0.1f));
    _shader->setFloatMat4("model", model);
    _nightLamp->Draw(*_shader);
}
