#include "Application.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <list>

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
    bool cond = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //restrict movement through walls
    if ((_cameraPos + cameraSpeed * _cameraFront).x < -30.0f || (_cameraPos + cameraSpeed * _cameraFront).x > 30.0f
        || (_cameraPos + cameraSpeed * _cameraFront).z < -30.0f || ((_cameraPos + cameraSpeed * _cameraFront).z > 30.0f)) {
        cond = true;
    }

    //restrict movement through wardrobe
    if ((_cameraPos + cameraSpeed * _cameraFront).x < -23.0f && (_cameraPos + cameraSpeed * _cameraFront).x > -30.0f
        && (_cameraPos + cameraSpeed * _cameraFront).z < 11.23f && (_cameraPos + cameraSpeed * _cameraFront).z > -8.0f) {
        cond = true;
    }

    //restrict movement through bed
    if ((_cameraPos + cameraSpeed * _cameraFront).z > 8.0f && (_cameraPos + cameraSpeed * _cameraFront).z < 30.0f
        && (_cameraPos + cameraSpeed * _cameraFront).x < 10.0f && (_cameraPos + cameraSpeed * _cameraFront).x > -9.0f) {
        cond = true;
    }

    //restrict movement through night lamp
    if ((_cameraPos + cameraSpeed * _cameraFront).x < -24.0f && (_cameraPos + cameraSpeed * _cameraFront).x > -30.0f
        && (_cameraPos + cameraSpeed * _cameraFront).z > 23.0f && (_cameraPos + cameraSpeed * _cameraFront).z < 30.0f) {
        cond = true;
    }

    //restrict movement through table
    if ((_cameraPos + cameraSpeed * _cameraFront).x < 7.2f && (_cameraPos + cameraSpeed * _cameraFront).x > -3.5f
        && (_cameraPos + cameraSpeed * _cameraFront).z > -30.0f && (_cameraPos + cameraSpeed * _cameraFront).z < -11.6f) {
        cond = true;
    }

    //restrict movement through 1st modern chair
    if ((_cameraPos + cameraSpeed * _cameraFront).x < 18.4f && (_cameraPos + cameraSpeed * _cameraFront).x > 10.9f
        && (_cameraPos + cameraSpeed * _cameraFront).z > -22.2f && (_cameraPos + cameraSpeed * _cameraFront).z < -13.0f) {
        cond = true;
    }

    //restrict movement through 2nd modern chair
    if ((_cameraPos + cameraSpeed * _cameraFront).x < -6.8f && (_cameraPos + cameraSpeed * _cameraFront).x > -13.7f
        && (_cameraPos + cameraSpeed * _cameraFront).z > -21.8f && (_cameraPos + cameraSpeed * _cameraFront).z < -13.6f) {
        cond = true;
    }

    //restrict movement through sofa
    if ((_cameraPos + cameraSpeed * _cameraFront).x < 9.95f && (_cameraPos + cameraSpeed * _cameraFront).x > -5.58f
        && (_cameraPos + cameraSpeed * _cameraFront).z > -30.0f && (_cameraPos + cameraSpeed * _cameraFront).z < -22.7f) {
        cond = true;
    }

    //restrict movement through 1st nightstand
    if ((_cameraPos + cameraSpeed * _cameraFront).x < 14.6f && (_cameraPos + cameraSpeed * _cameraFront).x > 10.0f
        && (_cameraPos + cameraSpeed * _cameraFront).z > 25.6f && (_cameraPos + cameraSpeed * _cameraFront).z < 30.0f) {
        cond = true;
    }

    //restrict movement through 2nd nightstand
    if ((_cameraPos + cameraSpeed * _cameraFront).x < -9.0f && (_cameraPos + cameraSpeed * _cameraFront).x > -14.3f
        && (_cameraPos + cameraSpeed * _cameraFront).z > 25.6f && (_cameraPos + cameraSpeed * _cameraFront).z < 30.0f) {
        cond = true;
    }

    //restrict movement through 2nd table
    if ((_cameraPos + cameraSpeed * _cameraFront).x < 30.0f && (_cameraPos + cameraSpeed * _cameraFront).x > 19.6f
        && (_cameraPos + cameraSpeed * _cameraFront).z > -8.8f && (_cameraPos + cameraSpeed * _cameraFront).z < 9.4f) {
        cond = true;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (cond == false) {
            _cameraPos += cameraSpeed * _cameraFront;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (cond == false) {
            _cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (cond == false) {
            _cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
        }
    }

    //toggle night mode
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        _directionalDiffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        _directionalSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    //toggle day mode
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        _directionalDiffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        _directionalSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
    }

    //turn off central lamp
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        _centralLampDiffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        _centralLampSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    //turn on central lamp
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        _centralLampDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        _centralLampSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    //turn off night lamp
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        _nightLampDiffuse = glm::vec3(0.0f, 0.0f, 0.0f);
        _nightLampSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    //turn on night lamp
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        _nightLampDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        _nightLampSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    //control central lamp
    if (_cameraPos.x < 1.5 && _cameraPos.x > -0.4 && _cameraPos.z > 0 && _cameraPos.z < 1) {
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && _centralLampDiffuse.x > 0.0f && _centralLampDiffuse.y > 0.0f && _centralLampDiffuse.z > 0.0f) {
            _centralLampDiffuse -= glm::vec3(0.025f, 0.025f, 0.025f);
            _centralLampSpecular -= glm::vec3(0.025f, 0.025f, 0.025f);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && _centralLampDiffuse.x < 0.9f && _centralLampDiffuse.y < 0.9f && _centralLampDiffuse.z < 0.9f) {
            _centralLampDiffuse += glm::vec3(0.025f, 0.025f, 0.025f);
            _centralLampSpecular += glm::vec3(0.025f, 0.025f, 0.025f);
        }
    }

    //control night lamp
    if (_cameraPos.x < -22.0f && _cameraPos.x > -30.0f && _cameraPos.z > 21.0f && _cameraPos.z < 30.0f) {
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && _nightLampDiffuse.x > 0.0f && _nightLampDiffuse.y > 0.0f && _nightLampDiffuse.z > 0.0f) {
            _nightLampDiffuse -= glm::vec3(0.025f, 0.025f, 0.025f);
            _nightLampSpecular -= glm::vec3(0.025f, 0.025f, 0.025f);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && _nightLampDiffuse.x < 0.9f && _nightLampDiffuse.y < 0.9f && _nightLampDiffuse.z < 0.9f) {
            _nightLampDiffuse += glm::vec3(0.025f, 0.025f, 0.025f);
            _nightLampSpecular += glm::vec3(0.025f, 0.025f, 0.025f);
        }
    }

    _cameraPos.y = 10;
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
    _roomBox = std::make_unique<Model>("res\\models\\room-box\\jhg.obj");
    _sun = std::make_unique<Model>("res\\models\\sun\\sun.obj");
    _nightLamp = std::make_unique<Model>("res\\models\\night-lamp\\night-lamp.obj");
    _table = std::make_unique<Model>("res\\models\\table\\Artu.obj");
    _wardrobe = std::make_unique<Model>("res\\models\\wardrobe\\10-1.obj");
    _nightstand = std::make_unique<Model>("res\\models\\nightstand\\TumbaObj.obj");
    _table2 = std::make_unique<Model>("res\\models\\table2\\dsadd.obj");
    _hercules = std::make_unique<Model>("res\\models\\hercules\\hercules.obj");
    _cube = std::make_unique<Model>("res\\models\\cube\\cube.obj");

    _shader = std::make_unique<Shader>("res\\lighting.vert.glsl", "res\\lighting.frag.glsl");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::Update(float dt)
{
    const auto view = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
    const auto projection = glm::perspective(glm::radians(_fov), 800.0f / 600.0f, 0.1f, 100.0f);

    _shader->use();
    _shader->setFloatMat4("view", view);
    _shader->setFloatMat4("projection", projection);
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

    _shader->setFloat3("dirLight.direction", -(_lightPos));
    _shader->setFloat3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    _shader->setFloat3("dirLight.diffuse", _directionalDiffuse);
    _shader->setFloat3("dirLight.specular", _directionalSpecular);
    
    _shader->setFloat3("pointLights[0].position", pointLightPositions[0]);
    _shader->setFloat3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    _shader->setFloat3("pointLights[0].diffuse", _centralLampDiffuse);
    _shader->setFloat3("pointLights[0].specular", _centralLampSpecular);
    _shader->setFloat("pointLights[0].constant", 1.0f);
    _shader->setFloat("pointLights[0].linear", 0.007);
    _shader->setFloat("pointLights[0].quadratic", 0.0002);

    _shader->setFloat3("pointLights[1].position", pointLightPositions[1]);
    _shader->setFloat3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    _shader->setFloat3("pointLights[1].diffuse", _nightLampDiffuse);
    _shader->setFloat3("pointLights[1].specular", _nightLampSpecular);
    _shader->setFloat("pointLights[1].constant", 1.0f);
    _shader->setFloat("pointLights[1].linear", 0.09);
    _shader->setFloat("pointLights[1].quadratic", 0.032);

    _shader->setFloatMat4("scale", scale);
    _shader->setFloatMat4("scale", rotate);

    float angle = -180.0f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-6.5f, 0.0f, 28.8f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
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
    model = glm::translate(model, glm::vec3(-10.0f, -0.3f, -18.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.3f));
    _shader->setFloatMat4("model", model);
    _modernChair->Draw(*_shader);

    angle = -90.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(14.5f, -0.3f, -18.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
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

   /* model = glm::mat4(1.0f);
    model = glm::translate(model, _lightPos2);
    model = glm::scale(model, glm::vec3(0.050f));
    _shader->setFloatMat4("model", model);
    _sun->Draw(*_shader);*/

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 29.0f));
    model = glm::scale(model, glm::vec3(0.1f));
    _shader->setFloatMat4("model", model);
    _nightLamp->Draw(*_shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, -18.0f));
    model = glm::scale(model, glm::vec3(6.0f));
    _shader->setFloatMat4("model", model);
    _table->Draw(*_shader);

    angle = -90.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-30.2f, -0.3f, 0.0f));
    model = glm::scale(model, glm::vec3(6.0f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    _shader->setFloatMat4("model", model);
    _wardrobe->Draw(*_shader);

    angle = 90.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10.8f, 2.2f, 30.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    _shader->setFloatMat4("model", model);
    _nightstand->Draw(*_shader);

    angle = 90.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-11.0f, 2.2f, 30.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    _shader->setFloatMat4("model", model);
    _nightstand->Draw(*_shader);

    angle = 270.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(27.0f, 2.2f, 0.5f));
    model = glm::scale(model, glm::vec3(0.3f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    _shader->setFloatMat4("model", model);
    _table2->Draw(*_shader);

    angle = -90.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(27.0f, 5.2f, 0.5f));
    model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.25f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    _shader->setFloatMat4("model", model);
    _hercules->Draw(*_shader);


    ////wardrobe
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(-28.0f, 0.0f, 1.8f));
    //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 6.0f));
    //_shader->setFloatMat4("model", model);
    //_cube->Draw(*_shader);
    //
    ////bed
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 9.5f));
    //model = glm::scale(model, glm::vec3(9.0f, 1.0f, 1.0f));
    //_shader->setFloatMat4("model", model);
    //_cube->Draw(*_shader);
    //_objCoordinates.push_back(glm::vec3(9.0f, 0.0f, 9.5f));
    //_objCoordinates.push_back(glm::vec3(-9.0f, 0.0f, 9.5f));

    ////night-lamp
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(-30.0f, 0.0f, 29.0f));
    //model = glm::scale(model, glm::vec3(2.5f, 1.0f, 2.5f));
    //_shader->setFloatMat4("model", model);
    //_cube->Draw(*_shader);

    ////table
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(2.0f, 0.0f, -17.0f));
    //model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.5f));
    //_shader->setFloatMat4("model", model);
    //_cube->Draw(*_shader);

    ////modern-chair
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(15.0f, 0.0f, -18.0f));
    //model = glm::scale(model, glm::vec3(4.0f, 1.0f, 3.5f));
    //_shader->setFloatMat4("model", model);
    //_cube->Draw(*_shader);

    ////modern-chair
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(-11.0f, -0.3f, -18.0f));
    //model = glm::scale(model, glm::vec3(4.0f, 1.0f, 3.5f));
    //_shader->setFloatMat4("model", model);
    //_cube->Draw(*_shader);

    ////sofa
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(2.0f, 0.0f, -25.0f));
    //model = glm::scale(model, glm::vec3(9.0f, 1.0f, 3.0f));
    //_shader->setFloatMat4("model", model);
    //_cube->Draw(*_shader);
}
