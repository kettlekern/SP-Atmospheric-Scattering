#include "Camera.h"
#include <IOstream>


using namespace glm;

void FPcamera::setCamera(GLFWwindow* window) {
	auto View = std::make_shared<MatrixStack>();

	double mousePosX, mousePosY;
	glfwGetCursorPos(window, &mousePosX, &mousePosY);

	theta = -(float)mousePosX / 36000.0f * SENSITIVITY;
	phi = (float)(mousePosY - mousePos.y) / 36000.0f * SENSITIVITY;

	if (VERT_ANGLE_LIMIT <= phi) {
		mousePos.y = mousePosY - VERT_ANGLE_LIMIT * 36000.0f / SENSITIVITY;
		phi = VERT_ANGLE_LIMIT;
	}

	if (-VERT_ANGLE_LIMIT >= phi) {
		mousePos.y = mousePosY + VERT_ANGLE_LIMIT * 36000.0f / SENSITIVITY;
		phi = -VERT_ANGLE_LIMIT;
	}
	direction.x = sin(theta) * sin(phi + pi / 2);
	direction.y = cos(phi + pi / 2);
	direction.z = cos(theta) * sin(phi + pi / 2);
	direction = normalize(direction);
}

void FPcamera::update(GLFWwindow* window, float elapsedTime){
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	setCamera(window);

	vec3 viewDir = getViewDir();
	vec3 rightDir = cross(viewDir, upDir);

	float finalmult = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? speedmult * 5.0f : speedmult;

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		pos += viewDir * elapsedTime * finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		pos -= viewDir * elapsedTime * finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		pos += rightDir * elapsedTime * finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		pos -= rightDir * elapsedTime * finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		pos += upDir * elapsedTime * finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		pos -= upDir * elapsedTime * finalmult;
	}
}

glm::vec3 FPcamera::getLocation(){
	return pos;
}

glm::vec3 FPcamera::getViewDir(){
	return direction;
}

//Returns the view matrix
glm::mat4 FPcamera::getView(){
	return glm::lookAt(pos, pos + getViewDir(), upDir);
}

//Returns the perspective matrix associated with the camera
glm::mat4 FPcamera::getPerspective(){
	return perspective(glm::radians(fov), aspect, near, far);
}