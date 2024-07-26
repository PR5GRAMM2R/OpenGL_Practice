#ifndef __CAMERA__
#define __CAMERA__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  10.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 벡터들을 사용하여 Camera 생성
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    
    // 스칼라 값들을 사용하여 Camera 생성
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Camera 의 View Matrix 를 가져옴
    glm::mat4 GetViewMatrix();

    // 키보드를 통한 Camera 의 이동 입력
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // 마우스를 통한 Camera 의 시선 입력
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // 마우스를 통한 Camera 의 시야 입력
    void ProcessMouseScroll(float yoffset);

private:
    // Camera 의 정보를 업데이트
    void updateCameraVectors();
};
#endif