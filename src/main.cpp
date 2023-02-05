#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <myinclude/shader.h>
#include <myinclude/camera.h>
#include <myinclude/model.h>
#include <myinclude/VAO.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow*, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Camera& camera);

unsigned int loadImageToTexture(const char* imagePath);
void initLights(Shader& shader, Camera& camera);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float deltaTime = 0.0f; // ��ǰ֡����Ⱦ��һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

bool firstMouse = true;
float lastX = float(SCR_WIDTH) / 2.0f;
float lastY = float(SCR_HEIGHT) / 2.0f;

glm::vec3 lightPos(1.7f, 1.0f, 0.3f); // ���
glm::vec3 lightColor(0.8f, 0.3f, 0.1f);	 // ���ɫ
glm::vec3 lightDir(-1.0f, -1.0f, 1.0f); // ƽ�й�

int main()
{
	//---����opengl---//

	// ��ʼ��glfw����glfwTerminate���
	glfwInit();
	// ��ʼ��OPENGL�汾�š���
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// �������� x,x,x,ȫ��
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "importModel", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ָ������Ϊ��Ҫ�����
	glfwMakeContextCurrent(window);
	// �趨����������Ӧ��ק
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// �趨�������
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	// ���� GLAD ʹopengl��������
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// �ڴ����ڵĿ��ӷ�Χ
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


	//---����---//

	// ��������
	float vertices[] = {
		// λ��				  // ��������	   // ����
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	};

	// �����λ
	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// ����BUFFER
	std::vector<float> num(std::begin(vertices), std::end(vertices));
	VBO VBO(num);

	VAO boxVAO;
	boxVAO.bind();
	boxVAO.linkAttrib(VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	boxVAO.linkAttrib(VBO, 1, 2, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	boxVAO.linkAttrib(VBO, 2, 3, GL_FLOAT, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	boxVAO.unBind();
	
	VAO lightVAO;
	lightVAO.bind();
	lightVAO.linkAttrib(VBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	lightVAO.unBind();

	// �������
	stbi_set_flip_vertically_on_load(true);
	unsigned int diffuseMap = loadImageToTexture("resources/textures/container2.png");
	unsigned int specularMap = loadImageToTexture("resources/textures/container2_specular.png");
	//unsigned int normalMap = loadImageToTexture("resources/textures/container2_normal.png");

	// ����shader
	Shader boxShader("resources/shaders/vertex.vert", "resources/shaders/fragment.frag");
	Shader lightShader("resources/shaders/lightVert.vert", "resources/shaders/lightFrag.frag");
	// ָ��texture��Ӧ������Ԫ
	boxShader.use();

	boxShader.setInt("material.diffuse", 0);
	boxShader.setInt("material.specular", 1);
	boxShader.setInt("material.normal", 2);

	std::string backpackPath = "resources/models/backpack/backpack.obj";
	std::string housePath = "resources/models/forest_house/forest_house.fbx";

	// ��stbi�������·�ת֮��
	Shader houseShader("resources/models/backpack/backpack.vert", "resources/models/backpack/backpack.frag");
	Model houseModel(housePath.c_str(), eNULL);

	//Shader backpackShader("resources/models/backpack/backpack.vert", "resources/models/backpack/backpack.frag");
	//Model backpackModel(backpackPath.c_str());

	Camera camera(window, (float)SCR_WIDTH, (float)SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

	glEnable(GL_DEPTH_TEST);

	// ʹ����ѭ����Ӧ�¼�
	while (!glfwWindowShouldClose(window))
	{
		processInput(window, camera);

		camera.processInput(window);

		// �� ColorBuffer �趨�����
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// model�任-view�任-projection�任 / ģ�ͱ任-��ͷ�任-ͶӰ�任
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();

		// ����
		boxShader.use();
		boxVAO.bind();

		// ��ǰ�ȼ���
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		initLights(boxShader, camera);
		boxShader.setMat4("view", view);
		boxShader.setMat4("projection", projection);

		for (unsigned int i = 0; i < 10;i++)
		{
			glm::mat4 model;
			glm::mat4 translate;
			glm::mat4 rotate;

			translate = glm::translate(translate, cubePositions[i]);
			float angle = 20.0f * i;
			rotate = glm::toMat4(glm::angleAxis(glm::radians(angle + (float)glfwGetTime() * 20), glm::normalize(glm::vec3(1.0f, 0.3f, 0.5f))));

			model = translate * rotate;
			boxShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		boxVAO.unBind();

		lightShader.use();
		lightVAO.bind();

		lightShader.setVec3("lightColor", lightColor);
		glm::mat4 model;
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.25f));
		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		lightVAO.unBind();

		houseShader.use();
		initLights(houseShader, camera);
		houseModel.scale(glm::vec3(0.2f));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		houseModel.Draw(houseShader, camera);
		glDisable(GL_BLEND);

		// �л� back �� front buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	boxVAO.Delete();
	lightVAO.Delete();
	VBO.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void processInput(GLFWwindow* window, Camera& camera)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		camera.enableOp = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		camera.enableOp = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	// ����һ�ν��봰��
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // y������С�ϴ�

	lastX = xpos;
	lastY = ypos;

	//camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//camera.processMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadImageToTexture(const char* imagePath)
{
	unsigned int texID;
	glGenTextures(1, &texID);

	// ����ͼ��
	int width, height, nrChannels;
	unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum colorChannel;
		if (nrChannels == 1)
			colorChannel = GL_RED;
		if (nrChannels == 3)
			colorChannel = GL_RGB;
		if (nrChannels == 4)
			colorChannel = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texID);
		//x,x,��������ʲôͨ����x,x,x,Դ�ļ���ʲôͨ��
		glTexImage2D(GL_TEXTURE_2D, 0, colorChannel, width, height, 0, colorChannel, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// ���û��ơ����˷�ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	return texID;
}

void initLights(Shader& shader, Camera& camera)
{
	shader.setBool("enableLight", true);

	shader.setVec3("viewPos", camera.position);

	shader.setVec3("material.specular", glm::vec3(1.0f));
	shader.setFloat("material.shininess", 128.0f);

	// dirlight
	shader.setVec3("dirLight.direction", lightDir);
	shader.setVec3("dirLight.ambient", glm::vec3(0.2f));
	shader.setVec3("dirLight.diffuse", glm::vec3(0.7f));
	shader.setVec3("dirLight.specular", glm::vec3(0.5f));
	shader.setFloat("dirLight.constant", 1.0f);
	shader.setFloat("dirLight.linear", 0.09f);
	shader.setFloat("dirLight.quadratic", 0.032f);

	// pointlight01
	shader.setVec3("light.position", lightPos);
	shader.setVec3("light.ambient", glm::vec3(0.2f));
	shader.setVec3("light.diffuse", lightColor);
	shader.setVec3("light.specular", glm::vec3(1.0f));
	shader.setFloat("light.constant", 1.0f);
	shader.setFloat("light.linear", 0.09f);
	shader.setFloat("light.quadratic", 0.032f);
}