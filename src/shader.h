#pragma once
#ifndef MANDELBROT_SHADER_INCLUDED
#define MANDELBROT_SHADER_INCLUDED

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <glad/glad.h>

class Shader
{
protected:

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

public:

    Shader(std::string vertexShaderSource, std::string fragmentShaderSource);

    void compileVertexShader(std::string shaderSoruce);
    void compileFragmentShader(std::string shaderSource);
    void link();
    void use();
    void deleteProgram();

    void setInt(const std::string& name, int value);
    void setVec2Int(const std::string& name, int x, int y);
    void setVec3Int(const std::string& name, int x, int y, int z);
    void setVec4Int(const std::string& name, int x, int y, int z, int w);
    void setUInt(const std::string& name, unsigned int value);
    void setVec2UInt(const std::string& name, unsigned int x, unsigned int y);
    void setVec3UInt(const std::string& name, unsigned int x, unsigned int y, unsigned int z);
    void setVec4UInt(const std::string& name, unsigned int x, unsigned int y, unsigned int z, unsigned int w);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, float x, float y, float z, float w);
    void setDouble(const std::string& name, double value);
    void setVec2Double(const std::string& name, double x, double y);
    void setVec3Double(const std::string& name, double x, double y, double z);
    void setVec4Double(const std::string& name, double x, double y, double z, double w);

protected: // static helpers

    static std::string readFileToString(const char* filePath);

    /** 
     * @param type Needs to be either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
     * @return Id of the shader object
     */
    static unsigned int loadShaderFromFile(int type, std::string filePath);

    /**
     * @return Id of the program object 
     */
    static unsigned int linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

};

#endif
