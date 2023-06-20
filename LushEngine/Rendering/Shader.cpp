#include "Rendering/Shader.hpp"

using namespace Lush;

// Shader::Shader(const File &vertexFile, const File &fragmentFile, const File &geometryFile, const File &tessControlFile, const File &tessEvalFile)
//     : Resource(ResourceType::SHADER, vertexFile, fragmentFile, geometryFile, tessControlFile, tessEvalFile)
// {
//     this->load(vertexFile, fragmentFile, geometryFile, tessControlFile, tessEvalFile);
// }

// void Shader::load(const File &vertexFile, const File &fragmentFile, const File &geometryFile, const File &tessControlFile, const File &tessEvalFile)
// {
//     std::string vShaderCode = vertexFile.load();
//     std::string fShaderCode = fragmentFile.load();
//     std::string gShaderCode = geometryFile.load();
//     std::string tcShaderCode = tessControlFile.load();
//     std::string teShaderCode = tessEvalFile.load();

//     const char *vShader = vShaderCode.c_str();
//     const char *fShader = fShaderCode.c_str();
//     const char *gShader = gShaderCode.c_str();
//     const char *tcShader = tcShaderCode.c_str();
//     const char *teShader = teShaderCode.c_str();

//     unsigned int vertex = 0;
//     unsigned int fragment = 0;
//     unsigned int geometry = 0;
//     unsigned int tessControl = 0;
//     unsigned int tessEval = 0;

//     vertex = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertex, 1, &vShader, nullptr);
//     glCompileShader(vertex);
//     this->checkCompileErrors(vertex, "VERTEX");
//     fragment = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragment, 1, &fShader, nullptr);
//     glCompileShader(fragment);
//     this->checkCompileErrors(fragment, "FRAGMENT");
//     if (!gShaderCode.empty()) {
//         geometry = glCreateShader(GL_GEOMETRY_SHADER);
//         glShaderSource(geometry, 1, &gShader, nullptr);
//         glCompileShader(geometry);
//         this->checkCompileErrors(geometry, "GEOMETRY");
//     }
//     if (!tcShaderCode.empty()) {
//         tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
//         glShaderSource(tessControl, 1, &tcShader, nullptr);
//         glCompileShader(tessControl);
//         this->checkCompileErrors(tessControl, "TESS_CONTROL");
//     }
//     if (!teShaderCode.empty()) {
//         tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
//         glShaderSource(tessEval, 1, &teShader, nullptr);
//         glCompileShader(tessEval);
//         this->checkCompileErrors(tessEval, "TESS_EVALUATION");
//     }
//     this->_ID = glCreateProgram();

//     glAttachShader(this->_ID, vertex);
//     glAttachShader(this->_ID, fragment);
//     if (!gShaderCode.empty())
//         glAttachShader(this->_ID, geometry);
//     if (!tcShaderCode.empty())
//         glAttachShader(this->_ID, tessControl);
//     if (!teShaderCode.empty())
//         glAttachShader(this->_ID, tessEval);
//     glLinkProgram(this->_ID);
//     this->checkCompileErrors(this->_ID, "PROGRAM");
//     glDeleteShader(vertex);
//     glDeleteShader(fragment);
//     if (!gShaderCode.empty())
//         glDeleteShader(geometry);
//     if (!tcShaderCode.empty())
//         glDeleteShader(tessControl);
//     if (!teShaderCode.empty())
//         glDeleteShader(tessEval);
// }

// void Shader::reload(const File &vertexFile, const File &fragmentFile, const File &geometryFile, const File &tessControlFile, const File &tessEvalFile)
// {
//     glDetachShader(this->_ID, GL_VERTEX_SHADER);
//     glDetachShader(this->_ID, GL_FRAGMENT_SHADER);
//     glDetachShader(this->_ID, GL_GEOMETRY_SHADER);
//     glDetachShader(this->_ID, GL_TESS_CONTROL_SHADER);
//     glDetachShader(this->_ID, GL_TESS_EVALUATION_SHADER);
//     glDeleteProgram(this->_ID);

//     this->load(vertexFile, fragmentFile, geometryFile, tessControlFile, tessEvalFile);
// }

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
        std::cout << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shaderID;
}

void Shader::load(const File &file)
{
    std::string source = file.load();
    std::regex patern("#shader\\s+(\\w+)\n*([\\s\\S]*?)(?=#shader\\s+\\w+|$)");

    std::sregex_iterator next(source.begin(), source.end(), patern);
    std::sregex_iterator end;
    std::map<std::string, std::string> shaders;
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
        std::cout << "Shader " << shader.first << " compiled " << shaderID << std::endl;

        glAttachShader(this->_ID, shaderID);
        shaderIDs.push_back(shaderID);
    }
    glLinkProgram(this->_ID);

    int success;
    glGetProgramiv(this->_ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(this->_ID, 512, nullptr, infoLog);
        std::cout << "Shader linking failed: " << infoLog << std::endl;
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
