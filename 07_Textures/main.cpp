#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // 텍스쳐 이미지를 가져오기 위해 사용하는 라이브러리
#include "Shader.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();		//  GLFW 를 초기화
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// OpenGL 버전이 3.* 이므로 MAJOR 와 MINOR 모두 3으로 설정
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);	// 창의 크기와 이름 등을 설정하여 생성

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

    // build and compile our shader program
    // ------------------------------------
    Shader ourShader("shaders/shader.vs", "shaders/shader.fs"); // you can name your shader files however you like

    float vertices[] = {	// 각각 (x 좌표, y 좌표, z 좌표) 인 정점들 <- normalized device coordinates 내부의 정점들
		// Position				// Colors				// Texture Coordinates
		0.5f	, 0.5f	, 0.0f	, 1.0f	, 0.0f	, 0.0f	, 1.0f	, 1.0f	,	// 우상단
		0.5f	, -0.5f	, 0.0f	, 0.0f	, 1.0f	, 0.0f	, 1.0f	, 0.0f	,	// 우하단
		-0.5f	, -0.5f	, 0.0f	, 0.0f	, 0.0f	, 1.0f	, 0.0f	, 0.0f	,	// 좌하단
		-0.5f	, 0.5f	, 0.0f	, 1.0f	, 1.0f	, 1.0f	, 0.0f	, 1.0f		// 좌상단
	};

	unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
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

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//								// glVertexAttribPointer(vertex 의 속성, vertex 의 크기, 데이터의 타입, 정규화 여부, vertex 간의 간격, 버퍼에서 데이터가 시작하는 위치의 offset) 설정
	//glEnableVertexAttribArray(0);	// glVertexAttribPointer 에서 설정한 vertex 의 정점 속성을 활성화

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
									// glVertexAttribPointer(vertex 의 속성, vertex 의 크기, 데이터의 타입, 정규화 여부, vertex 간의 간격, 버퍼에서 데이터가 시작하는 위치의 offset) 설정
	glEnableVertexAttribArray(0);	// glVertexAttribPointer 에서 설정한 vertex 의 정점 속성을 활성화

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
									// glVertexAttribPointer(vertex 의 속성, vertex 의 크기, 데이터의 타입, 정규화 여부, vertex 간의 간격, 버퍼에서 데이터가 시작하는 위치의 offset) 설정
	glEnableVertexAttribArray(1);	// glVertexAttribPointer 에서 설정한 vertex 의 정점 속성을 활성화

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
									// glVertexAttribPointer(vertex 의 속성, vertex 의 크기, 데이터의 타입, 정규화 여부, vertex 간의 간격, 버퍼에서 데이터가 시작하는 위치의 offset) 설정
	glEnableVertexAttribArray(2);	// glVertexAttribPointer 에서 설정한 vertex 의 정점 속성을 활성화

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////// Texture ////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int width, height, nrChannels;
	unsigned char* data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);	// 텍스쳐 이미지 파일 로드

	/*float texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f
    };*/

	unsigned int texture;
	glGenTextures(1, &texture);	// 얼마나 많은 텍스쳐를 저장할 것인지에 대한 설정
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// 텍스쳐 Wrapping x축(s) 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// 텍스쳐 Wrapping y축(t) 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// 텍스쳐 축소 Filtering 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 텍스쳐 확대 Filtering 옵션

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	// 텍스쳐를 어떻게 활용할 것인지 설정
		glGenerateMipmap(GL_TEXTURE_2D);	// 텍스쳐에 mipmap 적용
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////// 출력 //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // render loop
    // -----------
    
	while (!glfwWindowShouldClose(window)) {	// GLFW 가 종료되도록 (창을 닫도록) 지시받았는지 확인
		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);	// glClearColor 에서 설정한 색상으로 지워지게 됨 (하얀색으로 설정 -> 하얀색으로 지워짐)

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// 그린 삼각형의 내부를 채우지 않음
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// 그린 삼각형의 내부를 채움

		//float timeValue = glfwGetTime();	// 현 시간을 반환
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");	// ourColor 라는 이름을 가진 uniform 변수를 Shader Program 에서 찾음

		glBindTexture(GL_TEXTURE_2D, texture);

		ourShader.use();	// 쉐이더들을 연결한 shaderProgram 을 사용하여 렌더링

		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);	// uniform 변수에 4개의 float 변수를 넣음 (outColor 가 vec4 타입이기 때문)

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// 삼각형을 그리는데 총 6개의 정점들 (삼각형 두 개) 을 그림 <- EBO 사용
		//glBindVertexArray(0);

		//glDrawArrays(GL_TRIANGLES, 0, 3);	// 삼각형을 그리는데 3개의 정점을 그림 <- VAO 사용

		glfwSwapBuffers(window);	// 플리커링 현상 방지 <- 창의 렌더링을 끝마친 뒷 버퍼와 앞 버퍼를 교체함 (swap) (Double Buffer 활용)
		glfwPollEvents();	// 키보드나 마우스의 입력이 있었는지 확인
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