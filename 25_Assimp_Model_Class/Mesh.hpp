#ifndef __MESH__
#define __MESH__

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.hpp"

#include <string>
#include <vector>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;     // 정점의 위치
    // normal
    glm::vec3 Normal;       // 정점에서의 Normal 벡터
    // texCoords
    glm::vec2 TexCoords;    // 텍스쳐 좌표
    // tangent
    glm::vec3 Tangent;      // 
    // bitangent
    glm::vec3 Bitangent;    //
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;        // 텍스쳐 ID
    string type;            // 텍스쳐의 타입 ( Diffuse 텍스쳐 또는 Specular 텍스쳐 )
    string path;  // 다른 텍스처와 비교하기 위해 텍스처의 경로를 저장
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;      // 정점들
    vector<unsigned int> indices;       // 인덱스들 
    vector<Texture>      textures;      // 텍스쳐들
    unsigned int VAO;       // vertex 속성 포인터를 구성할 때 오직 한 번만 호출하기만 하면 되고
						    //		오브젝트를 그려야 할 때마다 해당 VAO를 바인딩 하기만 하면 된다는 장점을 가지고 있는 VAO (Vertex Array Object)

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);      // 생성자

    // render the mesh
    void Draw(Shader& shader);

private:
    // render data 
    unsigned int VBO;       // 많은 양의 정점들을 GPU 메모리 상에 저장할 수 있는 VBO (Vertex Buffer Objects) 
						    //							<- CPU 에서 GPU 로 데이터를 보내는 속도가 느리기에 한꺼번에 많이 보내야 하기 때문에 사용
    
    unsigned int EBO;       // 정점을 명시하는 데에 중복이 발생하기에 이러한 중복을 피하고자 사용하는 EBO (Element Buffer Objects) <- 인덱스를 저장

    // initializes all the buffer objects/arrays
    void setupMesh();
};

#endif