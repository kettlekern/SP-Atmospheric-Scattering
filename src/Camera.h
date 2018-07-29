#pragma once
#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "MatrixStack.h"
#include "SimpleComponents.hpp"
#include <GLFW/glfw3.h>
#define MESHSIZE 600
#define RESOLUTION 2.0f // Higher value = less verticies per unit of measurement

//Aparently a windows package defines these somewhere, so I'm going to break their code because that is so irresponsible of them
#undef near
#undef far


class OldCamera : public Camera
{
	glm::vec3 pos, rot;
public:
	int w, a, s, d, q, e, up, down, shift;
	int speedConst;
	OldCamera()
	{
		w = a = s = d = q = e = up = down = shift = 0;
		speedConst = 5;
		pos = rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, -3, 0);
	}

	glm::vec3 getLocation() { return pos; }

	glm::vec3 getViewDir() { return rot; }

	glm::mat4 update(float ftime);

	glm::mat4 getView() { return update(-.0005f); }

	glm::mat4 getPerspective(float aspect) {
		return glm::perspective((3.14159f / 4.0f), aspect, 0.1f, MESHSIZE * RESOLUTION);
	}

	void update(GLFWwindow* window, float elapsedTime);
};

class FPcamera : public Camera {
public:
	FPcamera() : pose(glm::vec3(0.0)) {};
	FPcamera(const glm::vec3& loc) : pose(loc) {}

	void update(GLFWwindow* window, float elapsedTime);

	glm::vec3 getLocation();

	glm::vec3 getViewDir();

	//Returns the view matrix
	glm::mat4 getView();

	void setCamera(GLFWwindow* window);

	//Returns the perspective matrix associated with the camera
	glm::mat4 getPerspective(float aspect);

	float fov = 35.0f;
	float near = .01f;
	float far = 100.0f;

protected:

	Pose pose;
	glm::vec3 upDir = glm::vec3(0.0, 1.0, 0.0);

	glm::vec2 mousePos;
	bool first = true;
	float speedmult = 1.0f;

	const float pi = 3.14159265f;
	const float VERT_ANGLE_LIMIT = cos(80 / 360 * 2 * pi);
	const float SENSITIVITY = 100.0f;
};

#endif