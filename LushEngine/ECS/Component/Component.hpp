#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Includes.hpp"

namespace Lush
{
    struct Component {
            virtual Component *clone() const = 0;
    };

    struct Transform : public Component {
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 rotation = glm::vec3(0.0f);
            glm::vec3 scale = glm::vec3(1.0f);

            Transform *clone() const override
            {
                return new Transform(*this);
            }
    };

    struct Model : public Component {
            std::string name = "Fox";

            Model *clone() const override
            {
                return new Model(*this);
            }
    };

    // enum CameraType {
    // PERSPECTIVE,
    // ORTHOGRAPHIC,
    // };

    struct Camera : public Component {
            glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
            float fov = 45.0f;
            float near = 0.1f;
            float far = 100.0f;

            Camera *clone() const override
            {
                return new Camera(*this);
            }
    };

    enum LightType {
        DIRECTIONAL,
        POINT,
        SPOT,
        AREA,

        LIGHT_TYPE_COUNT
    };

    struct Light : public Component {
            LightType type = LightType::DIRECTIONAL;
            float intensity = 1.0f;
            glm::vec3 color = glm::vec3(1.0f);
            float cutOff = 0.0f;

            Light *clone() const override
            {
                return new Light(*this);
            }
    };

    struct Cubemap : public Component {
            std::string name = "Sky";

            Cubemap *clone() const override
            {
                return new Cubemap(*this);
            }
    };

    struct Billboard : public Component {
            std::string name = "";

            Billboard *clone() const override
            {
                return new Billboard(*this);
            }
    };

    struct Map : public Component {
            std::string heightMap = "";
            std::string diffuseTexture = "";
            std::string normalTexture = "";
            std::string diffuseTexture2 = "";
            std::string diffuseTexture3 = "";

            Map *clone() const override
            {
                return new Map(*this);
            }
    };

    struct RigidBody : public Component {
            float mass = 1.0f;
            float friction = 0.5f;
            float restitution = 0.0f;
            bool kinematic = false;

            RigidBody *clone() const override
            {
                return new RigidBody(*this);
            }
    };

    struct CharacterController : public Component {
            glm::vec3 center = glm::vec3(0.0f);
            float slopeLimit = 45.0f;
            float stepOffset = 1.0f;

            CharacterController *clone() const override
            {
                return new CharacterController(*this);
            }
    };
}

#endif // COMPONENT_HPP
