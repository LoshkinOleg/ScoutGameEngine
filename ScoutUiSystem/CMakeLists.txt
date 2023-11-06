# Expects the following variables:
# 	ScoutGraphicEngine_IncludePath (from ScoutGraphicEngine CMakeLists)
# 	ScoutGraphicEngine (from ScoutGraphicEngine CMakeLists)
# Expects the following variables if compiling ImGuiSdlOpenGlRenderer implementation
# 	USE_SdlRenderer (from ScoutGraphicEngine CMakeLists)
# 	Thirdparty_ImGui
#	Thirdparty_ImGui_SdlRendererBackend
#	ImGui_IncludePath
#	ImGui_Backends_IncludePath

# Local name for the module to avoid typos.
set(ModuleName ScoutUiSystem)
set(ImGuiImplementation ImGuiSdlOpenGlRenderer)

# CMake parameters for the user to configure.
set(USE_${ImGuiImplementation} OFF CACHE BOOL "Whether to compile ImGui implementation of the UI system. Depends on SDL implementation of graphical renderer.")

# Declare appropriate defines according to user's configuration.
if(USE_${ImGuiImplementation})
	add_compile_definitions(BUILD_WITH_${ImGuiImplementation})
	if(NOT USE_SdlRenderer)
		message(FATAL_ERROR "For ImGuiSdlOpenGlRenderer to work, SdlRenderer implementation of ScoutGraphicEngine needs to be enabled.")
	endif()
endif()

# Scout module's headers.
set(${ModuleName}_IncludePath ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/)

# Scout module's sources.
file(GLOB ${ModuleName}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/*.h)
file(GLOB ${ModuleName}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/*.cpp)

# Declaring the Scout module's static library target.
add_library(${ModuleName} STATIC ${${ModuleName}_Inc} ${${ModuleName}_Src})
set_target_properties(${ModuleName} PROPERTIES LINKER_LANGUAGE CXX) # Helps CMake out to figure out the language if it has trouble.
if(USE_${ImGuiImplementation})
	file(GLOB ${ModuleName}_${ImGuiImplementation}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/${ImGuiImplementation}/*.h) # Implementation sources.
	file(GLOB ${ModuleName}_${ImGuiImplementation}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/${ImGuiImplementation}/*.cpp)
	target_sources(${ModuleName} PRIVATE ${${ModuleName}_${ImGuiImplementation}_Inc} ${${ModuleName}_${ImGuiImplementation}_Src})
	source_group(${ImGuiImplementation}_Inc FILES ${${ModuleName}_${ImGuiImplementation}_Inc}) # Source group for IDE filters that support it.
	source_group(${ImGuiImplementation}_Src FILES ${${ModuleName}_${ImGuiImplementation}_Src})
endif()

# Include paths for Scout module.
target_include_directories(${ModuleName} PUBLIC
	${${ModuleName}_IncludePath}
	${ScoutGraphicEngine_IncludePath}
	${ScoutInputSystem_IncludePath}
)
if (USE_${ImGuiImplementation})
target_include_directories(${ModuleName} PUBLIC
	${ImGui_IncludePath}
	${ImGui_Backends_IncludePath}
)
endif()

# Linking the Scout module.
target_link_libraries(${ModuleName} PUBLIC
	general ScoutGraphicEngine
	general ScoutInputSystem
)
if(USE_${ImGuiImplementation})
target_link_libraries(${ModuleName} PUBLIC
	general Thirdparty_ImGui
	general Thirdparty_ImGui_SdlRendererBackend
)
endif()
