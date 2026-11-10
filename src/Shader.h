
#pragma once

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertexPath,
           const std::string& fragmentPath);

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Use() const;

private:
    GLuint programID = 0;

    static std::string ReadFile(const std::string& path);

    static GLuint CompileShader(
        GLenum type,
        const std::string& source
    );
};
