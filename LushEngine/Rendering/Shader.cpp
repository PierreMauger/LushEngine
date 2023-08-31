#include "Rendering/Shader.hpp"

using namespace Lush;

Shader::Shader(const File &file) : Resource(ResourceType::SHADER, file)
{
    this->load(file);
}

unsigned int compileShader(unsigned int shaderType, const std::string& source) {
    unsigned int shaderID = glCreateShader(shaderType);
    const char* sourcePtr = source.c_str();
    glShaderSource(shaderID, 1, &sourcePtr, nullptr);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
    }
    return shaderID;
}

void Shader::load(const File &file)
{
    std::string source = file.load();
    std::regex patern("#shader\\s+(\\w+)\n*([\\s\\S]*?)(?=#shader\\s+\\w+|$)");

    std::sregex_iterator next(source.begin(), source.end(), patern);
    std::sregex_iterator end;
    std::unordered_map<std::string, std::string> shaders;
    while (next != end) {
        std::smatch match = *next;
        shaders[match[1].str()] = match[2].str();
        next++;
    }

    this->_ID = glCreateProgram();
    std::vector<unsigned int> shaderIDs;

    for (const auto &shader : shaders) {
        if (shaderTypes.find(shader.first) == shaderTypes.end())
            throw std::runtime_error("Unknown shader type: " + shader.first);
        unsigned int shaderID = compileShader(shaderTypes[shader.first], shader.second);
        glAttachShader(this->_ID, shaderID);
        shaderIDs.push_back(shaderID);
    }
    glLinkProgram(this->_ID);

    int success;
    glGetProgramiv(this->_ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(this->_ID, 512, nullptr, infoLog);
        throw std::runtime_error("Shader linking failed: " + std::string(infoLog));
    }
    for (const auto &shaderID : shaderIDs)
        glDeleteShader(shaderID);
}

void Shader::reload(const File &file)
{
    glDeleteProgram(this->_ID);
    this->load(file);
}

void Shader::use() const
{
    glUseProgram(this->_ID);
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

void Shader::checkCompileErrors(GLuint shader, const std::string &type)
{
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            throw std::runtime_error(std::string("Shader compilation error:\n") + infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            throw std::runtime_error(std::string("Shader linking error:\n") + infoLog);
        }
    }
}
