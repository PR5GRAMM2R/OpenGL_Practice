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

    // ���͵��� ����Ͽ� Camera ����
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    
    // ��Į�� ������ ����Ͽ� Camera ����
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Camera �� View Matrix �� ������
    glm::mat4 GetViewMatrix();

    // Ű���带 ���� Camera �� �̵� �Է�
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // ���콺�� ���� Camera �� �ü� �Է�
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // ���콺�� ���� Camera �� �þ� �Է�
    void ProcessMouseScroll(float yoffset);

private:
    // Camera �� ������ ������Ʈ
    void updateCameraVectors();
};
#endif