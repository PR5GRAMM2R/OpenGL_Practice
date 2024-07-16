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
        /*  함수   */
        Model(char *path)
        {
            loadModel(path);
        }

        void Draw(Shader shader);	

    private:
        /*  Model 데이터  */
        vector<Mesh> meshes;
        string directory;
        vector<Texture> textures_loaded;    // 텍스처들을 재사용하여 많은 프로세싱 파워를 절약 ( 매번 동일한 텍스쳐를 읽어들이기 않아도 됨 )

        /*  함수   */
        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

#endif // ! MODEL_H