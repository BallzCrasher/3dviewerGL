#ifndef VARIABLES_H
#define VARIABLES_H
#include<glm/glm.hpp>

namespace GlobalVars
{

inline const int WIDTH               {800};
inline const int HEIGHT              {600};
inline const float MOUSE_SENSITIVITY {0.1f};
inline float last_mouse_x            {WIDTH / 2.0f};
inline float last_mouse_y            {HEIGHT / 2.0f};
inline float yaw                     {-90.0f};
inline float pitch                   {0.0f};
inline float camera_speed            {2.5f};
inline float delta_time              {0.0f};
inline float fov                     {45.0f};
inline bool first_mouse_enter        {true};
inline glm::vec3 camera_pos          {glm::vec3(0.0f,0.0f,3.0f)};
inline glm::vec3 camera_front        {glm::vec3(0.0f,0.0f,-1.0f)};
inline glm::vec3 camera_up           {glm::vec3(0.0f,1.0f,0.0f)};
inline bool flashlight_switch        {false};

} // GlobalVGL
#endif
