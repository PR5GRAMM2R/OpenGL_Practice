#include "Shader.hpp"

#include <GLFW/glfw3.h>

using namespace std;

Shader::Shader(const char* vertexPath, const char* fragmentPath) 
{
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;

	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

	try {											// Path �� �ִ� vertex ���̴� ���ϰ� fregment ���̴� ������ �о��
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e) {
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);			// Vertex ���̴� ��ü�� ID
	glShaderSource(vertex, 1, &vShaderCode, NULL);	// ���̴� ������ �Է�
	glCompileShader(vertex);	// ���̴��� ������

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);		// ���̴��� �������� ���� ���θ� success �� ����
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);		// ���̴��� �������� �����Ѵٸ� infoLog �� �����޽��� ����
		cout << "ERROR::SHADER::VERTEX::COMPILEATION_FAILED\n" << infoLog << endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);		// Fragment ���̴� ������
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILEATION_FAILED\n" << infoLog << endl;
	}

	ID = glCreateProgram();	// Program ���� �� ID ��ȯ
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	glDeleteShader(vertex);	// ���̴� ��ü���� ����
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
	int modelLoc = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, float value1, float value2, float value3) const
{
	//glm::vec3 inputVector = glm::vec3(value1, value2, value3);
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value1, value2, value3);
}