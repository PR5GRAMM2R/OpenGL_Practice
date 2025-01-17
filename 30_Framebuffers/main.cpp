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
    //glDepthFunc(GL_LESS);

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

    /*
    glEnable(GL_BLEND); // 블랜딩을 활성화

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 


    glEnable(GL_CULL_FACE); // Face Culling 활성화
    glCullFace(GL_BACK);    // 후면만 폐기
    glFrontFace(GL_CW);     // 전면을 시계방향으로 설정 ( 반시계방향인 면을 후면으로 취급 )
    */


    // build and compile shaders
    // -------------------------
	Shader shader("shaders/shader.vs", "shaders/shader.fs");
    Shader screenShader("shaders/screenShader.vs", "shaders/screenShader.fs");



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

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
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

    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // load textures
    // -------------
    unsigned int cubeTexture  = loadTexture("resources/textures/container2.png");
    unsigned int floorTexture = loadTexture("resources/textures/metal.jpg");




    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("texture1", 0);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    
    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);



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



        // render
        // ------
        // bind to framebuffer and draw scene as we normally would to color texture 
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
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
        shader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);

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