#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <myinclude/shader.h>
#include <stb_image.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow*, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;



int main()
{
	// ��ʼ��glfw����glfwTerminate���
	glfwInit();

	// ��ʼ��OPENGL�汾�š���
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// �������� ���ĸ�����Ϊȫ��
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "firstWindow", NULL, NULL);
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

	// ���� GLAD ʹopengl��������
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// �ڴ����ڵĿ��ӷ�Χ
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);



	// ����shader
	Shader ourShader("Shaders/vertex.vs", "Shaders/fragment.fs");
	
	// ��������
	float vertices[] = {
		//λ��			//��ɫ
		-0.5f,-0.5f,0.0f,1.0f,0.0f,0.0f,
		0.5f,-0.5f,0.0f,0.0f,1.0f,0.0f,
		0.0f,0.5f,0.0f,0.0f,0.0f,1.0f
	};

	// ����BUFFER�Ͷ�������
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// ���Ӷ�������		 ��0��attribute��3�����ݣ�float���ͣ�x����������ʼƫ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);// ��0��vAttrib����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);// ��1��vAttrib����

	glEnable(GL_DEPTH_TEST);

	// ����
	int width , height , nrChannels;
	unsigned char* data = stbi_load("Resources/Textures/container.jpg", &width, &height, &nrChannels, 0);
	
	unsigned int texture;
	glGenTextures(1 , &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// ʹ����ѭ����Ӧ�¼�
	while ( !glfwWindowShouldClose ( window ) )
	{
		processInput(window);
	
		// �� ColorBuffer �趨�����
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ����uniform����
		//int vertexColorLocation = glGetUniformLocation(ourShader.ID, "horizonOffset");
		//glUniform1f(vertexColorLocation, 0.8f);
		
		// ����
		ourShader.use();
		glBindVertexArray ( VAO );
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// �л� back �� front buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
