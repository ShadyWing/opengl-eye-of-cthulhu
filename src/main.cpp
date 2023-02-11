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
unsigned int loadTexture(const char* imagePath);
unsigned int loadCubemap(std::vector<std::string>& faces);
void initLights(Shader& shader, Camera& camera);

// 窗口设置
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// 帧数监测
float deltaTime = 0.0f; // 当前帧与渲染上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// 鼠标设置
bool firstMouse = true;
float lastX = float(SCR_WIDTH) / 2.0f;
float lastY = float(SCR_HEIGHT) / 2.0f;

// 光源设置
glm::vec3 lightPos(1.7f, 1.0f, 0.3f); // 点光
glm::vec3 lightColor(1.0f, 0.5f, 0.2f);	 // 点光色
glm::vec3 lightDir(-1.0f, -1.0f, 1.0f); // 平行光

// 盒子设置
glm::vec3 boxPosition(-3.0f, 0.0f, 0.0f);

int main()
{
	//---设置opengl---//

	// 初始化glfw，和glfwTerminate配对
	glfwInit();
	// 初始化OPENGL版本号、包
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 创建窗口 x,x,x,全屏
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "importModel", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 指定窗口为主要活动窗口
	glfwMakeContextCurrent(window);
	// 设定窗口内容适应拖拽
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// 设定鼠标输入
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	// 加载 GLAD 使opengl正常工作
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// 在窗口内的可视范围
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


	//---绘制设置---//

	// 绘制内容
	float boxVertices[] = {
		// 位置				  // 法向			   // 纹理坐标
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  	// Bottom-left
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  	// top-right
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  	// bottom-right   
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  	// top-right
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  	// bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  	// top-left
		// Front face		  
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  	// bottom-left
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  	// bottom-right
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  	// top-right
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  	// top-right
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  	// top-left
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  	// bottom-left
		// Left face		  
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,		// top-right
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,		// top-left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,		// bottom-left
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,		// bottom-left
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,		// bottom-right
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,		// top-right
		// Right face		  
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  	// top-left
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  	// bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  	// top-right      
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  	// bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  	// top-left
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  	// bottom-left    
		// Bottom face		  
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  	// top-right
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  	// top-left
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  	// bottom-left
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  	// bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  	// bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  	// top-right
		// Top face		  
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  	// top-left
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  	// bottom-right
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  	// top-right     
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  	// bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  	// top-left
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  	// bottom-left    
	};
	float pointLightVertices[] = {
		// 位置				  
		// Back face
		-0.5f, -0.5f, -0.5f,  	// Bottom-left
		 0.5f,  0.5f, -0.5f,  	// top-right
		 0.5f, -0.5f, -0.5f,  	// bottom-right   
		 0.5f,  0.5f, -0.5f,  	// top-right
		-0.5f, -0.5f, -0.5f,  	// bottom-left
		-0.5f,  0.5f, -0.5f,  	// top-left
		// Front face		  
		-0.5f, -0.5f,  0.5f,  	// bottom-left
		 0.5f, -0.5f,  0.5f,  	// bottom-right
		 0.5f,  0.5f,  0.5f,  	// top-right
		 0.5f,  0.5f,  0.5f,  	// top-right
		-0.5f,  0.5f,  0.5f,  	// top-left
		-0.5f, -0.5f,  0.5f,  	// bottom-left
		// Left face		  
		-0.5f,  0.5f,  0.5f,  	// top-right
		-0.5f,  0.5f, -0.5f,  	// top-left
		-0.5f, -0.5f, -0.5f,  	// bottom-left
		-0.5f, -0.5f, -0.5f,  	// bottom-left
		-0.5f, -0.5f,  0.5f,  	// bottom-right
		-0.5f,  0.5f,  0.5f,  	// top-right
		// Right face		  
		 0.5f,  0.5f,  0.5f,  	// top-left
		 0.5f, -0.5f, -0.5f,  	// bottom-right
		 0.5f,  0.5f, -0.5f,  	// top-right      
		 0.5f, -0.5f, -0.5f,  	// bottom-right
		 0.5f,  0.5f,  0.5f,  	// top-left
		 0.5f, -0.5f,  0.5f,  	// bottom-left    
		 // Bottom face		  
		-0.5f, -0.5f, -0.5f,  	// top-right
		 0.5f, -0.5f, -0.5f,  	// top-left
		 0.5f, -0.5f,  0.5f,  	// bottom-left
		 0.5f, -0.5f,  0.5f,  	// bottom-left
		-0.5f, -0.5f,  0.5f,  	// bottom-right
		-0.5f, -0.5f, -0.5f,  	// top-right
		 // Top face		  
		-0.5f,  0.5f, -0.5f,  	// top-left
		 0.5f,  0.5f,  0.5f,  	// bottom-right
		 0.5f,  0.5f, -0.5f,  	// top-right     
		 0.5f,  0.5f,  0.5f,  	// bottom-right
		-0.5f,  0.5f, -0.5f,  	// top-left
		-0.5f,  0.5f,  0.5f,  	// bottom-left    
	};
	float screenVertices[] = {
		// 标准化屏幕坐标	// texcoord
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	
	// 盒子VAO
	VBO boxVBO(std::vector<float>(std::begin(boxVertices), std::end(boxVertices)));
	VAO boxVAO;
	boxVAO.bind();
	boxVAO.linkAttrib(boxVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	boxVAO.linkAttrib(boxVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	boxVAO.linkAttrib(boxVBO, 2, 3, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	boxVAO.unBind();
	// 光源VAO
	VBO lightVBO(std::vector<float>(std::begin(pointLightVertices), std::end(pointLightVertices)));
	VAO lightVAO;
	lightVAO.bind();
	lightVAO.linkAttrib(boxVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	lightVAO.unBind();
	// 屏幕VAO
	VBO screenVBO(std::vector<float>(std::begin(screenVertices), std::end(screenVertices)));
	VAO screenVAO;
	screenVAO.bind();
	screenVAO.linkAttrib(screenVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
	screenVAO.linkAttrib(screenVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2*sizeof(float)));
	screenVAO.unBind();
	// 天空盒VAO
	VBO skyboxVBO(std::vector<float>(std::begin(skyboxVertices), std::end(skyboxVertices)));
	VAO skyboxVAO;
	skyboxVAO.bind();
	skyboxVAO.linkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	skyboxVAO.unBind();
	
	// Texture path
	std::string containerDiffPath = "resources/textures/container2.png";
	std::string containerSpecPath = "resources/textures/container2_specular.png";
	std::vector<std::string> skyboxDiffPath{
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/front.jpg",
		"resources/textures/skybox/back.jpg"
	};
	// Shader path
	std::string vertPath = "resources/shaders/vertex.vert";
	std::string corePath = "resources/shaders/core.vert";
	std::string skyboxVertPath = "resources/shaders/skybox.vert";
	std::string blurPath = "resources/shaders/postprocess/blur.frag";
	std::string sharpenPath = "resources/shaders/postprocess/sharpen.frag";
	std::string unlitOpaquePath = "resources/shaders/unlit/unlitOpaque.frag";
	std::string unlitReflectPath = "resources/shaders/unlit/unlitReflect.frag";
	std::string skyboxFragPath = "resources/shaders/unlit/skybox.frag";
	std::string litOpaquePath = "resources/shaders/lit/opaque.frag";
	std::string litTransparentPath = "resources/shaders/lit/transparent.frag";
	// Model path
	std::string housePath = "resources/models/forest_house/forest_house.fbx";
	std::string backpackPath = "resources/models/backpack/backpack.obj";

	// 纹理对象
	stbi_set_flip_vertically_on_load(true);
	unsigned int diffuseMap = loadTexture(containerDiffPath.c_str());
	unsigned int specularMap = loadTexture(containerSpecPath.c_str());
	stbi_set_flip_vertically_on_load(false);
	unsigned int skyboxMap = loadCubemap(skyboxDiffPath);

	// 设置shader
	// ---------
	Shader boxShader(vertPath.c_str(), litOpaquePath.c_str());
	Shader reflectShader(vertPath.c_str(), unlitReflectPath.c_str());
	Shader lightShader(vertPath.c_str(), unlitOpaquePath.c_str());
	Shader screenShader(corePath.c_str(), sharpenPath.c_str());
	Shader skyboxShader(skyboxVertPath.c_str(), skyboxFragPath.c_str());
	// boxShader
	boxShader.bind();
	boxShader.setInt("material.texture_diffuse1", 0);
	boxShader.setInt("material.texture_specular1", 1);
	boxShader.unBind();
	// refelctShader
	reflectShader.bind();
	reflectShader.setInt("skybox", 0);
	reflectShader.unBind();
	// lighShader
	lightShader.bind();
	lightShader.setVec3("material.baseColor", lightColor);
	lightShader.unBind();
	// screenShader
	screenShader.bind();
	screenShader.setInt("iScreenTexture", 0);
	screenShader.unBind();
	// skyboxShader
	skyboxShader.bind();
	skyboxShader.setInt("skybox", 0);
	skyboxShader.unBind();

	// 导入模型
	// -------
	Shader houseShader(vertPath.c_str(), litTransparentPath.c_str());
	Model houseModel(housePath.c_str());
	//Shader backpackShader(vertPath.c_str(), litOpaquePath.c_str());
	//Model backpackModel(backpackPath.c_str(), eNULL);

	// 设置Framebuffer
	// --------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// ColorTexture Attachment
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	// Renderbuffer for depth and stencil
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	// 检查Attachment完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 生成相机
	Camera camera(window, (float)SCR_WIDTH, (float)SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

	// OPENGL渲染设置
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// 渲染循环
	// ---------------
	while (!glfwWindowShouldClose(window))
	{
		// 响应输入
		// -------
		processInput(window, camera);
		camera.processInput(window);


		// 渲染
		// ----
		glClearColor(0.07f, 0.07f, 0.07f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 变换矩阵
		glm::mat4 projection = camera.getProjectionMatrix();
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 model;

		// 进行后处理的渲染
		// -------------
		// 绑定Frambuffer进行渲染
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(lightColor.r, lightColor.g, lightColor.b, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 绘制箱子
		boxShader.bind();
		initLights(boxShader, camera);
		boxShader.setMat4("projection", projection);
		boxShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, boxPosition);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 20), glm::vec3(1.0f, 0.3f, 0.5f));
		boxShader.setMat4("model", model);
		// 绘制前绑定
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		boxVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		boxVAO.unBind();
		boxShader.unBind();
		
		// 绘制反光箱子
		reflectShader.bind();
		reflectShader.setVec3("cameraPos", camera.position);
		reflectShader.setMat4("projection", projection);
		reflectShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		reflectShader.setMat4("model", model);
		// 绘制前绑定
		boxVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		boxVAO.unBind();
		reflectShader.unBind();

		// 绘制房子
		houseShader.bind();
		initLights(houseShader, camera);
		houseModel.scale(glm::vec3(0.2f));
		houseModel.Draw(houseShader, camera);
		houseShader.unBind();

		// 绘制背包
		//backpackShader.bind();
		//initLights(backpackShader, camera);
		//backpackModel.translate(glm::vec3(3.0f, 0.f, 0.f));
		//backpackModel.scale(glm::vec3(0.5f));
		//backpackModel.Draw(backpackShader, camera);
		//backpackShader.unBind();

		// 渲染光源
		lightShader.bind();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.25f));
		lightShader.setMat4("model", model);
		lightVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		lightVAO.unBind();
		lightShader.unBind();

		// 最后 绘制天空盒
		glDepthFunc(GL_LEQUAL);
		skyboxShader.bind();
		skyboxShader.setMat4("projection", projection);
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMap);
		skyboxVAO.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		skyboxVAO.unBind();
		glDepthFunc(GL_LESS);

		// 渲染纹理Colorbuffer
		// -------------
		// 返回默认Frambuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		screenShader.bind();
		screenVAO.bind();
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		screenVAO.unBind();
		screenShader.unBind();
		glDisable(GL_BLEND);


		// 交换buffer 响应窗口事件
		// -------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// 删除OBJECT
	boxVAO.Delete();
	lightVAO.Delete();
	screenVAO.Delete();
	skyboxVAO.Delete();

	boxVBO.Delete();
	lightVBO.Delete();
	screenVBO.Delete();
	skyboxVBO.Delete();

	// 关闭窗口
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
	
	// 鼠标第一次进入窗口
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // y坐标下小上大
	
	lastX = xpos;
	lastY = ypos;

	//camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//camera.processMouseScroll(static_cast<float>(yoffset));
}

void initLights(Shader& shader, Camera& camera)
{
	shader.setVec3("viewPos", camera.position);

	shader.setFloat("material.shininess", 128.0f);

	// dirlight
	// --------
	shader.setVec3("dirLight.direction", lightDir);
	shader.setVec3("dirLight.ambient", glm::vec3(0.2f));
	shader.setVec3("dirLight.diffuse", glm::vec3(0.7f));
	shader.setVec3("dirLight.specular", glm::vec3(0.5f));
	// 衰弱
	shader.setFloat("dirLight.constant", 1.0f);
	shader.setFloat("dirLight.linear", 0.09f);
	shader.setFloat("dirLight.quadratic", 0.032f);

	// pointlight01
	// ------------
	shader.setVec3("light.position", lightPos);
	shader.setVec3("light.ambient", glm::vec3(0.2f));
	shader.setVec3("light.diffuse", lightColor);
	shader.setVec3("light.specular", glm::vec3(1.0f));
	// 衰弱
	shader.setFloat("light.constant", 1.0f);
	shader.setFloat("light.linear", 0.09f);
	shader.setFloat("light.quadratic", 0.032f);
}

unsigned int loadTexture(const char* imagePath)
{
	unsigned int texID;
	glGenTextures(1, &texID);

	// 纹理图像
	int width, height, nrChannels;
	unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum colorChannel;
		switch (nrChannels) {
		case 1:
			colorChannel = GL_RED; break;
		case 3:
			colorChannel = GL_RGB; break;
		case 4:
			colorChannel = GL_RGBA; break;
		default:break;
		}

		glBindTexture(GL_TEXTURE_2D, texID);
		//x,x,读进来是什么通道，x,x,x,源文件是什么通道
		glTexImage2D(GL_TEXTURE_2D, 0, colorChannel, width, height, 0, colorChannel, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// 设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

unsigned int loadCubemap(std::vector<std::string>& faces)
{
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	// 纹理图像
	int width, height, nrChannels;
	for (unsigned int i = 0;i < faces.size();i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubmap texture failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}

	// 设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return texID;
}