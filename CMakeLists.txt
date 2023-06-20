################## CONFIG ##################

# Init CMake
cmake_minimum_required(VERSION 3.22)
project(LushEngine LANGUAGES C CXX VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type and optimizations
set(CMAKE_BUILD_TYPE Release)  # Use Release for optimized build

# Add options for different build types
if(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++20 -O3 -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Werror")
else()
    set(CMAKE_CXX_FLAGS "/std:c++20 /Ox /Wall /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /DEBUG:FULL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /WX")
endif()

# Set module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

# Find required packages
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Bullet REQUIRED)

set(LIBS_PATH ${CMAKE_SOURCE_DIR}/libs)

################## BUILD ECS ##################

set(ECS ECS)

set(ECS_ROOT ${PROJECT_NAME}/ECS/)

set(ECS_INC
    ${ECS_ROOT}/
    ${ECS_ROOT}/Component/
    ${ECS_ROOT}/Entity/
    ${ECS_ROOT}/System/
)

set(ECS_SRC
    ${ECS_ROOT}/ECS.cpp
    ${ECS_ROOT}/Component/ScriptComponent.cpp
    ${ECS_ROOT}/Entity/Entity.cpp
    ${ECS_ROOT}/Entity/EntityManager.cpp
    ${ECS_ROOT}/System/ASystem.cpp
    ${ECS_ROOT}/System/SystemManager.cpp
)

include_directories(${ECS_INC})

add_library(${ECS} ${ECS_SRC})

install(TARGETS ${ECS})

################## BUILD IMGUI ##################

set(IMGUI IMGUI)

set(IMGUI_ROOT ${LIBS_PATH}/ImGui/)

set(IMGUI_INC
    ${IMGUI_ROOT}/
)

set(IMGUI_SRC
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_impl_glfw.cpp
    ${IMGUI_ROOT}/imgui_impl_opengl3.cpp
    ${IMGUI_ROOT}/ImGuizmo.cpp
    ${IMGUI_ROOT}/imgui_toast.cpp
)

include_directories(${IMGUI_INC})

add_library(${IMGUI} ${IMGUI_SRC})

install(TARGETS ${IMGUI})

################## BUILD ENGINE ##################

set(ENGINE ENGINE)

set(INC
    ${PROJECT_NAME}/
    ${PROJECT_NAME}/Rendering/
    ${PROJECT_NAME}/Systems/Camera/
    ${PROJECT_NAME}/Systems/FileWatcher/
    ${PROJECT_NAME}/Systems/GUI/
    ${PROJECT_NAME}/Systems/Picking/
    ${PROJECT_NAME}/Systems/Render/
    ${PROJECT_NAME}/Systems/Scene/
    ${PROJECT_NAME}/Systems/Script/
    ${PROJECT_NAME}/Systems/Game/
    ${PROJECT_NAME}/Script/
    ${PROJECT_NAME}/File/

    ${LIBS_PATH}/
    ${LIBS_PATH}/ImGui/
    ${LIBS_PATH}/STB/
    ${LIBS_PATH}/mono/
    ${LIBS_PATH}/assimp/
    ${LIBS_PATH}/RapidXML/
)

set(SRC
    ${PROJECT_NAME}/Engine.cpp
    ${PROJECT_NAME}/Graphic.cpp
    ${PROJECT_NAME}/ResourceManager.cpp
    ${PROJECT_NAME}/Scene.cpp
    ${PROJECT_NAME}/UUID.cpp
    ${PROJECT_NAME}/Rendering/Shapes.cpp
    ${PROJECT_NAME}/Rendering/Texture.cpp
    ${PROJECT_NAME}/Rendering/CubeMap.cpp
    ${PROJECT_NAME}/Rendering/RenderView.cpp
    ${PROJECT_NAME}/Rendering/Shader.cpp
    ${PROJECT_NAME}/Rendering/Mesh.cpp
    ${PROJECT_NAME}/Rendering/MapMesh.cpp
    ${PROJECT_NAME}/Rendering/RenderModel.cpp
    ${PROJECT_NAME}/Systems/Camera/CameraSystem.cpp
    ${PROJECT_NAME}/Systems/FileWatcher/FileWatcherSystem.cpp
    ${PROJECT_NAME}/Systems/GUI/GUISystem.cpp
    ${PROJECT_NAME}/Systems/Picking/PickingSystem.cpp
    ${PROJECT_NAME}/Systems/Render/RenderSystem.cpp
    ${PROJECT_NAME}/Systems/Scene/SceneSystem.cpp
    ${PROJECT_NAME}/Systems/Script/ScriptSystem.cpp
    ${PROJECT_NAME}/Systems/Game/GameSystem.cpp
    ${PROJECT_NAME}/Systems/Physic/PhysicSystem.cpp
    ${PROJECT_NAME}/Script/ScriptClass.cpp
    ${PROJECT_NAME}/Script/ScriptGlue.cpp
    ${PROJECT_NAME}/Script/ScriptInstance.cpp
    ${PROJECT_NAME}/Script/ScriptPack.cpp
    ${PROJECT_NAME}/File/File.cpp
    ${PROJECT_NAME}/File/Resource.cpp

    ${LIBS_PATH}/STB/stb.cpp
)

include_directories(${INC})
include_directories(${BULLET_INCLUDE_DIR})

add_library(${ENGINE} ${SRC})

target_link_libraries(${ENGINE} PRIVATE
    Threads::Threads
    ${OPENGL_LIBRARIES}
    glfw
    ${GLEW_LIBRARY}
    ${Boost_LIBRARIES}
    ${BULLET_LIBRARIES}
)
install(TARGETS ${ENGINE})

################## BUILD BINARIES ##################

set(EXEC_NAME lush)
set(EDITOR_NAME lushEditor)

add_executable(${EXEC_NAME} ${PROJECT_NAME}/main.cpp)
target_compile_definitions(${EXEC_NAME} PRIVATE EDITOR_MODE=false)
target_link_libraries(${EXEC_NAME} PRIVATE
    ${ENGINE}
    ${ECS}
    ${IMGUI}
    ${Boost_LIBRARIES}
    ${LIBS_PATH}/libmonosgen-2.0.so.1.0.0
    ${LIBS_PATH}/libassimp.so.5.2.0
    ${CMAKE_DL_LIBS}  # Add support for dynamic loading
)
set_target_properties(${EXEC_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
)

add_executable(${EDITOR_NAME} ${PROJECT_NAME}/main.cpp)
target_compile_definitions(${EDITOR_NAME} PRIVATE EDITOR_MODE=true)
target_link_libraries(${EDITOR_NAME} PRIVATE
    ${ENGINE}
    ${ECS}
    ${IMGUI}
    ${Boost_LIBRARIES}
    ${LIBS_PATH}/libmonosgen-2.0.so.1.0.0
    ${LIBS_PATH}/libassimp.so.5.2.0
    ${CMAKE_DL_LIBS}  # Add support for dynamic loading
)
set_target_properties(${EDITOR_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}
)

# Install targets
install(TARGETS ${EXEC_NAME} ${EDITOR_NAME})

################## POST BUILD ##################
