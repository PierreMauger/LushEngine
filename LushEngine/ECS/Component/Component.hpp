#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Includes.hpp"

namespace Lush
{
    class Component
    {
    };

    struct Transform : public Component {
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 rotation = glm::vec3(0.0f);
            glm::vec3 scale = glm::vec3(1.0f);
    };

    struct Model : public Component {
            std::string name = "Fox";
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
    };

    struct Cubemap : public Component {
            std::string name;

            Cubemap() : name("Sky") {}
    };

    struct Billboard : public Component {
            std::string name = "";
    };

    struct Map : public Component {
            std::string heightMap = "";
            std::string diffuseTexture = "";
            std::string normalTexture = "";
            std::string diffuseTexture2 = "";
            std::string diffuseTexture3 = "";
    };
}

#endif // COMPONENT_HPP
