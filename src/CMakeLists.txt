cmake_minimum_required(VERSION 3.8)

project(GLESWebSamples)

add_compile_options(-std=c++11)

if(EMSCRIPTEN)
	add_definitions(-DEMSCRIPTEN)
	add_definitions(-DGLFW_INCLUDE_ES2)
	SET(CMAKE_EXECUTABLE_SUFFIX ".html")	
	#SET(CMAKE_CXX_FLAGS "-s WASM=1 -O2 --preload-file grass.bmp")	
else()
	add_definitions(-DGLEW_STATIC)
	add_definitions(-DGLEW_NO_GLU)
	
	find_package(OpenGL)

	set(OPEN_SOURCE_DIR "E:/opensource/")
	set(GLFW_INCLUDE_DIR "${OPEN_SOURCE_DIR}glfw/include" CACHE PATH "glfw include dir")
	set(GLEW_INCLUDE_DIR "${OPEN_SOURCE_DIR}glew/glew_110_include" CACHE PATH "glew include dir")

	set(GLFW_LIBRARY_PATH "${OPEN_SOURCE_DIR}glfw/lib/Win32/Debug/" CACHE PATH "glfw lib path")
	set(GLEW_LIBRARY_PATH "${OPEN_SOURCE_DIR}glew/glew_110_lib/Win32/Debug/" CACHE PATH "glew lib path")

	find_library(GLFW_LIBRARY NAMES glfw3 PATHS ${GLFW_LIBRARY_PATH})
	find_library(GLEW_LIBRARY NAMES glew PATHS ${GLEW_LIBRARY_PATH}) 
endif()

add_library(Common STATIC
Common/GLSampleBase.cpp
Common/GLSampleBase.h
)

set(Common_INCLUDE_DIR "Common/")

if(NOT EMSCRIPTEN)
	target_include_directories(Common PRIVATE ${GLFW_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
	target_link_libraries(Common ${GLEW_LIBRARY} ${GLFW_LIBRARY} ${OPENGL_gl_LIBRARY})
endif()

MACRO(SETUP_SAMPLE_START EXE_NAME)
	UNSET(SOURCE_FILES)
	UNSET(HEADER_FILES)
	UNSET(RESOURCE_FILES)
	UNSET(EFFECT_FILES)
ENDMACRO(SETUP_SAMPLE_START)

MACRO(SETUP_SAMPLE_END EXE_NAME)
	SOURCE_GROUP("Source Files" FILES ${SOURCE_FILES})
	SOURCE_GROUP("Header Files" FILES ${HEADER_FILES})
	SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES})
	SOURCE_GROUP("Effect Files" FILES ${EFFECT_FILES})
	
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${EXE_NAME})
	
	ADD_EXECUTABLE(${EXE_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES} ${EFFECT_FILES})

	if(EMSCRIPTEN)
		SET_TARGET_PROPERTIES(${EXE_NAME} PROPERTIES LINK_FLAGS "-s USE_GLFW=3")
		target_include_directories(${EXE_NAME} PRIVATE ${Common_INCLUDE_DIR})	
		target_link_libraries(${EXE_NAME} Common)
	else()
		target_include_directories(${EXE_NAME} PRIVATE ${Common_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})	
		target_link_libraries(${EXE_NAME} Common ${GLEW_LIBRARY} ${GLFW_LIBRARY} ${OPENGL_gl_LIBRARY})
	endif()

ENDMACRO(SETUP_SAMPLE_END)

SETUP_SAMPLE_START(HelloGL)
	SET(SOURCE_FILES
		HelloGL/HelloGL.cpp
	)	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/HelloGL/grass.bmp ${CMAKE_CURRENT_BINARY_DIR}/HelloGL/grass.bmp COPYONLY) 
	if(EMSCRIPTEN)
		SET(CMAKE_CXX_FLAGS "--embed-file HelloGL/grass.bmp")	
	endif()
SETUP_SAMPLE_END(HelloGL)


SETUP_SAMPLE_START(HelloTriangle)
	SET(SOURCE_FILES
		HelloTriangle/HelloTriangle.cpp
	)	
SETUP_SAMPLE_END(HelloTriangle)

SETUP_SAMPLE_START(UniformsSample)
	SET(SOURCE_FILES
		UniformsSample/UniformsSample.cpp
	)	
SETUP_SAMPLE_END(UniformsSample)

SETUP_SAMPLE_START(VertexAttributesSample)
	SET(SOURCE_FILES
		VertexAttributesSample/VertexAttributesSample.cpp
	)	
SETUP_SAMPLE_END(VertexAttributesSample)
