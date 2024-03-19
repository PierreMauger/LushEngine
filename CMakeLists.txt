cmake_minimum_required(VERSION 3.22)
project(LushEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set build output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Set the paths to your ECS and IMGUI roots
set(LIBS_PATH ${CMAKE_SOURCE_DIR}/libs)
set(IMGUI_ROOT ${CMAKE_SOURCE_DIR}/libs/ImGui)
set(ECS_ROOT ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/ECS)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Bullet REQUIRED)

include_directories(
    ${PROJECT_NAME}/
    ${LIBS_PATH}/
    ${BULLET_INCLUDE_DIRS}
)

# Add source files for both binaries
set(COMMON_SOURCE_FILES
    ${ECS_ROOT}/ECS.cpp
    ${ECS_ROOT}/Component/ScriptComponent.cpp
    ${ECS_ROOT}/Entity/Entity.cpp
    ${ECS_ROOT}/Entity/EntityManager.cpp
    ${ECS_ROOT}/System/ASystem.cpp
    ${ECS_ROOT}/System/SystemManager.cpp

    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_impl_glfw.cpp
    ${IMGUI_ROOT}/imgui_impl_opengl3.cpp
    ${IMGUI_ROOT}/ImGuizmo.cpp
    ${IMGUI_ROOT}/imgui_toast.cpp

    ${PROJECT_NAME}/Engine.cpp
    ${PROJECT_NAME}/Graphic.cpp
    ${PROJECT_NAME}/ResourceManager.cpp
    ${PROJECT_NAME}/Scene.cpp
    ${PROJECT_NAME}/UUID.cpp
    ${PROJECT_NAME}/Serialization.cpp
    ${PROJECT_NAME}/Rendering/Shapes.cpp
    ${PROJECT_NAME}/Rendering/Texture.cpp
    ${PROJECT_NAME}/Rendering/Skybox.cpp
    ${PROJECT_NAME}/Rendering/RenderView.cpp
    ${PROJECT_NAME}/Rendering/Shader.cpp
    ${PROJECT_NAME}/Rendering/Mesh.cpp
    ${PROJECT_NAME}/Rendering/MapMesh.cpp
    ${PROJECT_NAME}/Rendering/RenderModel.cpp
    ${PROJECT_NAME}/Systems/Camera/CameraSystem.cpp
    ${PROJECT_NAME}/Systems/Render/RenderSystem.cpp
    ${PROJECT_NAME}/Systems/Script/ScriptSystem.cpp
    ${PROJECT_NAME}/Systems/Physic/PhysicSystem.cpp
    ${PROJECT_NAME}/Script/ScriptClass.cpp
    ${PROJECT_NAME}/Script/ScriptGlue.cpp
    ${PROJECT_NAME}/Script/ScriptInstance.cpp
    ${PROJECT_NAME}/Script/ScriptPack.cpp
    ${PROJECT_NAME}/Physic/PhysicInstance.cpp
    ${PROJECT_NAME}/Physic/CharacterInstance.cpp
    ${PROJECT_NAME}/Physic/TerrainInstance.cpp
    ${PROJECT_NAME}/Physic/CustomContactCallback.cpp
    ${PROJECT_NAME}/File/File.cpp
    ${PROJECT_NAME}/File/Resource.cpp

    ${LIBS_PATH}/STB/stb.cpp
    ${PROJECT_NAME}/main.cpp
)

# Source files specific to lushEditor
set(EDITOR_SOURCE_FILES
    ${PROJECT_NAME}/Systems/FileWatcher/FileWatcherSystem.cpp
    ${PROJECT_NAME}/Systems/GUI/GUISystem.cpp
    ${PROJECT_NAME}/Systems/Picking/PickingSystem.cpp
    ${PROJECT_NAME}/Systems/Scene/SceneSystem.cpp
)

# Source files specific to lushGame
set(GAME_SOURCE_FILES
    ${PROJECT_NAME}/Systems/PostProcessing/PostProcessingSystem.cpp
)

# Common libraries
set(COMMON_LIBRARIES
    ${OPENGL_LIBRARIES}
    glfw
    ${GLEW_LIBRARIES}
    ${Boost_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${LIBS_PATH}/libmonosgen-2.0.so.1.0.0
    ${LIBS_PATH}/libassimp.so.5.2.0
)

# Add the lushEditor target
add_executable(lushEditor ${COMMON_SOURCE_FILES} ${EDITOR_SOURCE_FILES})
target_compile_features(lushEditor PRIVATE cxx_std_20)
target_compile_definitions(lushEditor PRIVATE EDITOR_MODE)
target_link_libraries(lushEditor ${COMMON_LIBRARIES})

# Add the lushGame target
add_executable(lushGame ${COMMON_SOURCE_FILES} ${GAME_SOURCE_FILES})
target_compile_features(lushGame PRIVATE cxx_std_20)
target_link_libraries(lushGame ${COMMON_LIBRARIES})
