################## CONFIG ##################

#init Cmake
cmake_minimum_required(VERSION 3.22)
project(LushEngine LANGUAGES C CXX VERSION 1.0)

#set flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++20 -O3 -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Werror")
else()
    set(CMAKE_CXX_FLAGS "/std:c++20 /Ox /Wall /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /DEBUG:FULL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /WX")
endif()
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(CMAKE_THREADS_LIBS_INIT Threads::Threads)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(BUILD_SHARED_LIBS TRUE)
set(LIBS_PATH ${PROJECT_SOURCE_DIR}/libs)

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
    ${ECS_ROOT}/Component/SparseArray.cpp
    ${ECS_ROOT}/Component/ComponentManager.cpp
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
    ${PROJECT_NAME}/Rendering/Map.cpp
    ${PROJECT_NAME}/Rendering/RenderModel.cpp
    ${PROJECT_NAME}/Systems/Camera/CameraSystem.cpp
    ${PROJECT_NAME}/Systems/FileWatcher/FileWatcherSystem.cpp
    ${PROJECT_NAME}/Systems/GUI/GUISystem.cpp
    ${PROJECT_NAME}/Systems/Picking/PickingSystem.cpp
    ${PROJECT_NAME}/Systems/Render/RenderSystem.cpp
    ${PROJECT_NAME}/Systems/Scene/SceneSystem.cpp
    ${PROJECT_NAME}/Systems/Script/ScriptSystem.cpp
    ${PROJECT_NAME}/Systems/Game/GameSystem.cpp
    ${PROJECT_NAME}/Script/ScriptClass.cpp
    ${PROJECT_NAME}/Script/ScriptGlue.cpp
    ${PROJECT_NAME}/Script/ScriptInstance.cpp
    ${PROJECT_NAME}/Script/ScriptPack.cpp
    ${PROJECT_NAME}/File/File.cpp
    ${PROJECT_NAME}/File/Resource.cpp

    ${LIBS_PATH}/STB/stb.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${INC}
    ${OpenGL_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

add_library(${ENGINE} ${SRC})

target_link_libraries(${ENGINE} PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENGL_LIBRARIES}
    glfw
    ${GLEW_LIBRARY}
)

install(TARGETS ${ENGINE})

################## BUILD BINARIES ##################

set(EXEC_NAME lush)
set(EDITOR_NAME lushEditor)

add_executable(${EXEC_NAME}
    ${PROJECT_NAME}/main.cpp
)
target_compile_definitions(${EXEC_NAME} PRIVATE EDITOR_MODE=false)

target_link_libraries(${EXEC_NAME} PRIVATE
    ${ENGINE}
    ${ECS}
    ${IMGUI}
    ${LIBS_PATH}/libmonosgen-2.0.so.1.0.0
    ${LIBS_PATH}/libassimp.so.5.2.0
)

set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

add_executable(${EDITOR_NAME}
    ${PROJECT_NAME}/main.cpp
)
target_compile_definitions(${EDITOR_NAME} PRIVATE EDITOR_MODE=true)

target_link_libraries(${EDITOR_NAME} PRIVATE
    ${ENGINE}
    ${ECS}
    ${IMGUI}
    ${LIBS_PATH}/libmonosgen-2.0.so.1.0.0
    ${LIBS_PATH}/libassimp.so.5.2.0
)

set_target_properties(${EDITOR_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set_target_properties(${EDITOR_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

#install target
install(TARGETS ${EXEC_NAME})
install(TARGETS ${EDITOR_NAME})

################## POST BUILD ##################
