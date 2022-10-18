#ifndef COMPONENTTYPES_HPP
#define COMPONENTTYPES_HPP

#include "Includes.hpp"

enum CompInfo {
    TRANSFORM = 0b1,
    VELOCITY = 0b10,
    MODELID = 0b100,
    CAMERA = 0b1000,
    LIGHT = 0b10000,
};

typedef struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
} Transform;

typedef struct Velocity {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
} Velocity;

typedef struct ModelID {
    std::size_t id = 0;
} ModelID;

typedef struct CameraComponent {
    float fov = 45.0f;
    float near = 0.1f;
    float far = 100.0f;
    float sensitivity = 0.1f;
} CameraComponent;

typedef struct Light {
    int mod = 0;
    float intensity = 1.0f;
    glm::vec3 color = glm::vec3(1.0f);
    float cutOff = 0.0f;
} Light;

#endif // COMPONENTTYPES_HPP
