CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(ASSIMP GIT_REPOSITORY https://github.com/assimp/assimp.git GIT_TAG v5.2.5 GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(ASSIMP)
