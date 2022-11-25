#ifndef COMPONENTTYPES_HPP
#define COMPONENTTYPES_HPP

#include "Includes.hpp"

enum ComponentType {
    TRANSFORM = 1 << 0,
    VELOCITY = 1 << 1,
    MODEL = 1 << 2,
    CAMERA = 1 << 3,
    LIGHT = 1 << 4,
    CONTROL = 1 << 5,
    CUBEMAP = 1 << 6,
    BILLBOARD = 1 << 7,
};

typedef struct {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
} Transform;

typedef struct {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
} Velocity;

typedef struct {
    std::size_t id = 0;
} Model;

// enum CameraType {
    // PERSPECTIVE,
    // ORTHOGRAPHIC,
// };

enum CameraMod {
    FIRST_PERSON,
    THIRD_PERSON,

    CAMERA_MOD_COUNT
};

typedef struct {
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    float fov = 45.0f;
    float near = 0.1f;
    float far = 100.0f;
    float sensitivity = 0.1f;
    CameraMod mod = FIRST_PERSON;
    float distance = 10.0f;
    std::size_t target = 0;
    bool alignTarget = false;
} Camera;

enum LightType {
    DIRECTIONAL,
    POINT,
    SPOT,
    AREA,

    LIGHT_TYPE_COUNT
};

typedef struct {
    LightType type = LightType::DIRECTIONAL;
    float intensity = 1.0f;
    glm::vec3 color = glm::vec3(1.0f);
    float cutOff = 0.0f;
} Light;

typedef struct {
    bool control = true;
    bool alignTarget = false;
} Control;

typedef struct {
    std::size_t id = 0;
    float rotationSpeed = 0.0f;
    glm::vec3 color = glm::vec3(1.0f);
} CubeMap;

typedef struct {
    std::size_t textureId = 0;
} BillBoard;

#endif // COMPONENTTYPES_HPP
