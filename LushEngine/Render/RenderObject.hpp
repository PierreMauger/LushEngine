#ifndef RENDER_OBJECT_HPP
#define RENDER_OBJECT_HPP

#include "Camera.hpp"
#include "GameObject.hpp"
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
            bool _isHovered;
            bool _isSelected;
            bool _polygonMode;

        public:
            RenderObject(GameObject obj);
            ~RenderObject() = default;

            glm::vec3 getPosition() const;
            glm::vec3 getScale() const;
            glm::vec3 getRotation() const;
            bool isSelected() const;

            void setPosition(glm::vec3 position);
            void setScale(glm::vec3 scale);
            void setRotation(glm::vec3 rotation);
            void setHovered(bool isHovered);
            void setSelected(bool isSelected);
            virtual void draw(Camera &camera) = 0;
            virtual bool showImGui(int id) = 0;
    };
}

#endif // RENDER_OBJECT_HPP
