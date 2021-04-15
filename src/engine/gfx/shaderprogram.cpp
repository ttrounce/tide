#include "shaderprogram.h"
#include <iostream>

namespace tide
{

void CreateVertexShader(GLuint program, std::string source, GLint sourceLength)
{
    const char* trueSource = source.c_str();
    GLuint handle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(handle, 1, &trueSource, &sourceLength);
    glCompileShader(handle);

    GLint success = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

        GLchar log[logSize];
        glGetShaderInfoLog(handle, logSize, &logSize, log);

        std::cout << "[TIDE]" << " Vertex shader could not compile: " << std::endl << log << std::endl;

        glDeleteShader(handle);
        return;
    }
    glAttachShader(program, handle);
    glDeleteShader(handle);
}

void CreateFragmentShader(GLuint program, std::string source, GLint sourceLength)
{
    const char* trueSource = source.c_str();
    GLuint handle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(handle, 1, &trueSource, &sourceLength);
    glCompileShader(handle);

    GLint success = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

        GLchar log[logSize];
        glGetShaderInfoLog(handle, logSize, &logSize, log);

        std::cout << "[TIDE]" << " Fragment shader could not compile: " << std::endl << log << std::endl;

        glDeleteShader(handle);
        return;
    }
    glAttachShader(program, handle);
    // screw giving the user the handle
    glDeleteShader(handle);
}

void LinkProgram(GLuint program)
{
    glLinkProgram(program);
    glValidateProgram(program);
    
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

        GLchar log[logSize];
        glGetProgramInfoLog(program, logSize, &logSize, log);

        std::cout << "[TIDE]" << " Program couldn't link: " << std::endl << log << std::endl;
        glDeleteProgram(program);
    }
}

void UniformVec2(GLuint program, std::string name, glm::vec2 vec)
{
    glUniform2f(glGetUniformLocation(program, name.c_str()), vec[0], vec[1]);
}

void UniformVec3(GLuint program, std::string name, glm::vec3 vec)
{
    glUniform3f(glGetUniformLocation(program, name.c_str()), vec[0], vec[1], vec[2]);
}

void UniformVec4(GLuint program, std::string name, glm::vec4 vec)
{
    glUniform4f(glGetUniformLocation(program, name.c_str()), vec[0], vec[1], vec[2], vec[3]);
}

void UniformMat2(GLuint program, std::string name, glm::mat2 value)
{
    glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, false, &value[0][0]);
}

void UniformMat3(GLuint program, std::string name, glm::mat3 value)
{
    glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, false, &value[0][0]);
}

void UniformMat4(GLuint program, std::string name, glm::mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, false, &value[0][0]);
}

}