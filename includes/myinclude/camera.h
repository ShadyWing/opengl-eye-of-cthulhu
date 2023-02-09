#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera
{
public:
	bool enableOp = true;

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	int width;
	int height;
	float fov;
	float nearPlane;
	float farPlane;

	bool isDashing;
	bool isFPSController;

	float yaw;
	float pitch;

	float movementSpeed;
	float mouseSensitivity;

	Camera(
		GLFWwindow* window,
		int scrWidth, int scrHeight,
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW, float pitch = PITCH
	);
	
	glm::mat4 getViewMatrix();

	glm::mat4 getProjectionMatrix();

	void processKeyboard(GLFWwindow* window, float deltaTime);

	void processMouseMovement(GLFWwindow* window, GLboolean constrainPitch = true);

	void processMouseScroll(GLFWwindow* window, float yoffset);

	// 整合键盘、鼠标移动、鼠标滚轮
	// -----------------------
	void processInput(GLFWwindow* window);

private:
	float deltaTime = 0.0f; // 当前帧与渲染上一帧的时间差
	float lastFrame = 0.0f; // 上一帧的时间

	bool firstMouse = true;
	float lastX = float(width) / 2.0f;
	float lastY = float(height) / 2.0f;

	void updateCameraVectors();
	
	void updateDeltaTime();
};

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

#endif