cmake_minimum_required (VERSION 3.14)
project(ScoutGameEngine)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)

find_path(VCPKG_GENERATED_HEADERS_DIR "stb.h")
file(GLOB_RECURSE Core_sources ${PROJECT_SOURCE_DIR}/Core/source/*.cpp ${PROJECT_SOURCE_DIR}/Core/include/*.h)

# add_compile_definitions(XXH_INLINE_ALL)
add_compile_definitions(TINYGLTF_NO_STB_IMAGE)
add_compile_definitions(TINYGLTF_NO_INCLUDE_STB_IMAGE)
add_compile_definitions(TINYGLTF_NO_STB_IMAGE_WRITE)
add_compile_definitions(TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE)
add_compile_definitions(TINYGLTF_NO_EXTERNAL_IMAGE)
add_compile_definitions(TINYGLTF_NO_INCLUDE_JSON)

add_library(Core STATIC ${Core_sources})
target_include_directories(Core PUBLIC ${PROJECT_SOURCE_DIR}/Core/include/ ${VCPKG_GENERATED_HEADERS_DIR})
target_link_libraries(Core PUBLIC SDL2::SDL2 SDL2::SDL2main glad::glad ${OPENGL_LIBRARIES})

file(GLOB_RECURSE Game_sources ${PROJECT_SOURCE_DIR}/Game/source/*.cpp ${PROJECT_SOURCE_DIR}/Game/include/*.h)
file(GLOB_RECURSE Game_shaders ${PROJECT_SOURCE_DIR}/data/shaders/*.*)
add_library(Game STATIC ${Game_sources})
target_link_libraries(Game PUBLIC Core)

file(GLOB application_files ${PROJECT_SOURCE_DIR}/App/Application.cpp)
add_executable(Application ${application_files})
target_link_libraries(Application PRIVATE Game)

file(GLOB simpleMain_files ${PROJECT_SOURCE_DIR}/App/SimpleMain.cpp)
add_executable(SimpleMain ${simpleMain_files})
target_include_directories(SimpleMain PRIVATE ${VCPKG_GENERATED_HEADERS_DIR})
target_link_libraries(SimpleMain PRIVATE SDL2::SDL2 SDL2::SDL2main glad::glad ${OPENGL_LIBRARIES})