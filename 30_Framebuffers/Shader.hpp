#ifndef __SHADER__
#define __SHADER__

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public :
	unsigned int ID;	// 컴파일한 쉐이더들을 사용하기 위해 쉐이더들을 shaderProgram 으로 연결해주어야 함

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
	void setVec3(const std::string& name, float value1, float value2, float value3) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
};

#endif