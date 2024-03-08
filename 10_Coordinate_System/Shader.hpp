#ifndef __SHADER__
#define __SHADER__

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class Shader
{
public :
	unsigned int ID;	// 컴파일한 쉐이더들을 사용하기 위해 쉐이더들을 shaderProgram 으로 연결해주어야 함

	Shader(const char* vertexPath, const char* fragmentPath);

	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
};

#endif