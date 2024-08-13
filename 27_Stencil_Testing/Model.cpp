<<<<<<< HEAD
#include "Model.hpp"

unsigned int TextureFromFile(const char* path, const string& directory);// , bool gamma);

void Model::Draw(Shader shader)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(string path)
{
    Assimp::Importer importer;    // Importer 객체를 선언
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
                                // import.ReadFile( 파일의 경로, 여러 post-processing(전처리) 옵션들 )
                                
                                // aiProcess_Triangulate :      Assimp에게 모델이 삼각형으로만 이루어지지 않았다면 모델의 모든 primitive 도형들을 삼각형으로 변환하도록 함
                                // aiProcess_FlipUVs :          텍스처 좌표를 y 축으로 뒤집어줌 ( OpenGL에서 대부분의 이미지들은 y 축을 중심으로 거꾸로 된다 )
                                // aiProcess_GenNormals :       모델이 법선 벡터들을 가지고 있지 않다면 각 vertex에 대한 법선을 실제로 생성함
                                // aiProcess_SplitLargeMeshes : 큰 mesh들을 여러개의 작은 서브 mesh들로 나눔
                                // aiProcess_OptimizeMeshes :   여러 mesh들을 하나의 큰 mesh로 합침
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {                                                           //scene과 scene의 루트 노드가 null이 아닌지 확인하고 데이터가 불완전하다는 플래그가 세워져있는지 확인
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;                                                 // 에러를 출력하고 리턴
    }

    directory = path.substr(0, path.find_last_of('/'));     // 주어진 파일 경로의 디렉터리 경로를 얻어옴

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 노드의 모든 mesh들을 처리(만약 있다면)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // 그런 다음 각 자식들에게도 동일하게 적용
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// vertex 위치, 법선, 텍스처 좌표를 처리
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;    // 위치 벡터
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x; // 법선 벡터
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0])        // mesh가 텍스처 좌표를 가지고 있는가?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;   // 텍스쳐 좌표
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// indices 처리
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)		
	{	
		aiFace face = mesh->mFaces[i];							// 각 face들은 하나의 primitive를 나타냄
		for (unsigned int j = 0; j < face.mNumIndices; j++)		// face는 어떠한 순서로 vertex들을 그려야하는지를 정의하는 indices를 가지고 있음
			indices.push_back(face.mIndices[j]);
	}

	// material 처리
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];		// scene의 mMaterials 배열로부터 aiMaterial 객체를 얻음

	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");	// material 객체에서 텍스쳐를 얻는 함수

	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");

	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");

	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);     // GetTexture 함수를 통해 각 텍스처 파일의 위치를 얻음 ( model 파일의 텍스처 파일 경로가 model 파일 경로와 동일하다고 가정함 )
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; 
                break;
            }
        }
        if(!skip)
        {   // 텍스처가 이미 불러와져있지 않다면 불러옵니다.
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            //cout << str.C_Str() << endl;
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // 불러온 텍스처를 삽입합니다.
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory)//, bool gamma)  // 파일로부터 텍스쳐를 읽어 옴
{
    string filename = string(path);
    filename = directory + '/' + filename;
    //cout << filename << endl;

    const char* fn = filename.c_str();

    //cout << fn << endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);   // 얼마나 많은 텍스쳐를 저장할 것인지에 대한 설정

    int width, height, nrComponents;
    unsigned char *data = stbi_load(fn, &width, &height, &nrComponents, 0);   // 텍스쳐 이미지 파일 로드
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);    // 활성화된 텍스쳐 유닛에 해당 텍스쳐 (texture 변수) 를 바인딩
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);   // 텍스쳐를 어떻게 활용할 것인지 설정
        glGenerateMipmap(GL_TEXTURE_2D);    // 텍스쳐에 mipmap 적용

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // 텍스쳐 Wrapping x축(s) 옵션
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // 텍스쳐 Wrapping y축(t) 옵션
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);     // 텍스쳐 축소 Filtering 옵션
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // 텍스쳐 확대 Filtering 옵션

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
=======
#include "Model.hpp"

