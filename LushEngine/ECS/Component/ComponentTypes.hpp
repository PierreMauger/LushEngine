#ifndef COMPONENTTYPES_HPP
#define COMPONENTTYPES_HPP

#include "Includes.hpp"

typedef struct Position {
    float x;
    float y;
    float z;

    Position() : x(10), y(0), z(0) {}
} Position;

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
