#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // 텍스쳐 이미지를 가져오기 위해 사용하는 라이브러리
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

float deltaTime = 0.0f;	// 마지막 프레임과 현재 프레임 사이의 시간
float lastFrame = 0.0f; // 마지막 프레임의 시간

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);	// 카메라의 위치
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);	// 카메라의 앞쪽 방향
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);	// 위쪽 벡터

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

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

	glfwMakeContextCurrent(window);		// Double Buffer로 사용할 Window를 등록하는 함수이며, 등록되면 Thread로 설정되어 사용됨

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {  
		std::cout << "Failed to initialize GLAD" << std::endl;	// GLAD 가 제대로 초기화되지 않을 경우 에러 출력
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	// 창의 크기가 변경될 시 변경된 창의 가로, 세로 프레임에 따라 framebuffer_size_callback 호출

	glfwSetCursorPosCallback(window, mouse_callback);	// 마우스의 위치 입력을 콜백함수를 통해 받아 옴

	glfwSetScrollCallback(window, scroll_callback);		// 마우스의 스크롤 입력을 콜백함수를 통해 받아 옴

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


	float vertices[] = {	// 각각 (x 좌표, y 좌표, z 좌표) 인 정점들 <- normalized device coordinates 내부의 정점들
		// Position			  // Texture Coordinates

		// 윗면
		0.5f,	0.5f,	-0.5f,		1.0f,	1.0f,
		0.5f,	0.5f,	0.5f,		1.0f,	0.0f,
		-0.5f,	0.5f,	0.5f,		0.0f,	0.0f,
		-0.5f,	0.5f,	-0.5f,		0.0f,	1.0f,

		// 앞면
		0.5f,	0.5f,	0.5f,		1.0f,	1.0f,
		0.5f,	-0.5f,	0.5f,		1.0f,	0.0f,
		-0.5f,	-0.5f,	0.5f,		0.0f,	0.0f,
		-0.5f,	0.5f,	0.5f,		0.0f,	1.0f,
		
		// 아랫면
		0.5f,	-0.5f,	0.5f,		1.0f,	1.0f,
		0.5f,	-0.5f,	-0.5f,		1.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f,		0.0f,	0.0f,
		-0.5f,	-0.5f,	0.5f,		0.0f,	1.0f,

		// 뒷면
		0.5f,	-0.5f,	-0.5f,		1.0f,	1.0f,
		0.5f,	0.5f,	-0.5f,		1.0f,	0.0f,
		-0.5f,	0.5f,	-0.5f,		0.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f,		0.0f,	1.0f,

		// 왼쪽면
		-0.5f,	0.5f,	0.5f,		1.0f,	1.0f,
		-0.5f,	-0.5f,	0.5f,		1.0f,	0.0f,
		-0.5f,	-0.5f,	-0.5f,		0.0f,	0.0f,
		-0.5f,	0.5f,	-0.5f,		0.0f,	1.0f,

		// 오른쪽면
		0.5f,	0.5f,	-0.5f,		1.0f,	1.0f,
		0.5f,	-0.5f,	-0.5f,		1.0f,	0.0f,
		0.5f,	-0.5f,	0.5f,		0.0f,	0.0f,
		0.5f,	0.5f,	0.5f,		0.0f,	1.0f
	};

	unsigned int indices[] = {
		// 윗면
		0,	1,	3,
		1,	2,	3,

		// 앞면
		4,	5,	7,
		5,	6,	7,

		// 아랫면
		8,	9,	11,
		9,	10,	11,

		// 뒷면
		12,	13,	15,
		13,	14,	15,

		// 왼쪽면
		16,	17,	19,
		17,	18,	19,

		// 오른쪽면
		20,	21,	23,
		21,	22,	23
    };

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);	// 벡터의 선언
	glm::mat4 trans = glm::mat4(1.0f);	// 행렬의 선언 (Column major)
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));	// 이동변환을 행렬에 적용
	vec = trans * vec;
	std::cout << vec.x << vec.y << vec.z << std::endl;

	trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));	// 회전변환을 행렬에 적용
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));	// 확대 및 축소 변환을 행렬에 적용

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

	unsigned int EBO;	// 정점을 명시하는 데에 중복이 발생하기에 이러한 중복을 피하고자 사용하는 EBO (Element Buffer Objects) <- 인덱스를 저장
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);	// GL_ELEMENT_ARRAY_BUFFER 를 버퍼 타겟으로 지정
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// How to interperet Vertices ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
									// glVertexAttribPointer(vertex 의 속성, vertex 의 크기, 데이터의 타입, 정규화 여부, vertex 간의 간격, 버퍼에서 데이터가 시작하는 위치의 offset) 설정
	glEnableVertexAttribArray(0);	// glVertexAttribPointer 에서 설정한 vertex 의 정점 속성을 활성화

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
									// glVertexAttribPointer(vertex 의 속성, vertex 의 크기, 데이터의 타입, 정규화 여부, vertex 간의 간격, 버퍼에서 데이터가 시작하는 위치의 offset) 설정
	glEnableVertexAttribArray(1);	// glVertexAttribPointer 에서 설정한 vertex 의 정점 속성을 활성화

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////// Textures ///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int width, height, nrChannels;
	unsigned int texture1, texture2;

	stbi_set_flip_vertically_on_load(true);		// 뒤집어진 이미지 출력을 정상적으로 출력되게 함

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glGenTextures(1, &texture1);	// 얼마나 많은 텍스쳐를 저장할 것인지에 대한 설정

	//glActiveTexture(GL_TEXTURE0);	// 텍스쳐 유닛을 활성화 <- GL_TEXTURE0 텍스쳐 유닛은 default 로 항상 활성화

	glBindTexture(GL_TEXTURE_2D, texture1);	// 활성화된 텍스쳐 유닛에 해당 텍스쳐 (texture 변수) 를 바인딩

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// 텍스쳐 Wrapping x축(s) 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// 텍스쳐 Wrapping y축(t) 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// 텍스쳐 축소 Filtering 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 텍스쳐 확대 Filtering 옵션

	unsigned char* data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);	// 텍스쳐 이미지 파일 로드

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	// 텍스쳐를 어떻게 활용할 것인지 설정
		glGenerateMipmap(GL_TEXTURE_2D);	// 텍스쳐에 mipmap 적용
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glGenTextures(1, &texture2);	// 얼마나 많은 텍스쳐를 저장할 것인지에 대한 설정

	//glActiveTexture(GL_TEXTURE0);	// 텍스쳐 유닛을 활성화 <- GL_TEXTURE0 텍스쳐 유닛은 default 로 항상 활성화

	glBindTexture(GL_TEXTURE_2D, texture2);	// 활성화된 텍스쳐 유닛에 해당 텍스쳐 (texture 변수) 를 바인딩

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// 텍스쳐 Wrapping x축(s) 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// 텍스쳐 Wrapping y축(t) 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// 텍스쳐 축소 Filtering 옵션
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// 텍스쳐 확대 Filtering 옵션

	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);	// 텍스쳐 이미지 파일 로드

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	// 텍스쳐를 어떻게 활용할 것인지 설정
		glGenerateMipmap(GL_TEXTURE_2D);	// 텍스쳐에 mipmap 적용
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
	////////////////////////////////////////////////// 출력 //////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glEnable(GL_DEPTH_TEST); // Z-Buffer (깊이 버퍼) 를 활성화

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// 마우스가 창 안에서 보이지 않도록 설정

    // render loop
    // -----------
    
	while (!glfwWindowShouldClose(window)) {	// GLFW 가 종료되도록 (창을 닫도록) 지시받았는지 확인
		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);	// glClearColor 에서 설정한 색상으로 지워지게 됨 (하얀색으로 설정 -> 하얀색으로 지워짐)

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// 그린 삼각형의 내부를 채우지 않음
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// 그린 삼각형의 내부를 채움

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();	// 쉐이더들을 연결한 shaderProgram 을 사용하여 렌더링

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//model = glm::rotate(model, (float)sin(glfwGetTime()) * glm::radians(5.0f), glm::vec3(0.5f, 1.0f, 0.0f));	/////////////

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Z-Buffer (깊이 버퍼) 를 비워 줌 (초기화)

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);	// 카메라의 위치 변화에 따른 시점 이동

		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 1000.0f);	// 카메라의 시야 변화

		//float radius = 10.0f;
		//float camX = sin(glfwGetTime()) * radius;
		//float camZ = cos(glfwGetTime()) * radius;
		//glm::mat4 view;									// 카메라가 피사체 주위를 빙빙 돌도록 만듦. ( lookAt 함수를 사용 )
		//view = glm::lookAt(glm::vec3(camX, 0.0, camZ),	// 카메라의 위치
		//					glm::vec3(0.0, 0.0, 0.0),	// 피사체의 위치
		//					glm::vec3(0.0, 1.0, 0.0));	// 위쪽 벡터

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

		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	// 삼각형을 그리는데 총 6개의 정점들 (삼각형 두 개) 을 그림 <- EBO 사용
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
	
	float cameraSpeed = 25.0f * deltaTime;	// 각 프레임마다 이동시키도록 조치 (환경마다 프레임 간의 시간이 다르기 때문)

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)		// 키보드 W, S, A, D 입력에 따른 카메라 이동
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
	float yoffset = lastY - ypos; // y 좌표의 범위는 밑에서부터 위로가기 때문에 반대로 바꿉니다.
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