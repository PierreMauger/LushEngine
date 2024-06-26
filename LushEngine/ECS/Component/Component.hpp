#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Includes.hpp"

namespace Lush
{
    struct Component {
            [[nodiscard]] virtual Component *clone() const = 0;
            template <class Archive> void serialize([[maybe_unused]] Archive &ar, [[maybe_unused]] const unsigned int version){};
    };

    struct Transform : public Component {
            glm::vec3 position = glm::vec3(0.0f);
            glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 scale = glm::vec3(1.0f);

            [[nodiscard]] Transform *clone() const override
            {
                return new Transform(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &position;
                ar &rotation;
                ar &scale;
            }
    };

    struct Material {
            glm::vec3 diffuse = glm::vec3(1.0f);
            glm::vec3 ambient = glm::vec3(0.0f);
            glm::vec3 specular = glm::vec3(0.0f);
            glm::vec3 emission = glm::vec3(0.0f);
            float shininess = 32.0f;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &diffuse;
                ar &ambient;
                ar &specular;
                ar &emission;
                ar &shininess;
            }
    };

    struct Model : public Component {
            std::string name = "Fox";
            bool culling = true;
            std::unordered_map<std::string, Material> materials;
            std::vector<std::string> textures;

            Model *clone() const override
            {
                return new Model(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &name;
                ar &culling;
                ar &materials;
                ar &textures;
            }
    };

    enum CameraType {
        PERSPECTIVE,
        ORTHOGRAPHIC,

        CAMERA_TYPE_COUNT
    };

    struct Camera : public Component {
            CameraType type = CameraType::PERSPECTIVE;
            glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
            float fov = 45.0f;
            float near = 0.1f;
            float far = 100.0f;

            [[nodiscard]] Camera *clone() const override
            {
                return new Camera(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &type;
                ar &forward;
                ar &fov;
                ar &near;
                ar &far;
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
            // directional light
            glm::vec3 shadowSize = glm::vec3(20.0f);
            // point light
            float intensity = 1.0f;
            glm::vec3 color = glm::vec3(1.0f);
            // spot light
            float cutOff = 0.0f;

            [[nodiscard]] Light *clone() const override
            {
                return new Light(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &type;
                ar &shadowSize;
                ar &intensity;
                ar &color;
                ar &cutOff;
            }
    };

    struct Cubemap : public Component {
            std::string name = "LearnOpenGL";

            [[nodiscard]] Cubemap *clone() const override
            {
                return new Cubemap(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &name;
            }
    };

    struct Billboard : public Component {
            std::string name;
            bool lockYAxis = false;

            [[nodiscard]] Billboard *clone() const override
            {
                return new Billboard(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &name;
                ar &lockYAxis;
            }
    };

    struct Map : public Component {
            std::string heightMap;
            std::string diffuseTexture;
            std::string normalTexture;
            std::string diffuseTexture2;
            std::string diffuseTexture3;

            [[nodiscard]] Map *clone() const override
            {
                return new Map(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &heightMap;
                ar &diffuseTexture;
                ar &normalTexture;
                ar &diffuseTexture2;
                ar &diffuseTexture3;
            }
    };

    struct RigidBody : public Component {
            float mass = 1.0f;
            float friction = 0.5f;
            float restitution = 0.0f;
            bool kinematic = false;

            [[nodiscard]] RigidBody *clone() const override
            {
                return new RigidBody(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &mass;
                ar &friction;
                ar &restitution;
                ar &kinematic;
            }
    };

    enum ColliderType {
        BOX,
        SPHERE,
        CAPSULE,
        MESH,

        COLLIDER_TYPE_COUNT
    };

    struct Collider : public Component {
            ColliderType type = ColliderType::BOX;
            glm::vec3 center = glm::vec3(0.0f);
            glm::vec3 size = glm::vec3(0.0f);
            std::string tag;

            [[nodiscard]] Collider *clone() const override
            {
                return new Collider(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &type;
                ar &center;
                ar &size;
                ar &tag;
            }
    };

    struct CharacterController : public Component {
            glm::vec3 center = glm::vec3(0.0f);
            float slopeLimit = 45.0f;
            float stepOffset = 1.0f;

            [[nodiscard]] CharacterController *clone() const override
            {
                return new CharacterController(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &center;
                ar &slopeLimit;
                ar &stepOffset;
            }
    };

    struct UIElement : public Component {
            glm::vec2 size = glm::vec2(100.0f);
            glm::vec2 offset = glm::vec2(0.0f);
            int anchor = 0;
            std::string textureName;

            [[nodiscard]] UIElement *clone() const override
            {
                return new UIElement(*this);
            }

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &size;
                ar &offset;
                ar &anchor;
                ar &textureName;
            }
    };
}

#endif // COMPONENT_HPP
