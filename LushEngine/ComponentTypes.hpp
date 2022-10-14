#ifndef COMPONENTTYPES_HPP
#define COMPONENTTYPES_HPP

#include "Includes.hpp"

enum InfoComp {
    TRANSFORM = 0b1,
    VELOCITY = 0b10,
    MODELID = 0b100,
    CAMERA = 0b1000,
    LIGHT = 0b10000,
};

typedef struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform() : position(0.0f), rotation(0.0f), scale(1.0f) {}
} Transform;

typedef struct Velocity {
    float x;
    float y;
    float z;

    Velocity() : x(0), y(0), z(0) {}
} Velocity;

typedef struct ModelID {
    std::size_t id;

    ModelID() : id(0) {}
} ModelID;

typedef struct CameraComponent {
    float fov;
    float near;
    float far;
    float sensitivity;

    CameraComponent() : fov(45.0f), near(0.1f), far(100.0f), sensitivity(1.0f) {}
} CameraComponent;

typedef struct Light {
    int mod;
    float intensity;
    glm::vec3 color;
    glm::vec3 direction;
    float cutOff;

    Light() : mod(0), intensity(1.0f), color(1.0f), direction(0.0f), cutOff(0.0f) {}
} Light;

#endif // COMPONENTTYPES_HPP
