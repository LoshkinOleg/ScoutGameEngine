cmake_minimum_required (VERSION 3.14)
project(ScoutGameEngine)

# Using C++20 .
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Make an out-of-source CMake build under /build for relative paths to work as intended.
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/build/")
	message(FATAL_ERROR "Please specify an out-of-source directory ‘build/’ in the project’s root directory. If you don’t know what an out-of-source build is, here’s a link: https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html")
endif()

# Detect platform.
if (APPLE)
	message(FATAL_ERROR "Scout's development on Apple platforms isn't supported.")
endif()
if (UNIX AND NOT APPLE)
	set(LINUX TRUE)
endif()

# CMake parameters. Any parameters following the pattern USE_<functionality> should have a BUILD_WITH_<functionality> define in the code.
set(USE_EASY_PROFILER OFF CACHE BOOL "Whether to enable profiling with easy_profiler.")
if (USE_EASY_PROFILER)
	add_compile_definitions(BUILD_WITH_EASY_PROFILER)
	file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/Build/ProfilerOutputs)
	if(LINUX)
		message(WARNING "Easy profiler has trouble tracking context switching on Linux, don't rely on it to profile multithreaded code.")
	endif()
endif()

# Defines needed by third party code.
add_compile_definitions(GLM_FORCE_RADIANS ON)

# Prebuilt thirdparty libraries' paths.
if(WIN32)

set(EasyProfiler_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/easy_profiler/lib/windows/easy_profiler.lib
	)
set(Portaudio_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/portaudio/lib/windows/portaudio_static_x64.lib
	)
set(Sdl_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/SDL/lib/windows/SDL2.lib
	)
set(OpenGL_Libs opengl32.lib
	)
set(Glm_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/glm/lib/windows/glm_static.lib
	)

endif()
if(LINUX)

set(EasyProfiler_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/easy_profiler/lib/linux/libeasy_profiler.so
	)
set(Portaudio_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/portaudio/lib/linux/libportaudio.so
	)
set(Sdl_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/SDL/lib/linux/libSDL2-2.0.so.0
	)
set(OpenGL_Libs libGL.so
	)
set(Glm_Libs
	${PROJECT_SOURCE_DIR}/Thirdparty/glm/lib/linux/libglm_shared.so
	)

endif()

# Thirdparty headers locations.
set(DrWav_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/dr_wav/include/)
set(EasyProfiler_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/easy_profiler/include/)
set(ImGui_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/include/)
set(ImGui_Backends_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/include/impl/)
set(Portaudio_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/portaudio/include/)
set(Sdl_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/SDL/include/)
set(Glad_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/glad/include/)
set(Glm_IncludePath ${PROJECT_SOURCE_DIR}/Thirdparty/glm/include/)

# Thirdparty's sources.
file(GLOB_RECURSE DrWav_Inc ${PROJECT_SOURCE_DIR}/Thirdparty/dr_wav/include/*.h)
file(GLOB ImGui_Inc ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/include/*.h)
file(GLOB ImGui_Src ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/src/*.cpp)
file(GLOB ImGui_SdlRendererBackend_Inc ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/include/impl/imgui_impl_sdlrenderer.h ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/include/impl/imgui_impl_sdl.h)
file(GLOB ImGui_SdlRendererBackend_Src ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/src/impl/imgui_impl_sdlrenderer.cpp ${PROJECT_SOURCE_DIR}/Thirdparty/imgui/src/impl/imgui_impl_sdl.cpp)
file(GLOB_RECURSE Glad_Inc ${PROJECT_SOURCE_DIR}/Thirdparty/glad/include/**/*.h)
file(GLOB_RECURSE Glad_Src ${PROJECT_SOURCE_DIR}/Thirdparty/glad/src/*.c)

# Thrid party libraries targets.
add_library(Thirdparty_Glad STATIC ${Glad_Inc} ${Glad_Src})
set_target_properties(Thirdparty_Glad PROPERTIES LINKER_LANGUAGE CXX)
add_library(Thirdparty_ImGui STATIC ${ImGui_Inc} ${ImGui_Src})
set_target_properties(Thirdparty_ImGui PROPERTIES LINKER_LANGUAGE CXX)
add_library(Thirdparty_ImGui_SdlRendererBackend STATIC ${ImGui_SdlRendererBackend_Inc} ${ImGui_SdlRendererBackend_Src})
set_target_properties(Thirdparty_ImGui_SdlRendererBackend PROPERTIES LINKER_LANGUAGE CXX)

# Include directories for third party libraries targets.
target_include_directories(Thirdparty_Glad PUBLIC
	${Glad_IncludePath}
)
target_include_directories(Thirdparty_ImGui PUBLIC
	${ImGui_IncludePath}
)
target_include_directories(Thirdparty_ImGui_SdlRendererBackend PUBLIC
	${ImGui_Backends_IncludePath}
	${ImGui_IncludePath}
	${Sdl_IncludePath}
)

# Linking third party libraries.
target_link_libraries(Thirdparty_ImGui_SdlRendererBackend PUBLIC
	general Thirdparty_ImGui
)

# Scout modules.
add_subdirectory(ScoutCommon)
add_subdirectory(ScoutMath)
add_subdirectory(ScoutString)
add_subdirectory(ScoutAssetSystem)
add_subdirectory(ScoutGraphicEngine)
add_subdirectory(ScoutInputSystem)
add_subdirectory(ScoutAudioEngine)
add_subdirectory(ScoutUiSystem)

# Application's headers location.
set(Application_IncludePath ${PROJECT_SOURCE_DIR}/Application/Include/)

# Application's sources.
file(GLOB Application_Inc ${PROJECT_SOURCE_DIR}/Application/Include/*.h)
file(GLOB Application_Src ${PROJECT_SOURCE_DIR}/Application/Src/*.cpp)

# Application's executable declaration.
add_executable(Application ${Application_Inc} ${Application_Src})
set_target_properties(Application PROPERTIES LINKER_LANGUAGE CXX)

# Application's include directories.
target_include_directories(Application PUBLIC
	${ScoutCommon_IncludePath}
	${ScoutMath_IncludePath}
	${ScoutGraphicEngine_IncludePath}
	${ScoutInputSystem_IncludePath}
	${ScoutAudioEngine_IncludePath}
	${ScoutAssetSystem_IncludePath}
	${ScoutUiSystem_IncludePath}
)

# Linking specifications for Application.
target_link_libraries(Application PUBLIC
	general ScoutCommon
	general ScoutMath
	general ScoutGraphicEngine
	general ScoutInputSystem
	general ScoutAudioEngine
	general ScoutAssetSystem
	general ScoutUiSystem
)
