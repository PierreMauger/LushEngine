#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "File/Resource.hpp"
#include "Includes.hpp"

namespace Lush
{
    class Shader : public Resource
    {
        private:
            unsigned int _ID;

        public:
            Shader(const File &vertexFile, const File &fragmentFile, const File &geometryFile = File(), const File &tessControlFile = File(), const File &tessEvalFile = File());
            Shader() = default;
            ~Shader() = default;

            void load(const File &vertexFile, const File &fragmentFile, const File &geometryFile = File(), const File &tessControlFile = File(), const File &tessEvalFile = File());
            void reload(const File &vertexFile, const File &fragmentFile, const File &geometryFile = File(), const File &tessControlFile = File(),
                        const File &tessEvalFile = File());

            void use() const;
            unsigned int getID() const;
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
            void checkCompileErrors(GLuint shader, std::string type);
    };
}

#endif // SHADER_HPP
