#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow*, int width, int height);
void processInput(GLFWwindow* window);
void fprintShaderInfo(int shader);
void fprintShaderProgramInfo(int shaderProgram);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const GLchar* vertexShaderSource = 
"#version 460 core\n\
layout (location = 0) in vec3 aPos;\n\
layout (location = 1) in vec3 aColor;\n\
out vec3 ourColor;\n\
void main()\n\
{\n\
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
	ourColor = aColor;\n\
}\0";

const char* fragmentShaderSource = 
"#version 460 core\n\
in vec3 ourColor;\n\
out vec4 FragColor;\n\
void main()\n\
{\n\
	FragColor = vec4(ourColor, 1.0);\n\
}\0";



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
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	fprintShaderInfo(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	fprintShaderInfo(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	fprintShaderInfo(shaderProgram);

	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	
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

	// ʹ����ѭ����Ӧ�¼�
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// �� ColorBuffer �趨�����
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ����uniform����
		//float time = glfwGetTime();
		//float greenValue = (sin(time) / 2.0f) + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		
		// ����
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// �л� back �� front buffer
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}
}

void fprintShaderInfo(int shader)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void fprintShaderProgramInfo(int shaderProgram)
{
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
