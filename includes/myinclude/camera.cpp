#include "camera.h"

static Camera* camInst;

Camera::Camera
(
	GLFWwindow* window,
	int scrWidth,int scrHeight,
	glm::vec3 position,
	glm::vec3 up,
	float yaw,float pitch
) :
	width(scrWidth), 
	height(scrHeight),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	movementSpeed(SPEED),
	mouseSensitivity(SENSITIVITY),
	fov(FOV),
	isDashing(false),
	isFPSController(false),
	nearPlane(0.1f),
	farPlane(100.0f)
{
	camInst = this;

	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;

	updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(fov), (float)width / height, nearPlane, farPlane);
}

void Camera::processKeyboard(GLFWwindow* window, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		velocity *= 3;
	if (glfwGetKey(window, GLFW_KEY_W))
		position += front * velocity;
	if (glfwGetKey(window, GLFW_KEY_S))
		position -= front * velocity;
	if (glfwGetKey(window, GLFW_KEY_A))
		position -= right * velocity;
	if (glfwGetKey(window, GLFW_KEY_D))
		position += right * velocity;
	if (glfwGetKey(window, GLFW_KEY_E))
		position += up * velocity;
	if (glfwGetKey(window, GLFW_KEY_Q))
		position -= up * velocity;
	if (isFPSController)
		position.y = 0.0f;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		if (fov > 10)
			fov -= deltaTime * 300.0;
		if (fov <= 10)
			fov = 10;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		if (fov < FOV)
			fov += deltaTime * 600.0;
		if (fov >= FOV)
			fov = FOV;
	}
}

void Camera::processMouseMovement(GLFWwindow* window, GLboolean constrainPitch)
{
	double xpos;
	double ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// 鼠标第一次进入窗口
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // y坐标下小上大

	lastX = xpos;
	lastY = ypos;

	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::processMouseScroll(GLFWwindow* window, float yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 100.0f)
		fov = 100.0f;
}

void Camera::processInput(GLFWwindow* window)
{
	// 第一次点击前不操作
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && firstMouse)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}
	// 第一次点击后可以操作
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (firstMouse)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, width / 2.0, height / 2.0);
			firstMouse = false;
		}
	}
	if (!firstMouse && enableOp)
	{
		updateDeltaTime();

		processKeyboard(window, deltaTime);

		processMouseMovement(window);

		glfwSetScrollCallback(window, scrollCallback);
	}
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(front, worldUp));
	this->up = glm::normalize(glm::cross(right, front));
}

void Camera::updateDeltaTime()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camInst->processMouseScroll(window, static_cast<float>(yoffset));
};