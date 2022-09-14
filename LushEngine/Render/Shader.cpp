#include "Shader.hpp"

using namespace Lush;

Shader::Shader(const std::string vertexCode, const std::string fragmentCode, const std::string geometryCode, const std::string tessControlCode, const std::string tessEvalCode)
{
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    const char *tcShaderCode = tessControlCode.c_str();
    const char *teShaderCode = tessEvalCode.c_str();
    unsigned int vertex;
    unsigned int fragment;
    unsigned int geometry;
    unsigned int tessContol;
    unsigned int tessEval;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    this->checkCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    this->checkCompileErrors(fragment, "FRAGMENT");
    if (geometryCode != "") {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        this->checkCompileErrors(geometry, "GEOMETRY");
    }
    if (tessControlCode != "") {
        tessContol = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tessContol, 1, &tcShaderCode, NULL);
        glCompileShader(tessContol);
        this->checkCompileErrors(tessContol, "TESS_CONTROL");
    }
    if (tessEvalCode != "") {
        tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tessEval, 1, &teShaderCode, NULL);
        glCompileShader(tessEval);
        this->checkCompileErrors(tessEval, "TESS_EVALUATION");
    }
    this->_ID = glCreateProgram();
    glAttachShader(this->_ID, vertex);
    glAttachShader(this->_ID, fragment);
    if (geometryCode != "")
        glAttachShader(this->_ID, geometry);
    if (tessControlCode != "")
        glAttachShader(this->_ID, tessContol);
    if (tessEvalCode != "")
        glAttachShader(this->_ID, tessEval);
    glLinkProgram(this->_ID);
    this->checkCompileErrors(this->_ID, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryCode != "")
        glDeleteShader(geometry);
    if (tessControlCode != "")
        glDeleteShader(tessContol);
    if (tessEvalCode != "")
        glDeleteShader(tessEval);
}

void Shader::use() const
{
    glUseProgram(this->_ID);
}

unsigned int Shader::getID() const
{
    return this->_ID;
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(this->_ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(this->_ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(this->_ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(this->_ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(this->_ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(this->_ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(this->_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(this->_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            throw std::runtime_error(std::string("Shader compilation error:\n") + infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            throw std::runtime_error(std::string("Shader linking error:\n") + infoLog);
        }
    }
}
