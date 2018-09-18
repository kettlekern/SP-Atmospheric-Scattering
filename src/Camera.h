#pragma once
#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "MatrixStack.h"
#include <GLFW/glfw3.h>
#define MESHSIZE 600 //This is a property of the terrain. It should be defined in a terrain class
#define RESOLUTION 2.0f // Higher value = fewer verticies per unit of measurement

//Aparently a windows package defines these somewhere, so I'm going to break their code because that is so irresponsible of them
#undef near
#undef far
#undef aspect

class FPcamera {
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
	// Returns the distance to the far plane used by the perspective matrix for the camera
	float getFarDist() { return far; }

	//Returns the perspective matrix associated with the camera
	glm::mat4 getPerspective();
	//Sets the aspect ratio to the given value
	void setPerspective(float aspect) { this->aspect = aspect; }

protected:


	glm::vec3 pos = glm::vec3(0.0f, 20.0f, 0.0f);
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