#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vmath.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

using namespace std;

//���ڴ�С����
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float aspact = (float)4.0 / (float)3.0;

//ָ�������
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// ָ��ʱ�����
float deltaTime = 0.0f;
float lastFrame = 0.0f;


//�������
GLuint vertex_array_object; // == VAO���
GLuint vertex_buffer_object; // == VBO���
GLuint element_buffer_object;//==EBO���
GLuint texture_buffer_object1; // ���������1
GLuint texture_buffer_object2; // ���������2
GLuint texture_buffer_object3; // ���������3

//������ݲ���
std::vector<int> sphereIndices;
const int Y_SEGMENTS = 20;
const int X_SEGMENTS = 20;
const float Radio = 1.0;
const GLfloat  PI = 3.14159265358979323846f;


vector<float> Vertices;


void initial(void)
{
	//�������嶥���������Ƭ�ļ���
	// ������Ķ���
	for (int y = 0; y <= Y_SEGMENTS; y++)
	{
		for (int x = 0; x <= X_SEGMENTS; x++)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = Radio * std::cos(2 * xSegment * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = Radio * std::sin(2 * xSegment * PI) * std::sin(ySegment * PI);
			Vertices.push_back(xPos);
			Vertices.push_back(yPos);
			Vertices.push_back(zPos);
			Vertices.push_back(xSegment);
			Vertices.push_back(ySegment);
		}
	}

	// ������Ķ���
	for (int i = 0; i < Y_SEGMENTS; i++)
	{
		for (int j = 0; j < X_SEGMENTS; j++)
		{

			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
			sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
			sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
		}
	}

	// ��
	glGenVertexArrays(1, &vertex_array_object);
	glGenBuffers(1, &vertex_buffer_object);
	//���ɲ��������VAO��VBO
	glBindVertexArray(vertex_array_object);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	// ���������ݰ�����ǰĬ�ϵĻ�����
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(float) * 5, &Vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &element_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

	// ���ö�������ָ��
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	

	//������������
	//����
	glGenTextures(1, &texture_buffer_object1);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object1);

	//ָ������Ĳ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//��������
	int width, height, nrchannels;//������ͨ����
	//stbi_set_flip_vertically_on_load(true);
	//��������ͼƬwall.jpg����htmdb.png
	unsigned char* data = stbi_load("res/earth.bmp", &width, &height, &nrchannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//����Mipmap����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);//�ͷ���Դ

	//̫��
	glGenTextures(1, &texture_buffer_object2);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object2);

	//ָ������Ĳ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//��������
	//stbi_set_flip_vertically_on_load(true);
	//��������ͼƬwall.jpg����htmdb.png
	data = stbi_load("res/sun.bmp", &width, &height, &nrchannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//����Mipmap����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);//�ͷ���Դ

	//����
	glGenTextures(1, &texture_buffer_object3);
	glBindTexture(GL_TEXTURE_2D, texture_buffer_object3);

	//ָ������Ĳ���
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//��������
	//stbi_set_flip_vertically_on_load(true);
	//��������ͼƬwall.jpg����htmdb.png
	data = stbi_load("res/moon.bmp", &width, &height, &nrchannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//����Mipmap����
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);//�ͷ���Դ

	//�趨�����������
	glPointSize(15);//���õ�Ĵ�С
	glLineWidth(5);//�����߿�

	//�����޳�����
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//������Ȳ���
	glEnable(GL_DEPTH_TEST);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_L:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case GLFW_KEY_F:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case GLFW_KEY_B:
		glEnable(GL_CULL_FACE);    //�򿪱����޳�
		glCullFace(GL_BACK);          //�޳�����εı���
		break;
	case GLFW_KEY_C:
		glDisable(GL_CULL_FACE);     //�رձ����޳�
		break;
	default:
		break;
	}
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

//�����ֻص�����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//����������в���
	camera.ProcessMouseScroll((float)yoffset);
}

