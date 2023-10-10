#ifndef VARIABLES_H
#define VARIABLES_H
#include<glm/glm.hpp>
#define WIDTH 800
#define HEIGHT 600
inline float lastMouseX = WIDTH / 2.0f, lastMouseY = HEIGHT / 2.0f;
inline const float mouse_sensitivity = 0.1f;
inline float yaw = -90.0f;
inline float pitch = 0.0f;
inline float cameraSpeed = 2.5f;
inline float deltaTime = 0.0f;
inline float fov = 45.0f;
inline bool firstMouseEnter = true;
inline glm::vec3 cameraPos   = glm::vec3(0.0f,0.0f,3.0f);
inline glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
inline glm::vec3 cameraUp    = glm::vec3(0.0f,1.0f,0.0f);
inline bool flashLight_switch = false;
#endif
