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
#undef aspect


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

	glm::mat4 getView() { return update(.0005f); }

	glm::mat4 getPerspective(float aspect) {
		return glm::perspective((3.14159f / 4.0f), aspect, 0.1f, MESHSIZE * RESOLUTION);
	}

	void update(GLFWwindow* window, float elapsedTime);
};

class FPcamera : public Camera {
public:
	FPcamera() {};
	FPcamera(const glm::vec3& loc) : pos(loc) {}

	void update(GLFWwindow* window, float elapsedTime);

	glm::vec3 getLocation();

	glm::vec3 getViewDir();

	const glm::vec3 & getUpDir() {
		return upDir;
	}

	//Returns the view matrix
	glm::mat4 getView();

	void setCamera(GLFWwindow* window);

	// Returns the polar coordinate theta (Yaw angle) for the direction of the camera
	float getTheta() { return theta; }
	// Returns the polar coordinate phi (Pitch angle) for the direction of the camera
	float getPhi() { return phi; }
	// Returns the aspect ratio used for the camera
	float getAspect() { return aspect; }
	// Returns the Field of View of used for the camera
	float getFOV() { return fov; }
	// Returns the distance to the near plane used by the perspective matrix for the camera
	float getNearDist() { return near; }

	//Returns the perspective matrix associated with the camera
	glm::mat4 getPerspective(float aspect);

protected:

	glm::vec3 pos = glm::vec3(0.0f, 130.0f, 0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 upDir = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec2 mousePos;
	bool first = true;
	float speedmult = 50.0f; 

	const float pi = 3.14159265f;
	const float VERT_ANGLE_LIMIT = cos(0.5f / 360.0f * 2 * pi);
	const float SENSITIVITY = 100.0f;

	float fov = 60.0f;
	float near = 0.1f;
	float far = 800.0f;
	float theta;
	float phi;
	float aspect = 1.0f;
};

#endif