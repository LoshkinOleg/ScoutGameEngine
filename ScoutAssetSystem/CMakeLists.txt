# Local name for the module to avoid typos.
set(ModuleName ScoutAssetSystem)
set(DrWavImplementation DrWav)

# CMake parameters for the user to configure.
set(USE_${DrWavImplementation} OFF CACHE BOOL "Compile dr_wav implementation for .wav files IO")

# Declare appropriate defines according to user's configuration.
if(USE_${DrWavImplementation})
	add_compile_definitions(BUILD_WITH_${DrWavImplementation})
endif()

# Scout module's headers.
set(${ModuleName}_IncludePath ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/)

# Scout module's sources.
file(GLOB ${ModuleName}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/*.h)
file(GLOB ${ModuleName}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/*.cpp)

# Declaring the Scout module's static library target.
add_library(${ModuleName} STATIC ${${ModuleName}_Inc} ${${ModuleName}_Src})
set_target_properties(${ModuleName} PROPERTIES LINKER_LANGUAGE CXX) # Helps CMake out to figure out the language if it has trouble.
if(USE_${DrWavImplementation})
	file(GLOB ${ModuleName}_${DrWavImplementation}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/${DrWavImplementation}/*.h) # Implementation sources.
	file(GLOB ${ModuleName}_${DrWavImplementation}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/${DrWavImplementation}/*.cpp)
	target_sources(${ModuleName} PRIVATE ${${ModuleName}_${DrWavImplementation}_Inc} ${${ModuleName}_${DrWavImplementation}_Src})
	source_group(${DrWavImplementation}_Inc FILES ${${ModuleName}_${DrWavImplementation}_Inc}) # Source group for IDE filters that support it.
	source_group(${DrWavImplementation}_Src FILES ${${ModuleName}_${DrWavImplementation}_Src})
endif()

# Include paths for Scout module.
target_include_directories(${ModuleName} PUBLIC
	${${ModuleName}_IncludePath}
	${ScoutCommon_IncludePath}
	${ScoutString_IncludePath}
)
if (USE_${DrWavImplementation})
target_include_directories(${ModuleName} PUBLIC
	${DrWav_IncludePath}
)
endif()

# Linking the Scout module.
target_link_libraries(${ModuleName} PUBLIC
	general ScoutCommon
	general ScoutString
)
