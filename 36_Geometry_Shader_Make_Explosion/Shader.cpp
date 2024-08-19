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

	try {											// Path 에 있는 vertex 쉐이더 파일과 fregment 쉐이더 파일을 읽어옴
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

	vertex = glCreateShader(GL_VERTEX_SHADER);			// Vertex 쉐이더 객체의 ID
	glShaderSource(vertex, 1, &vShaderCode, NULL);	// 쉐이더 유형을 입력
	glCompileShader(vertex);	// 쉐이더를 컴파일

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);		// 쉐이더의 컴파일의 성공 여부를 success 에 저장
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);		// 쉐이더의 컴파일이 실패한다면 infoLog 에 에러메시지 저장
		cout << "ERROR::SHADER::VERTEX::COMPILEATION_FAILED\n" << infoLog << endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);		// Fragment 쉐이더 컴파일
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILEATION_FAILED\n" << infoLog << endl;
	}

	ID = glCreateProgram();	// Program 생성 후 ID 반환
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	glDeleteShader(vertex);	// 쉐이더 객체들을 제거
	glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
	string vertexCode;
	string geometryCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream gShaderFile;
	ifstream fShaderFile;

	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	gShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

	try {			// Path 에 있는 vertex 쉐이더와 geometry 쉐이더, fregment 쉐이더 파일을 읽어옴
		vShaderFile.open(vertexPath);
		gShaderFile.open(geometryPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, gShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		gShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		geometryCode = gShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e) {
		cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* gShaderCode = geometryCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, geometry, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);			// Vertex 쉐이더 객체의 ID
	glShaderSource(vertex, 1, &vShaderCode, NULL);	// 쉐이더 유형을 입력
	glCompileShader(vertex);	// 쉐이더를 컴파일

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);		// 쉐이더의 컴파일의 성공 여부를 success 에 저장
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);		// 쉐이더의 컴파일이 실패한다면 infoLog 에 에러메시지 저장
		cout << "ERROR::SHADER::VERTEX::COMPILEATION_FAILED\n" << infoLog << endl;
	}

	geometry = glCreateShader(GL_GEOMETRY_SHADER);		// Geometry 쉐이더 컴파일
	glShaderSource(geometry, 1, &gShaderCode, NULL);
	glCompileShader(geometry);

	glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(geometry, 512, NULL, infoLog);
		cout << "ERROR::SHADER::GEOMETRY::COMPILEATION_FAILED\n" << infoLog << endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);		// Fragment 쉐이더 컴파일
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "ERROR::SHADER::FRAGMENT::COMPILEATION_FAILED\n" << infoLog << endl;
	}

	ID = glCreateProgram();	// Program 생성 후 ID 반환
	glAttachShader(ID, vertex);
	glAttachShader(ID, geometry);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	glDeleteShader(vertex);	// 쉐이더 객체들을 제거
	glDeleteShader(geometry);
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

void Shader::setVec3(const std::string& name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}