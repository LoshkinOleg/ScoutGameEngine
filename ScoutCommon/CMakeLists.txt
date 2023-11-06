# Local name for the module to avoid typos.
set(ModuleName ScoutCommon)

# Scout module's headers.
set(${ModuleName}_IncludePath ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/)

# Scout module's sources.
file(GLOB ${ModuleName}_Inc ${PROJECT_SOURCE_DIR}/${ModuleName}/Include/Scout/*.h)
file(GLOB ${ModuleName}_Src ${PROJECT_SOURCE_DIR}/${ModuleName}/Src/*.cpp)

# Declaring the Scout module's static library target.
add_library(${ModuleName} STATIC ${${ModuleName}_Inc} ${${ModuleName}_Src})
set_target_properties(${ModuleName} PROPERTIES LINKER_LANGUAGE CXX) # Helps CMake out to figure out the language if it has trouble.

# Include paths for Scout module.
target_include_directories(${ModuleName} PUBLIC
	${${ModuleName}_IncludePath}
)

# Linking the Scout module.
target_link_libraries(${ModuleName} PUBLIC
	
)
