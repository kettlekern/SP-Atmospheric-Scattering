#include "Camera.h"
#include <IOstream>


using namespace glm;

glm::mat4 OldCamera::update(float ftime)
{
	if (shift == 1) {
		speedConst = 10;
	}
	else {
		speedConst = 1;
	}

	float speed = 0, elevation = 0;
	if (w == 1)
	{
		speed = speedConst * ftime;
	}
	else if (s == 1)
	{
		speed = -speedConst * ftime;
	}
	float yangle = 0;
	if (a == 1)
		yangle = -0.03f * ftime;
	else if (d == 1)
		yangle = 0.03f * ftime;

	if (q == 1) {
		elevation = speedConst * ftime;
	}
	else if (e == 1) {
		elevation = -speedConst * ftime;
	}

	rot.y += yangle;
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 dir = glm::vec4(0.0f, elevation, speed, 0.0f);
	dir = dir * R;
	pos += glm::vec3(dir.x, dir.y, dir.z);

	glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
	return R * T;
}

void OldCamera::update(GLFWwindow* window, float dt) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
	{
		w = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) 
	{
		w = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
	{
		s = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) 
	{
		s = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		a = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
	{
		a = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		d = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
	{
		d = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		q = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
	{
		q = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		e = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
	{
		e = 0;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		shift = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		shift = 0;
	}

	update(-dt);
}

void FPcamera::setCamera(GLFWwindow* window) {
	auto View = make_shared<MatrixStack>();

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
		pos += viewDir*elapsedTime*finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		pos -= viewDir*elapsedTime*finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		pos += rightDir*elapsedTime*finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		pos -= rightDir * elapsedTime * finalmult;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		pos += upDir * elapsedTime * finalmult * 100.0f;
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		pos -= upDir * elapsedTime * finalmult * 100.0f;
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
glm::mat4 FPcamera::getPerspective(float aspect){
	//This is terrible practice, fix this later
	this->aspect = aspect;
	return perspective(glm::radians(fov), aspect, near, far);
}