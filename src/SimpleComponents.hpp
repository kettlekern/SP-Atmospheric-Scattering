#pragma once
#ifndef _SIMPLECOMPONENTS_H_
#define _SIMPLECOMPONENTS_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

using namespace std;

// This file is for components so simple they can be written as just a header

class Camera {
 public:
  virtual ~Camera(){};
  virtual glm::vec3 getLocation() = 0;
  virtual glm::vec3 getViewDir() = 0;
  virtual glm::mat4 getView() = 0;
  virtual glm::mat4 getPerspective(float aspect) = 0;
  virtual void update(GLFWwindow* window, float elapsedTime) = 0;

};
#endif
