#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//#version 330 core
//layout(location = 0) in vec3 aPos;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

int main()
{
	glfwInit();		//  GLFW �� �ʱ�ȭ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// OpenGL ������ 3.* �̹Ƿ� MAJOR �� MINOR ��� 3���� ����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);	// â�� ũ��� �̸� ���� �����Ͽ� ����

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


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float vertices[] = {	// ���� (x ��ǥ, y ��ǥ, z ��ǥ) �� ������ <- normalized device coordinates ������ ������
		0.5f ,0.5f, 0.0f,
		0.5f ,-0.5f, 0.0f,
		-0.5f ,-0.5f, 0.0f,
		-0.5f ,0.5f, 0.0f
	};

	unsigned int indices[] = {	// �ε��� ����
		0, 1, 3,
		1, 2, 3
	};


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

	unsigned int EBO;	// ������ ����ϴ� ���� �ߺ��� �߻��ϱ⿡ ���� �ߺ��� ���ϰ��� ����ϴ� EBO (Element Buffer Objects) <- �ε����� ����
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	// GL_ELEMENT_ARRAY_BUFFER �� ���� Ÿ������ ����
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// How to interperet Vertices ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
									// glVertexAttribPointer(vertex �� �Ӽ�, vertex �� ũ��, �������� Ÿ��, ����ȭ ����, vertex ���� ����, ���ۿ��� �����Ͱ� �����ϴ� ��ġ�� offset) ����
	glEnableVertexAttribArray(0);	// glVertexAttribPointer ���� ������ vertex �� ���� �Ӽ��� Ȱ��ȭ


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// Vertex Shader /////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* vertexShaderSource = "#version 330 core\n"	// GLSL ���
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	unsigned int vertexShader;	// ���̴� ��ü�� ID
	vertexShader = glCreateShader(GL_VERTEX_SHADER);	// ���̴� ������ �Է�

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);	// ���̴��� ������

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);	// ���̴��� �������� ���� ���θ� success �� ����

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);	// ���̴��� �������� �����Ѵٸ� infoLog �� �����޽��� ����
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////// Fragment Shader /////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* fragmentShaderSource = "#version 330 core\n"	// GLSL ���
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// Shader Program ////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned int shaderProgram;	// �������� ���̴����� ����ϱ� ���� ���̴����� shaderProgram ���� �������־�� ��
	shaderProgram = glCreateProgram();	// Program ���� �� ID ��ȯ

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// glUseProgram(shaderProgram);

	// glBindVertexArray(VAO);
	// someOpenGLFunctionThatDrawsOurTriangle();

	// glDrawArrays(GL_TRIANGLES, 0, 3);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// ��� //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	while (!glfwWindowShouldClose(window)) {	// GLFW �� ����ǵ��� (â�� �ݵ���) ���ù޾Ҵ��� Ȯ��
		processInput(window);

		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  //      glClear(GL_COLOR_BUFFER_BIT);	// glClearColor ���� ������ �������� �������� �� (�Ͼ������ ���� -> �Ͼ������ ������)

		//glUseProgram(shaderProgram);
		//glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  //      glDrawArrays(GL_TRIANGLES, 0, 3);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// �׸� �ﰢ���� ���θ� ä���� ����
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// �׸� �ﰢ���� ���θ� ä��

		glUseProgram(shaderProgram);	// ���̴����� ������ shaderProgram �� ����Ͽ� ������
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// �ﰢ���� �׸��µ� �� 6���� ������ (�ﰢ�� �� ��) �� �׸�
		glBindVertexArray(0);

		glfwSwapBuffers(window);	// �ø�Ŀ�� ���� ���� <- â�� �������� ����ģ �� ���ۿ� �� ���۸� ��ü�� (swap) (Double Buffer Ȱ��)
		glfwPollEvents();	// Ű���峪 ���콺�� �Է��� �־����� Ȯ��
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

    glfwTerminate();	// ������ �ݺ� ����

	glDeleteShader(vertexShader);	// ���̴� ��ü���� ����
	glDeleteShader(fragmentShader);

	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
    
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);	// â�� ���η� width, ���η� height ��ŭ ������ ��ǥ�踦 ����.
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {	// ESC Ű �Է��� �ִ� ���, glfwSetWindowShouldClose ���� â�� �ݵ��� ����
		glfwSetWindowShouldClose(window, true);
	}
}