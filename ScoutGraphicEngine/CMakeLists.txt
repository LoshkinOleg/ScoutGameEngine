# Local name for the module to avoid typos.
set(ModuleName ScoutGraphicEngine)
set(BespokeGlRendererImplementation BespokeGl)
set(SdlRendererImplementation SdlRenderer)

# CMake parameters for the user to configure.
# set(USE_${BespokeGlRendererImplementation} OFF CACHE BOOL "Compile Scout's bespoke graphical renderer implementation.") # Not implemented
set(USE_${SdlRendererImplementation} OFF CACHE BOOL "Compile SDL's renderer implementation.")

# Declare appropriate defines according to user's configuration.
if(USE_${BespokeGlRendererImplementation})
	add_compile_definitions(BUILD_WITH_${BespokeGlRendererImplementation})
endif()
if(USE_${SdlRendererImplementation})
	add_compile_definitions(BUILD_WITH_${SdlRendererImplementation})
	if(NOT USE_SdlInput)
		message(FATAL_ERROR "For SdlRenderer to work, the SdlInput implementation of ScoutInputSystem needs to be enabled.")
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
if(USE_${BespokeGlRendererImplementation}) # BespokeGl
	file(GLOB ${ModuleName}_${BespokeGlRendererImplementation}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/${BespokeGlRendererImplementation}/*.h) # Implementation sources.
	file(GLOB ${ModuleName}_${BespokeGlRendererImplementation}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/${BespokeGlRendererImplementation}/*.cpp)
	target_sources(${ModuleName} PRIVATE ${${ModuleName}_${BespokeGlRendererImplementation}_Inc} ${${ModuleName}_${BespokeGlRendererImplementation}_Src})
	source_group(${BespokeGlRendererImplementation}_Inc FILES ${${ModuleName}_${BespokeGlRendererImplementation}_Inc}) # Source group for IDE filters that support it.
	source_group(${BespokeGlRendererImplementation}_Src FILES ${${ModuleName}_${BespokeGlRendererImplementation}_Src})
endif()
if(USE_${SdlRendererImplementation}) # Sdl
	file(GLOB ${ModuleName}_${SdlRendererImplementation}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/${SdlRendererImplementation}/*.h) # Implementation sources.
	file(GLOB ${ModuleName}_${SdlRendererImplementation}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/${SdlRendererImplementation}/*.cpp)
	target_sources(${ModuleName} PRIVATE ${${ModuleName}_${SdlRendererImplementation}_Inc} ${${ModuleName}_${SdlRendererImplementation}_Src})
	source_group(${SdlRendererImplementation}_Inc FILES ${${ModuleName}_${SdlRendererImplementation}_Inc}) # Source group for IDE filters that support it.
	source_group(${SdlRendererImplementation}_Src FILES ${${ModuleName}_${SdlRendererImplementation}_Src})
endif()

# Include paths for Scout module.
target_include_directories(${ModuleName} PUBLIC
	${${ModuleName}_IncludePath}
	${ScoutCommon_IncludePath}
	${ScoutMath_IncludePath}
)
if (USE_${BespokeGlRendererImplementation}) # BespokeGl
target_include_directories(${ModuleName} PUBLIC
	${Glm_IncludePath}
	${Glad_IncludePath}
)
endif()
if (USE_${SdlRendererImplementation}) # Sdl
target_include_directories(${ModuleName} PUBLIC
	${Sdl_IncludePath}
	${ScoutInputSystem_IncludePath} # TODO: find a way to remove this coupling
)
endif()

# Linking the Scout module.
target_link_libraries(${ModuleName} PUBLIC
	general ScoutCommon
	general ScoutMath
)
if(USE_${BespokeGlRendererImplementation}) # BespokeGl
target_link_libraries(${ModuleName} PUBLIC
	general ${OpenGL_Libs}
	general ${Glm_Libs}
)
endif()
if(USE_${SdlRendererImplementation}) # Sdl
target_link_libraries(${ModuleName} PUBLIC
	general ${Sdl_Libs}
	general ScoutInputSystem # TODO: find a way to remove this coupling
)
endif()
