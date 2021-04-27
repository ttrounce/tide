#include "shaderprogram.h"
#include <fmt/core.h>

void CreateVertexShader(GLuint program, const std::string& source, GLint sourceLength)
{
    const char* trueSource = source.c_str();
    GLuint handle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(handle, 1, &trueSource, &sourceLength);
    glCompileShader(handle);

    GLint success = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

        GLchar log[logSize];
        glGetShaderInfoLog(handle, logSize, &logSize, log);

        fmt::print("[TIDE] Vertex shader could not compile:\n{}\n", std::string(log));

        glDeleteShader(handle);
        return;
    }
    glAttachShader(program, handle);
    glDeleteShader(handle);
}

void CreateFragmentShader(GLuint program, const std::string& source, GLint sourceLength)
{
    const char* trueSource = source.c_str();
    GLuint handle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(handle, 1, &trueSource, &sourceLength);
    glCompileShader(handle);

    GLint success = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

        GLchar log[logSize];
        glGetShaderInfoLog(handle, logSize, &logSize, log);

        fmt::print("[TIDE] Fragment shader could not compile:\n{}\n", std::string(log));

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
    if (success == GL_FALSE)
    {
        GLint logSize = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

        GLchar log[logSize];
        glGetProgramInfoLog(program, logSize, &logSize, log);

        fmt::print("Program could not link:\n{}\n", std::string(log));
        glDeleteProgram(program);
    }
}

void UniformVec2(GLuint program, const std::string& name, const glm::vec2& vec)
{
    glUniform2f(glGetUniformLocation(program, name.c_str()), vec[0], vec[1]);
}

void UniformVec3(GLuint program, const std::string& name, const glm::vec3& vec)
{
    glUniform3f(glGetUniformLocation(program, name.c_str()), vec[0], vec[1], vec[2]);
}

void UniformVec4(GLuint program, const std::string& name, const glm::vec4& vec)
{
    glUniform4f(glGetUniformLocation(program, name.c_str()), vec[0], vec[1], vec[2], vec[3]);
}

void UniformMat2(GLuint program, const std::string& name, const glm::mat2& value)
{
    glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, false, &value[0][0]);
}

void UniformMat3(GLuint program, const std::string& name, const glm::mat3& value)
{
    glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, false, &value[0][0]);
}

void UniformMat4(GLuint program, const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, false, &value[0][0]);
}
