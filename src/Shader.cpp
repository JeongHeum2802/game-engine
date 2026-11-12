
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open shader file: " + path
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint Shader::CompileShader(
    GLenum type,
    const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* shaderCode = source.c_str();

    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length > 0 ? length : 1);
        glGetShaderInfoLog(
            shader, static_cast<GLsizei>(log.size()),
            nullptr, log.data()
        );

        std::cerr << "Shader compile error:\n"
                  << log.data() << '\n';

        glDeleteShader(shader);

        throw std::runtime_error(
            "Shader compilation failed"
        );
    }

    return shader;
}

Shader::Shader(
    const std::string& vertexPath,
    const std::string& fragmentPath)
{
    std::string vertexSource = ReadFile(vertexPath);
    std::string fragmentSource = ReadFile(fragmentPath);

    GLuint vertexShader = CompileShader(
        GL_VERTEX_SHADER, vertexSource
    );

    GLuint fragmentShader = 0;

    try
    {
        fragmentShader = CompileShader(
            GL_FRAGMENT_SHADER, fragmentSource
        );

        programID = glCreateProgram();

        if (programID == 0)
            throw std::runtime_error(
                "Failed to create shader program"
            );

        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);
        glLinkProgram(programID);

        GLint success = 0;
        glGetProgramiv(
            programID, GL_LINK_STATUS, &success
        );

        if (!success)
        {
            GLint length = 0;
            glGetProgramiv(
                programID, GL_INFO_LOG_LENGTH, &length
            );

            std::vector<char> log(length > 0 ? length : 1);
            glGetProgramInfoLog(
                programID,
                static_cast<GLsizei>(log.size()),
                nullptr,
                log.data()
            );

            std::cerr << "Shader link error:\n"
                      << log.data() << '\n';

            throw std::runtime_error(
                "Shader program linking failed"
            );
        }
    }
    catch (...)
    {
        glDeleteShader(vertexShader);

        if (fragmentShader)
            glDeleteShader(fragmentShader);

        if (programID)
        {
            glDeleteProgram(programID);
            programID = 0;
        }

        throw;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Use() const
{
    glUseProgram(programID);
}

Shader::~Shader()
{
    if (programID != 0)
    {
        glDeleteProgram(programID);
    }
}

void Shader::SetFloat(
    const std::string& name,
    float value
) const
{
    GLint location = glGetUniformLocation(
        programID,
        name.c_str()
    );

    glUniform1f(location, value);
}

void Shader::SetMat4(
    const std::string& name,
    const Mat4& matrix
) const
{
    GLint location = glGetUniformLocation(
        programID,
        name.c_str()
    );

    glUniformMatrix4fv(
        location,
        1,
        GL_FALSE,
        matrix.Data()
    );
}

void Shader::SetVec3(
    const std::string& name,
    const Vec3& vec3
) const
{
    GLint location = glGetUniformLocation(
        programID,
        name.c_str()
    );

    glUniform3f(location, vec3.x, vec3.y, vec3.z);
}