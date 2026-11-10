
#pragma once

#include <glad/glad.h>
#include <string>

#include "Math/Mat4.h"

class Shader
{
public:
    Shader(const std::string& vertexPath,
           const std::string& fragmentPath);

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void Use() const;
    void SetFloat(const std::string& name, float value) const;
    void SetMat4(const std::string& name, const Mat4& matrix) const;

private:
    GLuint programID = 0;

    static std::string ReadFile(const std::string& path);

    static GLuint CompileShader(
        GLenum type,
        const std::string& source
    );
};