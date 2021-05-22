#include "shader.h"

Shader::Shader(std::string vertexShaderSource, std::string fragmentShaderSource) {
    compileVertexShader(vertexShaderSource);
    compileFragmentShader(fragmentShaderSource);
    link();
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::deleteProgram() {
    glDeleteProgram(shaderProgram);
}

void Shader::compileVertexShader(std::string shaderSoruce) {
    vertexShader = loadShaderFromFile(GL_VERTEX_SHADER, shaderSoruce);
}

void Shader::compileFragmentShader(std::string shaderSource) {
    fragmentShader = loadShaderFromFile(GL_FRAGMENT_SHADER, shaderSource);
}

void Shader::link() {
    shaderProgram = linkShaderProgram(vertexShader, fragmentShader);
}

void Shader::use() {
    glUseProgram(shaderProgram);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setVec2Int(const std::string& name, int x, int y) {
    glUniform2i(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}

void Shader::setVec3Int(const std::string& name, int x, int y, int z) {
    glUniform3i(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}

void Shader::setVec4Int(const std::string& name, int x, int y, int z, int w) {
    glUniform4i(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}

void Shader::setUInt(const std::string& name, unsigned int value) {
    glUniform1ui(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setVec2UInt(const std::string& name, unsigned int x, unsigned int y) {
    glUniform2ui(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}

void Shader::setVec3UInt(const std::string& name, unsigned int x, unsigned int y, unsigned int z) {
    glUniform3ui(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}

void Shader::setVec4UInt(const std::string& name, unsigned int x, unsigned int y, unsigned int z, unsigned int w) {
    glUniform4ui(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, float x, float y) {
    glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}

void Shader::setDouble(const std::string& name, double value) {
    glUniform1d(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setVec2Double(const std::string& name, double x, double y) {
    glUniform2d(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}

void Shader::setVec3Double(const std::string& name, double x, double y, double z) {
    glUniform3d(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}

void Shader::setVec4Double(const std::string& name, double x, double y, double z, double w) {
    glUniform4d(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}

std::string Shader::readFileToString(const char* filePath) {
    std::ifstream inputStream(filePath);
    inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::stringstream stringStream; 
    try {
        stringStream << inputStream.rdbuf();
    } catch (std::ifstream::failure e) {
        std::cout << "Error: Could not read file" << std::endl;
    }
    return stringStream.str();
}

unsigned int Shader::loadShaderFromFile(int type, std::string filePath) {
    unsigned int shader = glCreateShader(type);
    std::string shaderSourceString = readFileToString(filePath.c_str());
    const char* shaderSource = shaderSourceString.c_str();
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader failed to compile: " << infoLog << std::endl;
    }
    return shader;
}

unsigned int Shader::linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "Shader program failed to link: " << infoLog << std::endl;
    }
    return shaderProgram;
}
