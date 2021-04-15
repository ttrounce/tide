#ifndef TIDE_SHADER_PROGRAM_H
#define TIDE_SHADER_PROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
namespace tide
{

void CreateVertexShader(GLuint program, std::string source, GLint sourceLength);
void CreateFragmentShader(GLuint program, std::string source, GLint sourceLength);
void LinkProgram(GLuint program);

void UniformVec2(GLuint program, std::string name, glm::vec2 vec);
void UniformVec3(GLuint program, std::string name, glm::vec3 vec);
void UniformVec4(GLuint program, std::string name, glm::vec4 vec);
void UniformMat2(GLuint program, std::string name, glm::mat2 value);
void UniformMat3(GLuint program, std::string name, glm::mat3 value);
void UniformMat4(GLuint program, std::string name, glm::mat4 value);

}
#endif // TIDE_SHADER_PROGRAM_H
