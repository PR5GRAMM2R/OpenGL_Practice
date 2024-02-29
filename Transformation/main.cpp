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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {  
		std::cout << "Failed to initialize GLAD" << std::endl;	// GLAD �� ����� �ʱ�ȭ���� ���� ��� ���� ���
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	// â�� ũ�Ⱑ ����� �� ����� â�� ����, ���� �����ӿ� ���� framebuffer_size_callback ȣ��

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shaders/shader.vs", "shaders/shader.fs"); // you can name your shader files however you like

    float vertices[] = {	// ���� (x ��ǥ, y ��ǥ, z ��ǥ) �� ������ <- normalized device coordinates ������ ������
		// Position				// Colors				// Texture Coordinates
		0.5f	, 0.5f	, 0.0f	, 1.0f	, 0.0f	, 0.0f	, 1.0f	, 1.0f	,	// ����
		0.5f	, -0.5f	, 0.0f	, 0.0f	, 1.0f	, 0.0f	, 1.0f	, 0.0f	,	// ���ϴ�
		-0.5f	, -0.5f	, 0.0f	, 0.0f	, 0.0f	, 1.0f	, 0.0f	, 0.0f	,	// ���ϴ�
		-0.5f	, 0.5f	, 0.0f	, 1.0f	, 1.0f	, 1.0f	, 0.0f	, 1.0f		// �»��
	};

	unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
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

	/*trans = glm::translate(trans, glm::vec3(0.2, -0.2, 0.0));
	trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0, 1.0, 1.0));*/

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

	unsigned int EBO;	// ������ �����ϴ� ���� �ߺ��� �߻��ϱ⿡ ���� �ߺ��� ���ϰ��� ����ϴ� EBO (Element Buffer Objects) <- �ε����� ����
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	// GL_ELEMENT_ARRAY_BUFFER �� ���� Ÿ������ ����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// How to interperet Vertices ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//								// glVertexAttribPointer(vertex �� �Ӽ�, vertex �� ũ��, �������� Ÿ��, ����ȭ ����, vertex ���� ����, ���ۿ��� �����Ͱ� �����ϴ� ��ġ�� offset) ����
	//glEnableVertexAttribArray(0);	// glVertexAttribPointer ���� ������ vertex �� ���� �Ӽ��� Ȱ��ȭ

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
									// glVertexAttribPointer(vertex �� �Ӽ�, vertex �� ũ��, �������� Ÿ��, ����ȭ ����, vertex ���� ����, ���ۿ��� �����Ͱ� �����ϴ� ��ġ�� offset) ����
	glEnableVertexAttribArray(0);	// glVertexAttribPointer ���� ������ vertex �� ���� �Ӽ��� Ȱ��ȭ

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
									// glVertexAttribPointer(vertex �� �Ӽ�, vertex �� ũ��, �������� Ÿ��, ����ȭ ����, vertex ���� ����, ���ۿ��� �����Ͱ� �����ϴ� ��ġ�� offset) ����
	glEnableVertexAttribArray(1);	// glVertexAttribPointer ���� ������ vertex �� ���� �Ӽ��� Ȱ��ȭ

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
									// glVertexAttribPointer(vertex �� �Ӽ�, vertex �� ũ��, �������� Ÿ��, ����ȭ ����, vertex ���� ����, ���ۿ��� �����Ͱ� �����ϴ� ��ġ�� offset) ����
	glEnableVertexAttribArray(2);	// glVertexAttribPointer ���� ������ vertex �� ���� �Ӽ��� Ȱ��ȭ

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////// Textures ///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int width, height, nrChannels;
	unsigned int texture1, texture2;

	stbi_set_flip_vertically_on_load(true);

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
	////////////////////////////////////////////////// ��� //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // render loop
    // -----------
    
	while (!glfwWindowShouldClose(window)) {	// GLFW �� ����ǵ��� (â�� �ݵ���) ���ù޾Ҵ��� Ȯ��
		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);	// glClearColor ���� ������ �������� �������� �� (�Ͼ������ ���� -> �Ͼ������ ������)

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// �׸� �ﰢ���� ���θ� ä���� ����
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// �׸� �ﰢ���� ���θ� ä��

		//float timeValue = glfwGetTime();	// �� �ð��� ��ȯ
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();	// ���̴����� ������ shaderProgram �� ����Ͽ� ������


		unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");	// transform ��� �̸��� ���� uniform ������ Shader Program ���� ã��
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));	// uniform ������ ����� �����ϴ� �Լ��� ���
																				//	(uniform ������ ��ġ, ������ ����� ����, ��ġ��ķ� ��ȯ(transpose)�� ���� ����, ������ ����� data)

		//trans = glm::rotate(trans, glm::radians(1.0f), glm::vec3(1.0, 0.0, 0.0));
		//trans = glm::rotate(trans, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
		//trans = glm::rotate(trans, glm::radians(1.0f), glm::vec3(0.0, 0.0, 1.0));

		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.2, -0.2, 0.0));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0, 1.0, 1.0));

		//trans = glm::rotate(trans, glm::radians(1.0f), glm::vec3(1.0, 1.0, 1.0));
		trans = glm::scale(trans, glm::vec3(1.001, 1.001, 1.001));

		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");	// ourColor ��� �̸��� ���� uniform ������ Shader Program ���� ã��
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);	// uniform ������ 4���� float ������ ���� (outColor �� vec4 Ÿ���̱� ����)

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// �ﰢ���� �׸��µ� �� 6���� ������ (�ﰢ�� �� ��) �� �׸� <- EBO ���
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}