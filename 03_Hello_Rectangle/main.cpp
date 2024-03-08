#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//#version 330 core
//layout(location = 0) in vec3 aPos;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

int main()
{
	glfwInit();		//  GLFW 를 초기화
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// OpenGL 버전이 3.* 이므로 MAJOR 와 MINOR 모두 3으로 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);	// 창의 크기와 이름 등을 설정하여 생성

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;	// 창이 제대로 생성되지 않을 경우 에러 출력
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {  
		std::cout << "Failed to initialize GLAD" << std::endl;	// GLAD 가 제대로 초기화되지 않을 경우 에러 출력
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	// 창의 크기가 변경될 시 변경된 창의 가르, 세로 프레임에 따라 framebuffer_size_callback 호출


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float vertices[] = {	// 각각 (x 좌표, y 좌표, z 좌표) 인 정점들 <- normalized device coordinates 내부의 정점들
		0.5f ,0.5f, 0.0f,
		0.5f ,-0.5f, 0.0f,
		-0.5f ,-0.5f, 0.0f,
		-0.5f ,0.5f, 0.0f
	};

	unsigned int indices[] = {	// 인덱스 정보
		0, 1, 3,
		1, 2, 3
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned int VAO;	// vertex 속성 포인터를 구성할 때 오직 한 번만 호출하기만 하면 되고
						//		오브젝트를 그려야 할 때마다 해당 VAO를 바인딩 하기만 하면 된다는 장점을 가지고 있는 VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;	// 많은 양의 정점들을 GPU 메모리 상에 저장할 수 있는 VBO (Vertex Buffer Objects) 
						//							<- CPU 에서 GPU 로 데이터를 보내는 속도가 느리기에 한꺼번에 많이 보내야 하기 때문에 사용
	glGenBuffers(1, &VBO);	// VBO 버퍼의 ID 설정
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	// GL_ARRAY_BUFFER 를 타겟으로 하는 모든 버퍼는 VBO 를 사용하게 됨 (Binding Buffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// 현재 바인딩된 GL_ARRAY_BUFFER 에 앞서 설정한 vertices 배열을 넣음.
						//	GL_STREAM_DRAW	: 데이터 변경 X, 거의 쓰이지 않음
						//	GL_STATIC_DRAW	: 데이터 변경 X, 자주 쓰임
						//	GL_DYNAMIC_DRAW	: 데이터가 자주 변경됨, 자주 쓰임

	unsigned int EBO;	// 정점을 명시하는 데에 중복이 발생하기에 일한 중복을 피하고자 사용하는 EBO (Element Buffer Objects) <- 인덱스를 저장
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	// GL_ELEMENT_ARRAY_BUFFER 를 버퍼 타겟으로 지정
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// How to interperet Vertices ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
									// glVertexAttribPointer(vertex 의 속성, vertex 의 크기, 데이터의 타입, 정규화 여부, vertex 간의 간격, 버퍼에서 데이터가 시작하는 위치의 offset) 설정
	glEnableVertexAttribArray(0);	// glVertexAttribPointer 에서 설정한 vertex 의 정점 속성을 활성화


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////// Vertex Shader /////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* vertexShaderSource = "#version 330 core\n"	// GLSL 언어
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	unsigned int vertexShader;	// 쉐이더 객체의 ID
	vertexShader = glCreateShader(GL_VERTEX_SHADER);	// 쉐이더 유형을 입력

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);	// 쉐이더를 컴파일

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);	// 쉐이더의 컴파일의 성공 여부를 success 에 저장

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);	// 쉐이더의 컴파일이 실패한다면 infoLog 에 에러메시지 저장
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////// Fragment Shader /////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	const char* fragmentShaderSource = "#version 330 core\n"	// GLSL 언어
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

	unsigned int shaderProgram;	// 컴파일한 쉐이더들을 사용하기 위해 쉐이더들을 shaderProgram 으로 연결해주어야 함
	shaderProgram = glCreateProgram();	// Program 생성 후 ID 반환

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// glUseProgram(shaderProgram);

	// glBindVertexArray(VAO);
	// someOpenGLFunctionThatDrawsOurTriangle();

	// glDrawArrays(GL_TRIANGLES, 0, 3);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// 출력 //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	while (!glfwWindowShouldClose(window)) {	// GLFW 가 종료되도록 (창을 닫도록) 지시받았는지 확인
		processInput(window);

		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  //      glClear(GL_COLOR_BUFFER_BIT);	// glClearColor 에서 설정한 색상으로 지워지게 됨 (하얀색으로 설정 -> 하얀색으로 지워짐)

		//glUseProgram(shaderProgram);
		//glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  //      glDrawArrays(GL_TRIANGLES, 0, 3);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// 그린 삼각형의 내부를 채우지 않음
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// 그린 삼각형의 내부를 채움

		glUseProgram(shaderProgram);	// 쉐이더들을 연결한 shaderProgram 을 사용하여 렌더링
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// 삼각형을 그리는데 총 6개의 정점들 (삼각형 두 개) 을 그림
		glBindVertexArray(0);

		glfwSwapBuffers(window);	// 플리커링 현상 방지 <- 창의 렌더링을 끝마친 뒷 버퍼와 앞 버퍼를 교체함 (swap) (Double Buffer 활용)
		glfwPollEvents();	// 키보드나 마우스의 입력이 있었는지 확인
	}

	/////////////////////////////////////////////////////////////////////////////////////////////

    glfwTerminate();	// 렌더링 반복 종료

	glDeleteShader(vertexShader);	// 쉐이더 객체들을 제거
	glDeleteShader(fragmentShader);

	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
    
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);	// 창을 가로로 width, 세로로 height 만큼 분할한 좌표계를 설정.
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {	// ESC 키 입력이 있는 경우, glfwSetWindowShouldClose 에서 창을 닫도록 지시
		glfwSetWindowShouldClose(window, true);
	}
}