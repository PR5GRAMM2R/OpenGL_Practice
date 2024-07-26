#ifndef __MODEL__
#define __MODEL__

#include <glad/glad.h> // holds all OpenGL type declarations

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.hpp"
#include "Mesh.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Model 
{
	public:
		/*  Model ������  */
		vector<Texture> textures_loaded;	// �ؽ�ó���� �����Ͽ� ���� ���μ��� �Ŀ��� ���� ( �Ź� ������ �ؽ��ĸ� �о���̱� �ʾƵ� �� )
		vector<Mesh>    meshes;
		string directory;
		bool gammaCorrection;

		/*  �Լ�   */
		Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
		{
			loadModel(path);
		}

		void Draw(Shader shader);

    private:
        /*  �Լ�   */
        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif // ! MODEL_H