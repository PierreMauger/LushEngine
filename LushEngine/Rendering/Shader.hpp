#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Includes.hpp"
#include "File/Resource.hpp"

namespace Lush
{
    static std::map<std::string, unsigned int> shaderTypes = {
        {"vertex", GL_VERTEX_SHADER},
        {"fragment", GL_FRAGMENT_SHADER},
        {"geometry", GL_GEOMETRY_SHADER},
        {"tessControl", GL_TESS_CONTROL_SHADER},
        {"tessEval", GL_TESS_EVALUATION_SHADER}
    };

    class Shader : public Resource
    {
        private:
            unsigned int _ID = 0;

        public:
            Shader(const File &file);
            Shader() = default;
            ~Shader() = default;

            void load(const File &file);
            void reload(const File &file);

            void use() const;
            void setBool(const std::string &name, bool value) const;
            void setInt(const std::string &name, int value) const;
            void setFloat(const std::string &name, float value) const;
            void setVec2(const std::string &name, const glm::vec2 &value) const;
            void setVec3(const std::string &name, const glm::vec3 &value) const;
            void setVec4(const std::string &name, const glm::vec4 &value) const;
            void setMat2(const std::string &name, const glm::mat2 &mat) const;
            void setMat3(const std::string &name, const glm::mat3 &mat) const;
            void setMat4(const std::string &name, const glm::mat4 &mat) const;

        private:
            void checkCompileErrors(GLuint shader, const std::string &type);
    };
}

#endif // SHADER_HPP
