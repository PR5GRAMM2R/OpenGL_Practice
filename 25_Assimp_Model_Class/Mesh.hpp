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
    glm::vec3 Position;     // ������ ��ġ
    // normal
    glm::vec3 Normal;       // ���������� Normal ����
    // texCoords
    glm::vec2 TexCoords;    // �ؽ��� ��ǥ
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
    unsigned int id;        // �ؽ��� ID
    string type;            // �ؽ����� Ÿ�� ( Diffuse �ؽ��� �Ǵ� Specular �ؽ��� )
    string path;  // �ٸ� �ؽ�ó�� ���ϱ� ���� �ؽ�ó�� ��θ� ����
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;      // ������
    vector<unsigned int> indices;       // �ε����� 
    vector<Texture>      textures;      // �ؽ��ĵ�
    unsigned int VAO;       // vertex �Ӽ� �����͸� ������ �� ���� �� ���� ȣ���ϱ⸸ �ϸ� �ǰ�
						    //		������Ʈ�� �׷��� �� ������ �ش� VAO�� ���ε� �ϱ⸸ �ϸ� �ȴٴ� ������ ������ �ִ� VAO (Vertex Array Object)

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);      // ������

    // render the mesh
    void Draw(Shader& shader);

private:
    // render data 
    unsigned int VBO;       // ���� ���� �������� GPU �޸� �� ������ �� �ִ� VBO (Vertex Buffer Objects) 
						    //							<- CPU ���� GPU �� �����͸� ������ �ӵ��� �����⿡ �Ѳ����� ���� ������ �ϱ� ������ ���
    
    unsigned int EBO;       // ������ ����ϴ� ���� �ߺ��� �߻��ϱ⿡ �̷��� �ߺ��� ���ϰ��� ����ϴ� EBO (Element Buffer Objects) <- �ε����� ����

    // initializes all the buffer objects/arrays
    void setupMesh();
};

#endif