// ����ƶ��ص�����
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // y����ϵ���з�ת

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void reshaper(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	if (height == 0)
	{
		aspact = (float)width;
	}
	else
	{
		aspact = (float)width / (float)height;
	}

}

int main()
{
	glfwInit(); // ��ʼ��GLFW

	// OpenGL�汾Ϊ3.3�����ΰ汾�ž���Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// ʹ�ú���ģʽ(������������)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ��������(���ߡ���������)
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SEM_model", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}

	// �����ڵ�����������Ϊ��ǰ�̵߳���������
	glfwMakeContextCurrent(window);

	// ��ʼ��GLAD������OpenGL����ָ���ַ�ĺ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	initial();//��ʼ��

	//���ڴ�С�ı�ʱ����reshaper����
	glfwSetFramebufferSizeCallback(window, reshaper);

	//�������м��̲���ʱ����key_callback����
	glfwSetKeyCallback(window, key_callback);

	//���ص�����
	glfwSetCursorPosCallback(window, mouse_callback);
	//�������ֹ���ʱ���ú���scroll_callback
	glfwSetScrollCallback(window, scroll_callback);

	Shader lightingShader("sphere.vs", "sphere.fs");
	lightingShader.use();
	lightingShader.setInt("texture1", 0);
	float a = 6.0f, b = 3.0f;
	float r = 1.0f;
	while (!glfwWindowShouldClose(window))
	{

		//����ʱ��
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ���봦��
		processInput(window);
		// �����ɫ�������Ȼ�����
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// �������
		lightingShader.use();
		lightingShader.setVec3("light.position", glm::vec3(0.0f,0.0f,0.0f));
		lightingShader.setVec3("light.direction", camera.Front);
		lightingShader.setVec3("viewPos", camera.Position);

		//��Դ����
		lightingShader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("light.specular", 0.05f, 0.05f, 0.05f);
		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.045f);
		lightingShader.setFloat("light.quadratic", 0.0075f);

		//�����
		lightingShader.setFloat("shininess", 2.0f);


		//����
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texture_buffer_object1);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		float currentTime = (float)glfwGetTime();

		glBindVertexArray(vertex_array_object);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 scale_earth = glm::mat4(1.0f);
		glm::mat4 trans_revolution_earth = glm::mat4(1.0f);
		glm::mat4 rot_tilt = glm::mat4(1.0f);
		glm::mat4 rot_self = glm::mat4(1.0f);
		scale_earth = glm::scale(scale_earth, glm::vec3(0.4, 0.4, 0.4));
		rot_tilt = glm::rotate(rot_tilt,  glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		trans_revolution_earth = glm::translate(trans_revolution_earth, glm::vec3(-a * std::cos(PI/4*currentTime), 0.0f, b * std::sin(PI/4 * currentTime)));
		
		rot_self = glm::rotate(rot_self,  -365 * currentTime* glm::radians(45.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
		model = trans_revolution_earth *rot_self* rot_tilt * scale_earth;
		
		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		
	    //̫��
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object2);

		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::rotate(model, currentTime * glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		
		//����
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_buffer_object3);
		
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		glBindVertexArray(vertex_array_object);
		glm::mat4 scale_moon = glm::mat4(1.0f);
		glm::mat4 trans3 = glm::mat4(1.0f);
		glm::mat4 trans_revolution_moon = glm::mat4(1.0f);
		scale_moon = glm::scale(scale_moon, glm::vec3(0.1, 0.1, 0.1));
		trans3 = glm::translate(trans3, glm::vec3(a, 0.0f, 0.0f));
		trans_revolution_moon = glm::translate(trans_revolution_moon, glm::vec3(r * std::cos(PI *7 * currentTime), 0.0f, r * std::sin(PI * 7 * currentTime)));
		rot_tilt = glm::mat4(1.0f);
		rot_tilt = glm::rotate(rot_tilt, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = trans_revolution_earth * rot_tilt *  trans_revolution_moon * rot_self * scale_moon;

		lightingShader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// ����ɾ��VAO��VBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
