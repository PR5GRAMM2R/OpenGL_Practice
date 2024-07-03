#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // �ؽ��� �̹����� �������� ���� ����ϴ� ���̺귯��
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;	// ������ �����Ӱ� ���� ������ ������ �ð�
float lastFrame = 0.0f; // ������ �������� �ð�

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);	// ī�޶��� ��ġ
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);	// ī�޶��� ���� ����
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);	// ���� ����

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

int main()
{
    glfwInit();		//  GLFW �� �ʱ�ȭ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// OpenGL ������ 3.* �̹Ƿ� MAJOR �� MINOR ��� 3���� ����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);	// â�� ũ��� �̸� ���� �����Ͽ� ����

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;	// â�� ����� �������� ���� ��� ���� ���
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);		// Double Buffer�� ����� Window�� ����ϴ� �Լ��̸�, ��ϵǸ� Thread�� �����Ǿ� ����

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {  
		std::cout << "Failed to initialize GLAD" << std::endl;	// GLAD �� ����� �ʱ�ȭ���� ���� ��� ���� ���
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	// â�� ũ�Ⱑ ����� �� ����� â�� ����, ���� �����ӿ� ���� framebuffer_size_callback ȣ��

	glfwSetCursorPosCallback(window, mouse_callback);	// ���콺�� ��ġ �Է��� �ݹ��Լ��� ���� �޾� ��

	glfwSetScrollCallback(window, scroll_callback);		// ���콺�� ��ũ�� �Է��� �ݹ��Լ��� ���� �޾� ��

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shaders/shader.vs", "shaders/shader.fs"); // you can name your shader files however you like


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


	float vertices[] = {	// ���� (x ��ǥ, y ��ǥ, z ��ǥ) �� ������ <- normalized device coordinates ������ ������
		// Position			  // Texture Coordinates

		// ����
		0.5f,	0.5f,	-0.5f,		1.0f,	1.0f,
		0.5f,	0.5f,	0.5f,		1.0f,	0.0f,
		-0.5f,	0.5f,	0.5f,		0.0f,	0.0f,
		-0.5f,	0.5f,	-0.5f,		0.0f,	1.0f,

		// �ո�
		0.5f,	0.5f,	0.5f,		1.0f,	1.0f,
		0.5f,	-0.5f,	0.5f,		1.0f,	0.0f,
		-0.5f,	-0.5f,	0.5f,		0.0f,	0.0f,
		-0.5f,	0.5f,	0.5f,		0.0f,	1.0f,
		
		// �Ʒ���
		0.5f,	-0.5f,	0.5f,		1.0f,	1.0f,
		0.5f,	-0.5f,	-0.5f,		1.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f,		0.0f,	0.0f,
		-0.5f,	-0.5f,	0.5f,		0.0f,	1.0f,

		// �޸�
		0.5f,	-0.5f,	-0.5f,		1.0f,	1.0f,
		0.5f,	0.5f,	-0.5f,		1.0f,	0.0f,
		-0.5f,	0.5f,	-0.5f,		0.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f,		0.0f,	1.0f,

		// ���ʸ�
		-0.5f,	0.5f,	0.5f,		1.0f,	1.0f,
		-0.5f,	-0.5f,	0.5f,		1.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f,		0.0f,	0.0f,
		-0.5f,	0.5f,	-0.5f,		0.0f,	1.0f,

		// �����ʸ�
		0.5f,	0.5f,	-0.5f,		1.0f,	1.0f,
		0.5f,	-0.5f,	-0.5f,		1.0f,	0.0f,
		0.5f,	-0.5f,	0.5f,		0.0f,	0.0f,
		0.5f,	0.5f,	0.5f,		0.0f,	1.0f
	};

	unsigned int indices[] = {
		// ����
		0,	1,	3,
		1,	2,	3,

		// �ո�
		4,	5,	7,
		5,	6,	7,

		// �Ʒ���
		8,	9,	11,
		9,	10,	11,

		// �޸�
		12,	13,	15,
		13,	14,	15,

		// ���ʸ�
		16,	17,	19,
		17,	18,	19,

		// �����ʸ�
		20,	21,	23,
		21,	22,	23
    };

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);	// ������ ����
	glm::mat4 trans = glm::mat4(1.0f);	// ����� ���� (Column major)
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));	// �̵���ȯ�� ��Ŀ� ����
	vec = trans * vec;
	std::cout << vec.x << vec.y << vec.z << std::endl;

	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));	// ȸ����ȯ�� ��Ŀ� ����
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));	// Ȯ�� �� ��� ��ȯ�� ��Ŀ� ����

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned int VAO;	// vertex �Ӽ� �����͸� ������ �� ���� �� ���� ȣ���ϱ⸸ �ϸ� �ǰ�
						//		������Ʈ�� �׷��� �� ������ �ش� VAO�� ���ε� �ϱ⸸ �ϸ� �ȴٴ� ������ ������ �ִ� VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;	// ���� ���� �������� GPU �޸� �� ������ �� �ִ� VBO (Vertex Buffer Objects) 
						//							<- CPU ���� GPU �� �����͸� ������ �ӵ��� �����⿡ �Ѳ����� ���� ������ �ϱ� ������ ���
	glGenBuffers(1, &VBO);	// VBO ������ ID ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// GL_ARRAY_BUFFER �� Ÿ������ �ϴ� ��� ���۴� VBO �� ����ϰ� �� (Binding Buffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// ���� ���ε��� GL_ARRAY_BUFFER �� �ռ� ������ vertices �迭�� ����.
						//	GL_STREAM_DRAW	: ������ ���� X, ���� ������ ����
						//	GL_STATIC_DRAW	: ������ ���� X, ���� ����
						//	GL_DYNAMIC_DRAW	: �����Ͱ� ���� �����, ���� ����

	unsigned int EBO;	// ������ ����ϴ� ���� �ߺ��� �߻��ϱ⿡ �̷��� �ߺ��� ���ϰ��� ����ϴ� EBO (Element Buffer Objects) <- �ε����� ����
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	// GL_ELEMENT_ARRAY_BUFFER �� ���� Ÿ������ ����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// How to interperet Vertices ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
									// glVertexAttribPointer(vertex �� �Ӽ�, vertex �� ũ��, �������� Ÿ��, ����ȭ ����, vertex ���� ����, ���ۿ��� �����Ͱ� �����ϴ� ��ġ�� offset) ����
	glEnableVertexAttribArray(0);	// glVertexAttribPointer ���� ������ vertex �� ���� �Ӽ��� Ȱ��ȭ

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
									// glVertexAttribPointer(vertex �� �Ӽ�, vertex �� ũ��, �������� Ÿ��, ����ȭ ����, vertex ���� ����, ���ۿ��� �����Ͱ� �����ϴ� ��ġ�� offset) ����
	glEnableVertexAttribArray(1);	// glVertexAttribPointer ���� ������ vertex �� ���� �Ӽ��� Ȱ��ȭ

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////// Textures ///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int width, height, nrChannels;
	unsigned int texture1, texture2;

	stbi_set_flip_vertically_on_load(true);		// �������� �̹��� ����� ���������� ��µǰ� ��

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glGenTextures(1, &texture1);	// �󸶳� ���� �ؽ��ĸ� ������ �������� ���� ����

	//glActiveTexture(GL_TEXTURE0);	// �ؽ��� ������ Ȱ��ȭ <- GL_TEXTURE0 �ؽ��� ������ default �� �׻� Ȱ��ȭ

	glBindTexture(GL_TEXTURE_2D, texture1);	// Ȱ��ȭ�� �ؽ��� ���ֿ� �ش� �ؽ��� (texture ����) �� ���ε�

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// �ؽ��� Wrapping x��(s) �ɼ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// �ؽ��� Wrapping y��(t) �ɼ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// �ؽ��� ��� Filtering �ɼ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// �ؽ��� Ȯ�� Filtering �ɼ�

	unsigned char* data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);	// �ؽ��� �̹��� ���� �ε�

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	// �ؽ��ĸ� ��� Ȱ���� ������ ����
		glGenerateMipmap(GL_TEXTURE_2D);	// �ؽ��Ŀ� mipmap ����
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glGenTextures(1, &texture2);	// �󸶳� ���� �ؽ��ĸ� ������ �������� ���� ����

	//glActiveTexture(GL_TEXTURE0);	// �ؽ��� ������ Ȱ��ȭ <- GL_TEXTURE0 �ؽ��� ������ default �� �׻� Ȱ��ȭ

	glBindTexture(GL_TEXTURE_2D, texture2);	// Ȱ��ȭ�� �ؽ��� ���ֿ� �ش� �ؽ��� (texture ����) �� ���ε�

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// �ؽ��� Wrapping x��(s) �ɼ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// �ؽ��� Wrapping y��(t) �ɼ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// �ؽ��� ��� Filtering �ɼ�
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// �ؽ��� Ȯ�� Filtering �ɼ�

	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);	// �ؽ��� �̹��� ���� �ε�

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	// �ؽ��ĸ� ��� Ȱ���� ������ ����
		glGenerateMipmap(GL_TEXTURE_2D);	// �ؽ��Ŀ� mipmap ����
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	ourShader.setInt("texture2", 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////// Transformation Matrix /////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////// Model Matrix //////////////////////////////////////////////////

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	/////////////////////////////////////////////// View Matrix //////////////////////////////////////////////////

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	/*float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view;
	view = glm::lookAt(glm::vec3(camX, 0.0, camZ), 
						glm::vec3(0.0, 0.0, 0.0), 
						glm::vec3(0.0, 1.0, 0.0));  */

	/////////////////////////////////////////// Projection Matrix ////////////////////////////////////////////////

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// ��� //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glEnable(GL_DEPTH_TEST); // Z-Buffer (���� ����) �� Ȱ��ȭ

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// ���콺�� â �ȿ��� ������ �ʵ��� ����

    // render loop
    // -----------
    
	while (!glfwWindowShouldClose(window)) {	// GLFW �� ����ǵ��� (â�� �ݵ���) ���ù޾Ҵ��� Ȯ��
		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);	// glClearColor ���� ������ �������� �������� �� (�Ͼ������ ���� -> �Ͼ������ ������)

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// �׸� �ﰢ���� ���θ� ä���� ����
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// �׸� �ﰢ���� ���θ� ä��

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();	// ���̴����� ������ shaderProgram �� ����Ͽ� ������

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//model = glm::rotate(model, (float)sin(glfwGetTime()) * glm::radians(5.0f), glm::vec3(0.5f, 1.0f, 0.0f));	/////////////

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Z-Buffer (���� ����) �� ��� �� (�ʱ�ȭ)

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);	// ī�޶��� ��ġ ��ȭ�� ���� ���� �̵�

		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 1000.0f);	// ī�޶��� �þ� ��ȭ

		//float radius = 10.0f;
		//float camX = sin(glfwGetTime()) * radius;
		//float camZ = cos(glfwGetTime()) * radius;
		//glm::mat4 view;									// ī�޶� �ǻ�ü ������ ���� ������ ����. ( lookAt �Լ��� ��� )
		//view = glm::lookAt(glm::vec3(camX, 0.0, camZ),	// ī�޶��� ��ġ
		//					glm::vec3(0.0, 0.0, 0.0),	// �ǻ�ü�� ��ġ
		//					glm::vec3(0.0, 1.0, 0.0));	// ���� ����

		//ourShader.setMat4("model", model);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);

		glBindVertexArray(VAO);

		for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * (pow(i, 1.1) + 1);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::rotate(model, (float)sin(glfwGetTime()) * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
            ourShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	// �ﰢ���� �׸��µ� �� 6���� ������ (�ﰢ�� �� ��) �� �׸� <- EBO ���
		//glBindVertexArray(0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);	// �ﰢ���� �׸��µ� 3���� ������ �׸� <- VAO ���

		glfwSwapBuffers(window);	// �ø�Ŀ�� ���� ���� <- â�� �������� ����ģ �� ���ۿ� �� ���۸� ��ü�� (swap) (Double Buffer Ȱ��)
		glfwPollEvents();	// Ű���峪 ���콺�� �Է��� �־����� Ȯ��
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
	
	float cameraSpeed = 25.0f * deltaTime;	// �� �����Ӹ��� �̵���Ű���� ��ġ (ȯ�渶�� ������ ���� �ð��� �ٸ��� ����)

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)		// Ű���� W, S, A, D �Է¿� ���� ī�޶� �̵�
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // y ��ǥ�� ������ �ؿ������� ���ΰ��� ������ �ݴ�� �ٲߴϴ�.
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw   += xoffset;
	pitch += yoffset;

	if(pitch > 89.0f)
		pitch =  89.0f;
	if(pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(fov >= 1.0f && fov <= 45.0f)
  	fov -= yoffset;
  if(fov <= 1.0f)
  	fov = 1.0f;
  if(fov >= 45.0f)
  	fov = 45.0f;
}