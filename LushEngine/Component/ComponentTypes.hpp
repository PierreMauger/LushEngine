#ifndef COMPONENTTYPES_HPP
#define COMPONENTTYPES_HPP

#include "Includes.hpp"

typedef struct {
    float x;
    float y;
    float z;
} Position;

typedef struct {
    float x;
    float y;
    float z;
} Velocity;

typedef struct {
    std::size_t id;
} ModelID;

#endif // COMPONENTTYPES_HPP
