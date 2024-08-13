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
    Assimp::Importer importer;    // Importer ��ü�� ����
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
                                // import.ReadFile( ������ ���, ���� post-processing(��ó��) �ɼǵ� )
                                
                                // aiProcess_Triangulate :      Assimp���� ���� �ﰢ�����θ� �̷������ �ʾҴٸ� ���� ��� primitive �������� �ﰢ������ ��ȯ�ϵ��� ��
                                // aiProcess_FlipUVs :          �ؽ�ó ��ǥ�� y ������ �������� ( OpenGL���� ��κ��� �̹������� y ���� �߽����� �Ųٷ� �ȴ� )
                                // aiProcess_GenNormals :       ���� ���� ���͵��� ������ ���� �ʴٸ� �� vertex�� ���� ������ ������ ������
                                // aiProcess_SplitLargeMeshes : ū mesh���� �������� ���� ���� mesh��� ����
                                // aiProcess_OptimizeMeshes :   ���� mesh���� �ϳ��� ū mesh�� ��ħ
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {                                                           //scene�� scene�� ��Ʈ ��尡 null�� �ƴ��� Ȯ���ϰ� �����Ͱ� �ҿ����ϴٴ� �÷��װ� �������ִ��� Ȯ��
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;                                                 // ������ ����ϰ� ����
    }

    directory = path.substr(0, path.find_last_of('/'));     // �־��� ���� ����� ���͸� ��θ� ����

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // ����� ��� mesh���� ó��(���� �ִٸ�)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // �׷� ���� �� �ڽĵ鿡�Ե� �����ϰ� ����
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

		// vertex ��ġ, ����, �ؽ�ó ��ǥ�� ó��
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;    // ��ġ ����
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x; // ���� ����
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0])        // mesh�� �ؽ�ó ��ǥ�� ������ �ִ°�?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;   // �ؽ��� ��ǥ
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

	// indices ó��
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)		
	{	
		aiFace face = mesh->mFaces[i];							// �� face���� �ϳ��� primitive�� ��Ÿ��
		for (unsigned int j = 0; j < face.mNumIndices; j++)		// face�� ��� ������ vertex���� �׷����ϴ����� �����ϴ� indices�� ������ ����
			indices.push_back(face.mIndices[j]);
	}

	// material ó��
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];		// scene�� mMaterials �迭�κ��� aiMaterial ��ü�� ����

	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");	// material ��ü���� �ؽ��ĸ� ��� �Լ�

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
        mat->GetTexture(type, i, &str);     // GetTexture �Լ��� ���� �� �ؽ�ó ������ ��ġ�� ���� ( model ������ �ؽ�ó ���� ��ΰ� model ���� ��ο� �����ϴٰ� ������ )
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
        {   // �ؽ�ó�� �̹� �ҷ��������� �ʴٸ� �ҷ��ɴϴ�.
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            //cout << str.C_Str() << endl;
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // �ҷ��� �ؽ�ó�� �����մϴ�.
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory)//, bool gamma)  // ���Ϸκ��� �ؽ��ĸ� �о� ��
{
    string filename = string(path);
    filename = directory + '/' + filename;
    //cout << filename << endl;

    const char* fn = filename.c_str();

    //cout << fn << endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);   // �󸶳� ���� �ؽ��ĸ� ������ �������� ���� ����

    int width, height, nrComponents;
    unsigned char *data = stbi_load(fn, &width, &height, &nrComponents, 0);   // �ؽ��� �̹��� ���� �ε�
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);    // Ȱ��ȭ�� �ؽ��� ���ֿ� �ش� �ؽ��� (texture ����) �� ���ε�
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);   // �ؽ��ĸ� ��� Ȱ���� ������ ����
        glGenerateMipmap(GL_TEXTURE_2D);    // �ؽ��Ŀ� mipmap ����

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // �ؽ��� Wrapping x��(s) �ɼ�
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // �ؽ��� Wrapping y��(t) �ɼ�
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);     // �ؽ��� ��� Filtering �ɼ�
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // �ؽ��� Ȯ�� Filtering �ɼ�

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
    Assimp::Importer importer;    // Importer ��ü�� ����
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
                                // import.ReadFile( ������ ���, ���� post-processing(��ó��) �ɼǵ� )
                                
                                // aiProcess_Triangulate :      Assimp���� ���� �ﰢ�����θ� �̷������ �ʾҴٸ� ���� ��� primitive �������� �ﰢ������ ��ȯ�ϵ��� ��
                                // aiProcess_FlipUVs :          �ؽ�ó ��ǥ�� y ������ �������� ( OpenGL���� ��κ��� �̹������� y ���� �߽����� �Ųٷ� �ȴ� )
                                // aiProcess_GenNormals :       ���� ���� ���͵��� ������ ���� �ʴٸ� �� vertex�� ���� ������ ������ ������
                                // aiProcess_SplitLargeMeshes : ū mesh���� �������� ���� ���� mesh��� ����
                                // aiProcess_OptimizeMeshes :   ���� mesh���� �ϳ��� ū mesh�� ��ħ
	
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {                                                           //scene�� scene�� ��Ʈ ��尡 null�� �ƴ��� Ȯ���ϰ� �����Ͱ� �ҿ����ϴٴ� �÷��װ� �������ִ��� Ȯ��
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;                                                 // ������ ����ϰ� ����
    }

    directory = path.substr(0, path.find_last_of('/'));     // �־��� ���� ����� ���͸� ��θ� ����

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // ����� ��� mesh���� ó��(���� �ִٸ�)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // �׷� ���� �� �ڽĵ鿡�Ե� �����ϰ� ����
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

		// vertex ��ġ, ����, �ؽ�ó ��ǥ�� ó��
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;    // ��ġ ����
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x; // ���� ����
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0])        // mesh�� �ؽ�ó ��ǥ�� ������ �ִ°�?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;   // �ؽ��� ��ǥ
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

	// indices ó��
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)		
	{	
		aiFace face = mesh->mFaces[i];							// �� face���� �ϳ��� primitive�� ��Ÿ��
		for (unsigned int j = 0; j < face.mNumIndices; j++)		// face�� ��� ������ vertex���� �׷����ϴ����� �����ϴ� indices�� ������ ����
			indices.push_back(face.mIndices[j]);
	}

	// material ó��
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];		// scene�� mMaterials �迭�κ��� aiMaterial ��ü�� ����

	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");	// material ��ü���� �ؽ��ĸ� ��� �Լ�

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
        mat->GetTexture(type, i, &str);     // GetTexture �Լ��� ���� �� �ؽ�ó ������ ��ġ�� ���� ( model ������ �ؽ�ó ���� ��ΰ� model ���� ��ο� �����ϴٰ� ������ )
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
        {   // �ؽ�ó�� �̹� �ҷ��������� �ʴٸ� �ҷ��ɴϴ�.
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            //cout << str.C_Str() << endl;
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // �ҷ��� �ؽ�ó�� �����մϴ�.
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory)//, bool gamma)  // ���Ϸκ��� �ؽ��ĸ� �о� ��
{
    string filename = string(path);
    filename = directory + '/' + filename;
    //cout << filename << endl;

    const char* fn = filename.c_str();

    //cout << fn << endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);   // �󸶳� ���� �ؽ��ĸ� ������ �������� ���� ����

    int width, height, nrComponents;
    unsigned char *data = stbi_load(fn, &width, &height, &nrComponents, 0);   // �ؽ��� �̹��� ���� �ε�
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);    // Ȱ��ȭ�� �ؽ��� ���ֿ� �ش� �ؽ��� (texture ����) �� ���ε�
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);   // �ؽ��ĸ� ��� Ȱ���� ������ ����
        glGenerateMipmap(GL_TEXTURE_2D);    // �ؽ��Ŀ� mipmap ����

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // �ؽ��� Wrapping x��(s) �ɼ�
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // �ؽ��� Wrapping y��(t) �ɼ�
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);     // �ؽ��� ��� Filtering �ɼ�
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // �ؽ��� Ȯ�� Filtering �ɼ�

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
