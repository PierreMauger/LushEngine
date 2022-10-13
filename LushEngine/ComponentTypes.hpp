#ifndef COMPONENTTYPES_HPP
#define COMPONENTTYPES_HPP

#include "Includes.hpp"

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

#endif // COMPONENTTYPES_HPP
