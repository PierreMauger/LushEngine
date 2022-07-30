#ifndef RENDER_OBJECT_HPP
#define RENDER_OBJECT_HPP

#include "GameObject.hpp"
#include "Camera.hpp"
#include "Includes.hpp"

namespace Lush
{
    class RenderObject
    {
        protected:
            int _type;
            std::string _name;
            glm::vec3 _position;
            glm::vec3 _scale;
            glm::vec3 _rotation;

        public:
            RenderObject(GameObject obj);
            ~RenderObject() = default;

            glm::vec3 getPosition() const;
            glm::vec3 getScale() const;
            glm::vec3 getRotation() const;

            void setPosition(glm::vec3 position);
            void setScale(glm::vec3 scale);
            void setRotation(glm::vec3 rotation);
            virtual void draw(Camera &camera) = 0;
    };
}

#endif // RENDER_OBJECT_HPP