unsigned int TextureFromFile(const char* path, const string& directory);// , bool gamma);

void Model::Draw(Shader shader)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(string path)
{
    Assimp::Importer importer;    // Importer 객체를 선언
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
                                // import.ReadFile( 파일의 경로, 여러 post-processing(전처리) 옵션들 )
                                
                                // aiProcess_Triangulate :      Assimp에게 모델이 삼각형으로만 이루어지지 않았다면 모델의 모든 primitive 도형들을 삼각형으로 변환하도록 함
                                // aiProcess_FlipUVs :          텍스처 좌표를 y 축으로 뒤집어줌 ( OpenGL에서 대부분의 이미지들은 y 축을 중심으로 거꾸로 된다 )
                                // aiProcess_GenNormals :       모델이 법선 벡터들을 가지고 있지 않다면 각 vertex에 대한 법선을 실제로 생성함
                                // aiProcess_SplitLargeMeshes : 큰 mesh들을 여러개의 작은 서브 mesh들로 나눔
                                // aiProcess_OptimizeMeshes :   여러 mesh들을 하나의 큰 mesh로 합침
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {                                                           //scene과 scene의 루트 노드가 null이 아닌지 확인하고 데이터가 불완전하다는 플래그가 세워져있는지 확인
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;                                                 // 에러를 출력하고 리턴
    }

    directory = path.substr(0, path.find_last_of('/'));     // 주어진 파일 경로의 디렉터리 경로를 얻어옴

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 노드의 모든 mesh들을 처리(만약 있다면)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // 그런 다음 각 자식들에게도 동일하게 적용
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// vertex 위치, 법선, 텍스처 좌표를 처리
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;    // 위치 벡터
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x; // 법선 벡터
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0])        // mesh가 텍스처 좌표를 가지고 있는가?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;   // 텍스쳐 좌표
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// indices 처리
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)		
	{	
		aiFace face = mesh->mFaces[i];							// 각 face들은 하나의 primitive를 나타냄
		for (unsigned int j = 0; j < face.mNumIndices; j++)		// face는 어떠한 순서로 vertex들을 그려야하는지를 정의하는 indices를 가지고 있음
			indices.push_back(face.mIndices[j]);
	}

	// material 처리
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];		// scene의 mMaterials 배열로부터 aiMaterial 객체를 얻음

	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");	// material 객체에서 텍스쳐를 얻는 함수

	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");

	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");

	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);     // GetTexture 함수를 통해 각 텍스처 파일의 위치를 얻음 ( model 파일의 텍스처 파일 경로가 model 파일 경로와 동일하다고 가정함 )
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; 
                break;
            }
        }
        if(!skip)
        {   // 텍스처가 이미 불러와져있지 않다면 불러옵니다.
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            //cout << str.C_Str() << endl;
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // 불러온 텍스처를 삽입합니다.
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory)//, bool gamma)  // 파일로부터 텍스쳐를 읽어 옴
{
    string filename = string(path);
    filename = directory + '/' + filename;
    //cout << filename << endl;

    const char* fn = filename.c_str();

    //cout << fn << endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);   // 얼마나 많은 텍스쳐를 저장할 것인지에 대한 설정

    int width, height, nrComponents;
    unsigned char *data = stbi_load(fn, &width, &height, &nrComponents, 0);   // 텍스쳐 이미지 파일 로드
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);    // 활성화된 텍스쳐 유닛에 해당 텍스쳐 (texture 변수) 를 바인딩
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);   // 텍스쳐를 어떻게 활용할 것인지 설정
        glGenerateMipmap(GL_TEXTURE_2D);    // 텍스쳐에 mipmap 적용

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // 텍스쳐 Wrapping x축(s) 옵션
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // 텍스쳐 Wrapping y축(t) 옵션
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);     // 텍스쳐 축소 Filtering 옵션
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // 텍스쳐 확대 Filtering 옵션

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
>>>>>>> master
