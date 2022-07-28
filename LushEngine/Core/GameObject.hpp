#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <glm/glm.hpp>

#include "Includes.hpp"
#include "Packet.hpp"

namespace Lush
{
    class GameObject
    {
        private:
            int _type;
            std::string _name;
            glm::vec3 _position;
            glm::vec3 _scale;
            glm::vec3 _rotation;

        public:
            GameObject() = default;
            GameObject(int type, std::string name, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f),
                       glm::vec3 rotation = glm::vec3(0.0f));
            ~GameObject() = default;

            int getType() const;
            std::string getName() const;
            glm::vec3 getPos() const;
            glm::vec3 getScale() const;
            glm::vec3 getRotation() const;

            void setType(int type);
            void setName(std::string name);
            void setPos(glm::vec3 pos);
            void setScale(glm::vec3 scale);
            void setRotation(glm::vec3 rotation);

            void move(glm::vec3 speed);

            friend Packet &operator<<(Packet &packet, const GameObject &gameObject)
            {
                packet << gameObject._type << gameObject._name << gameObject._position << gameObject._scale
                       << gameObject._rotation;
                return packet;
            }
            friend Packet &operator>>(Packet &packet, GameObject &gameObject)
            {
                packet >> gameObject._type >> gameObject._name >> gameObject._position >> gameObject._scale
                       >> gameObject._rotation;
                return packet;
            }
    };
}

#endif // GAME_OBJECT_HPP
