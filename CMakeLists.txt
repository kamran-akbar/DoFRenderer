cmake_minimum_required(VERSION 3.10)

# Set the project name, and specify C++ and Cuda as its languages (CXX, CUDA, ...)
project(DoFRender LANGUAGES CXX)


# Specify the C++ standard (C++ version 17)
set(CMAKE_CXX_STANDARD 				17)
set(CMAKE_CXX_STANDARD_REQUIRED 	ON)
set(CMAKE_CXX_EXTENSIONS      		OFF)

# Determine platform
if (WIN32)
	set(ISWINDOWS TRUE)
	message(STATUS "Found WINDOWS Platform!")
# UNIX-like OS's, including Apple OS X and CygWin
elseif(UNIX)
	set(ISWINDOWS FALSE)
	message(STATUS "Found UNIX Platform!")
	message(FATAL_ERROR "Platform not supported!")
else()
	message(FATAL_ERROR "Platform not supported!")
endif()

#========================================================================
# glm 
set(BUILD_SHARED_LIBS FALSE)
set(BUILD_STATIC_LIBS TRUE)
add_definitions(-D BUILD_STATIC_LIBS)
add_subdirectory(src/ext/glm/)

#========================================================================
# GLFW
set( GLFW_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS} )
unset(BUILD_SHARED_LIBS CACHE)
set (GLFW_BUILD_SHARED_LIBS	OFF CACHE BOOL "Build shared libraries" FORCE)
set (GLFW_BUILD_EXAMPLES	OFF CACHE BOOL "Build the GLFW example program" FORCE)
set (GLFW_BUILD_TESTS		OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set (GLFW_BUILD_DOCS		OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set (GLFW_INSTALL			OFF CACHE BOOL "Generate installation target" FORCE)
add_subdirectory(src/ext/glfw/)
unset(GLFW_USE_HYBRID_HPG CACHE)
unset(USE_MSVC_RUNTIME_LIBRARY_DLL CACHE)

#========================================================================
# Assimp
#set (BUILD_SHARED_LIBS         	ON CACHE BOOL "Build shared libs" FORCE)
#set (ASSIMP_BUILD_ASSIMP_TOOLS	OFF CACHE BOOL "Build supplementary tools for Assimp" FORCE)
#set (ASSIMP_BUILD_TESTS      	OFF CACHE BOOL "Build Assimp tests" FORCE)
#add_subdirectory(src/ext/assimp/) 

#========================================================================
# tinyXML-2 
#add_library(TinyXML STATIC src/ext/tinyxml2/tinyxml2.h src/ext/tinyxml2/tinyxml2.cpp)
#set_target_properties(TinyXML PROPERTIES OUTPUT_NAME tinyXML)

 

#========================================================================
# Define all core header files

# All header files
FILE ( GLOB DoF_RENDERER_HEADER
  include/DoFRenderer/core/*
)

# All source files
FILE ( GLOB DoF_RENDERER_SOURCE
  src/core/*
)

FILE ( GLOB DoF_SHADERS
  src/shaders/*
)

# Define a grouping for source files in IDE project generation
source_group("Header Files\\core" include/DoFRenderer/core/.*)
source_group("Source Files\\core" src/core/*)
source_group("Source Files\\shaders" src/shaders/*)

include_directories(include)

# Add external Include
include_directories(
	"src/shaders/"
	"src/ext/" #glm and glad
	"src/ext/glfw/include/" #glfw
)

# Adds DoFRenderer library target -> STATIC | SHARED | MODULE
add_library(DoF_Renderer_lib STATIC 
	${DoF_RENDERER_HEADER}
	${DoF_RENDERER_SOURCE}
	${DoF_SHADERS}
) 

# All libraries
set(ALL_DOF_RENDERER_LIBS
	DoF_Renderer_lib
	glm_static
	glfw
)

set_target_properties(DoF_Renderer_lib PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(DoF_Renderer_lib PROPERTIES OUTPUT_NAME DoFrendererlib)
#set_target_properties(${PROJECT_NAME} PROPERTIES ${CMAKE_CUDA_ARCHITECTURES} "75")
target_link_libraries(DoF_Renderer_lib 
	glm_static
	glfw
	#assimp
)

if(MSVC)
	add_definitions(
	"-D_CRT_SECURE_NO_WARNINGS"
	)
endif(MSVC)

# Main renderer
add_executable(DoF_Renderer_exe src/core/main.cpp)

set_target_properties(DoF_Renderer_exe PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(DoF_Renderer_exe PROPERTIES OUTPUT_NAME DoFrenderer)
#target_compile_features(DoF_Renderer_exe PRIVATE ${PBRT_CXX11_FEATURES})
target_link_libraries(DoF_Renderer_exe ${ALL_DOF_RENDERER_LIBS})

# Set DoF_Renderer_exe has startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DoF_Renderer_exe)


