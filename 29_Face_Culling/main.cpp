#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "Camera.hpp"
#include "Model.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);    // Depth Testing을 활성화 ( fragment shader를 실행하기 전에 depth test를 수행 -> 비용을 줄일 수 있음 )
    
    //glDepthFunc(GL_ALWAYS);
    glDepthFunc(GL_LESS);

    // 함수	        // 설명
    // GL_ALWAYS	depth test가 항상 통과됩니다.
    // GL_NEVER	    depth test가 절대 통과되지 않습니다.
    // GL_LESS	    fragment의 깊이 값이 저장된 깊이 값보다 작을 경우 통과시킵니다.
    // GL_EQUAL	    fragment의 깊이 값이 저장된 깊이 값과 동일한 경우 통과시킵니다.
    // GL_LEQUAL	fragment의 깊이 값이 저장된 깂이 값과 동일하거나 작을 경우 통과시킵니다.
    // GL_GREATER	fragment의 깊이 값이 저장된 깊이 값보다 클 경우 통과시킵니다.
    // GL_NOTEQUAL	fragment의 깊이 값이 저장된 깊이 값과 동일하지 않을 경우 통과시킵니다.
    // GL_GEQUAL	fragment의 깊이 값이 저장된 깊이 값과 동일하거나 클 경우 통과시킵니다.


    /*
    glEnable(GL_STENCIL_TEST);  // Stencil Testing 을 활성화
    
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                                // fragment의 stencil 값이 레퍼런스 값인 1과 동일하지 않(GL_NOTEQUAL)다면 test를 통과시킨 후 렌더링하고 그렇지 않으면 폐기하라고 지시
    
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // glStencilOp ( stencil test가 실패하였을 때 취할 행동, stencil test가 통과했지만 depth test는 실패했을 때 취할 행동, stencil, depth test 모두 통과했을 때 취할 행동 )

    // 행동	            // 설명
    // GL_KEEP	        현재 저장된 stencil 값을 유지
    // GL_ZERO	        stencil 값을 0으로 설정
    // GL_REPLACE	    stencil 값을 glStencilFunc 함수에서 지정한 레퍼런스 값으로 설정
    // GL_INCR	        최댓값보다 작다면 stencil 값을 1만큼 증가시킴
    // GL_INCR_WRAP	    GL_INCR와 같지만 최댓값을 초과하면 0로 돌아옴
    // GL_DECR	        최솟값보다 크다면 stencil 값을 1만큼 감소시킴
    // GL_DECR_WRAP	    GL_DECR와 같지만 0보다 작다면 최댓값으로 설정
    // GL_INVERT	    현재 stencil buffer 값의 비트를 뒤집음

    */


    glEnable(GL_BLEND); // 블랜딩을 활성화

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 


    glEnable(GL_CULL_FACE); // Face Culling 활성화
    glCullFace(GL_BACK);    // 후면만 폐기
    glFrontFace(GL_CW);     // 전면을 시계방향으로 설정 ( 반시계방향인 면을 후면으로 취급 )


    // build and compile shaders
    // -------------------------
	Shader shader("shaders/shader.vs", "shaders/shader.fs");


	vector<glm::vec3> vegetation;
	vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    float vegetationVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };


	// load models
	// -----------
	//const char* path = "resources/objects/backpack/backpack.obj";

	//Model ourModel((char*)path);

	float cubeVertices[] = {
		// positions          // texture Coords
        // 
		// Back face
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
	    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
	    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left

	    // Front face
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
	     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
	    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left

	    // Left face
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
	    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
	    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right

	    // Right face
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
	     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
	     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
	     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     

	     // Bottom face
	     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
	      0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
	      0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	      0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
	     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
	     -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right

	     // Top face
	     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	      0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
	      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
	     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
	     -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left    
	};

	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f								
    };

    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    // cube VAO
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // load textures
    // -------------
    unsigned int cubeTexture  = loadTexture("resources/textures/marble.jpg");
    unsigned int floorTexture = loadTexture("resources/textures/metal.jpg");
    unsigned int transparentTexture = loadTexture("resources/textures/window.png");


    // transparent window locations
    // --------------------------------
    vector<glm::vec3> windows
    {
        glm::vec3(-1.5f, 0.0f, -0.48f),
        glm::vec3( 1.5f, 0.0f, 0.51f),
        glm::vec3( 0.0f, 0.0f, 0.7f),
        glm::vec3(-0.3f, 0.0f, -2.3f),
        glm::vec3( 0.5f, 0.0f, -0.6f)
    };


    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    
    // draw in wireframe    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);


        // sort the transparent windows before rendering
        // ---------------------------------------------
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(camera.Position - windows[i]);
            sorted[distance] = windows[i];
        }


        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // 각 렌더링 루프가 돌때마다 GL_DEPTH_BUFFER_BIT를 사용하여 depth buffer를 비워줌
                                                                                    // stencil buffer 또한 0 으로 전부 비워주어야 함

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // draw objects
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // windows (from furthest to nearest)
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        /*
        // set uniforms
        shaderSingleColor.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shaderSingleColor.setMat4("view", view);
        shaderSingleColor.setMat4("projection", projection);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
        glStencilMask(0x00);    // stencil buffer 작성 비활성화

        // floor
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // 1st. render pass, draw objects as normal, writing to the stencil buffer
        // --------------------------------------------------------------------
        glStencilFunc(GL_ALWAYS, 1, 0xFF);  // 컨테이너의 각 fragment들이 stencil buffer를 수정하여 stencil 값을 1로 만듦
        glStencilMask(0xFF);    // stencil buffer 작성 활성화

        // cubes
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
        // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
        // the objects' size differences, making it look like borders.
        // -----------------------------------------------------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        shaderSingleColor.use();
        float scale = 1.1f;

        // cubes
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        shaderSingleColor.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);
        */


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &transparentVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &transparentVAO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        //cout << width << " " << height << " " << nrComponents << endl;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);   ///
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}