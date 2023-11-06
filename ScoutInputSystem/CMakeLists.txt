# Local name for the module to avoid typos.
set(ModuleName ScoutInputSystem)
set(ImplementationName SdlInput)

# CMake parameters for the user to configure.
set(USE_${ImplementationName} OFF CACHE BOOL "Compile SDL implementation of Scout's input systems.")

# Declare appropriate defines according to user's configuration.
if(USE_${ImplementationName})
	add_compile_definitions(BUILD_WITH_${ImplementationName})
endif()

# Scout module's headers.
set(${ModuleName}_IncludePath ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/)

# Scout module's sources.
file(GLOB ${ModuleName}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/*.h)
file(GLOB ${ModuleName}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/*.cpp)

# Declaring the Scout module's static library target.
add_library(${ModuleName} STATIC ${${ModuleName}_Inc} ${${ModuleName}_Src})
set_target_properties(${ModuleName} PROPERTIES LINKER_LANGUAGE CXX) # Helps CMake out to figure out the language if it has trouble.
if(USE_${ImplementationName})
	file(GLOB ${ModuleName}_${ImplementationName}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/${ImplementationName}/*.h) # Implementation sources.
	file(GLOB ${ModuleName}_${ImplementationName}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/${ImplementationName}/*.cpp)
	target_sources(${ModuleName} PRIVATE ${${ModuleName}_${ImplementationName}_Inc} ${${ModuleName}_${ImplementationName}_Src})
	source_group(${ImplementationName}_Inc FILES ${${ModuleName}_${ImplementationName}_Inc}) # Source group for IDE filters that support it.
	source_group(${ImplementationName}_Src FILES ${${ModuleName}_${ImplementationName}_Src})
endif()

# Include paths for Scout module.
target_include_directories(${ModuleName} PUBLIC
	${${ModuleName}_IncludePath}
	${ScoutCommon_IncludePath}
)
if (USE_${ImplementationName})
target_include_directories(${ModuleName} PUBLIC
	${Sdl_IncludePath}
)
endif()

# Linking the Scout module.
target_link_libraries(${ModuleName} PUBLIC
	general ScoutCommon
)
if(USE_${ImplementationName})
target_link_libraries(${ModuleName} PUBLIC
	general ${Sdl_Libs}
)
endif()
