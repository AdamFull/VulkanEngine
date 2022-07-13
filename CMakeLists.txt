cmake_minimum_required(VERSION 3.19.1)
# Initializing project
project(IncenerateRenderEngine)

option(VULKAN_API_SUPPORT "Enable vulkan api support" ON)
option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)
option(OPENGL_API_SUPPORT "Enable opengl api support" ON)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(MSVC)
	add_compile_options(/W4 /bigobj -D_WIN32_WINNT=0x0601)
else()
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
endif()

file(GLOB_RECURSE CPP_FILES_CORE ${CMAKE_CURRENT_SOURCE_DIR}/source/core/*.cpp)
file(GLOB_RECURSE CPP_FILES_EDITOR ${CMAKE_CURRENT_SOURCE_DIR}/source/editor/*.cpp)
file(GLOB_RECURSE CPP_FILES_UTIL ${CMAKE_CURRENT_SOURCE_DIR}/source/util/*.cpp)
file(GLOB_RECURSE CPP_FILES_FS ${CMAKE_CURRENT_SOURCE_DIR}/source/filesystem/*.cpp)
file(GLOB_RECURSE CPP_FILES_SERIALIZE ${CMAKE_CURRENT_SOURCE_DIR}/source/serializer/*.cpp)
file(GLOB_RECURSE CPP_FILES_SCRIPTING ${CMAKE_CURRENT_SOURCE_DIR}/source/scripting/*.cpp)
file(GLOB_RECURSE H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.h)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.hpp)
file(GLOB_RECURSE CPP_FILES_IMGUI ${CMAKE_CURRENT_SOURCE_DIR}/source/external/imgui/*.cpp)

set(CPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CPP_FILES_CORE}
	${CPP_FILES_EDITOR}
	${CPP_FILES_UTIL}
	${CPP_FILES_FS}
	${CPP_FILES_SERIALIZE}
    ${CPP_FILES_SCRIPTING}
	${CPP_FILES_IMGUI}
)

add_executable(${PROJECT_NAME} ${CPP_FILES} ${H_FILES} ${HPP_FILES})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/KTX-Software)
set(KTX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/KTX-Software/include)

if(VULKAN_API_SUPPORT)
	find_package(Vulkan REQUIRED)
	message(${Vulkan_LIBRARIES})
    target_compile_definitions(${PROJECT_NAME} PUBLIC VULKAN_API_SUPPORT)
else()
    set(VULKAN_LIBRARIES "")
    set(VULKAN_INCLUDE_DIRS "")
endif(VULKAN_API_SUPPORT)

if(OPENGL_API_SUPPORT)
    find_package(OpenGL REQUIRED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC OPENGL_API_SUPPORT)
else()
    set(OPENGL_LIBRARIES "")
    set(OPENGL_INCLUDE_DIR "")
endif(OPENGL_API_SUPPORT)

find_package(glfw3 REQUIRED)
find_package(glslang REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(glm REQUIRED)

# Link the debug and release libraries to the project
target_link_libraries( ${PROJECT_NAME} PUBLIC 
    ${Vulkan_LIBRARIES}
    ${OPENGL_LIBRARIES}
    glfw
	glslang
	SPIRV
    spirv-cross-core
    spirv-cross-glsl
    imgui
    ktx
)

target_include_directories( ${PROJECT_NAME} PRIVATE 
    ${Vulkan_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/ChaiScript/include
    ${CMAKE_CURRENT_SOURCE_DIR}/source/
    ${CMAKE_CURRENT_SOURCE_DIR}/source/core/
    ${IMGUI_INCLUDE_DIR}
    ${KTX_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

target_precompile_headers( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common.h)
