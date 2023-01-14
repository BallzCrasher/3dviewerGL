#ifndef VARIABLES_H
#define VARIABLES_H
#include<glm/glm.hpp>
#define WIDTH 800
#define HEIGHT 600
static float lastMouseX = WIDTH / 2.0f, lastMouseY = HEIGHT / 2.0f;
static const float mouse_sensitivity = 0.1f;
static float yaw = -90.0f;
static float pitch = 0.0f;
static float cameraSpeed = 2.5f;
static float deltaTime = 0.0f;
static float fov = 45.0f;
static bool firstMouseEnter = true;
static glm::vec3 cameraPos   = glm::vec3(0.0f,0.0f,3.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
static glm::vec3 cameraUp    = glm::vec3(0.0f,1.0f,0.0f);
static bool flashLight_switch = false;
#endif
