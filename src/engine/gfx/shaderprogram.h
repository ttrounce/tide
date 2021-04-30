#ifndef TIDE_SHADER_PROGRAM_H
#define TIDE_SHADER_PROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

void CreateVertexShader(GLuint program, const std::string& source, GLint sourceLength);
void CreateFragmentShader(GLuint program, const std::string& source, GLint sourceLength);
void LinkProgram(GLuint program);

void UniformVec2(GLuint loc, const glm::vec2& vec);
void UniformVec3(GLuint loc, const glm::vec3& vec);
void UniformVec4(GLuint loc, const glm::vec4& vec);
void UniformMat2(GLuint loc, const glm::mat2& value);
void UniformMat3(GLuint loc, const glm::mat3& value);
void UniformMat4(GLuint loc, const glm::mat4& value);

#endif // TIDE_SHADER_PROGRAM_H
