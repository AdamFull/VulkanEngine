cmake_minimum_required(VERSION 3.19.1)

option(AUTO_LOCATE_VULKAN "AUTO_LOCATE_VULKAN" ON)
if(AUTO_LOCATE_VULKAN)
	message(STATUS "Attempting auto locate Vulkan using CMake......")
	
	# Find Vulkan Path using CMake's Vulkan Module
	# This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
	# Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
	find_package(Vulkan)
	
	# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
	if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
		set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
		STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
	endif()
		 
	if(NOT Vulkan_FOUND)
		# CMake may fail to locate the libraries but could be able to 
		# provide some path in Vulkan SDK include directory variable
		# 'Vulkan_INCLUDE_DIRS', try to extract path from this.
		message(STATUS "Failed to locate Vulkan SDK, retrying again...")
		if(EXISTS "${VULKAN_PATH}")
			message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
		else()
			message("Error: Unable to locate Vulkan SDK. Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'")
			message("and specify manually path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in the CMakeLists.txt.")
			return()
		endif()
	endif()
else()
	message(STATUS "Attempting to locate Vulkan SDK using manual path......")
	set(VULKAN_SDK "F:/VulkanSDK")
	set(VULKAN_VERSION "1.2.182.0")
	set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")
	message(STATUS "Using manual specified path: ${VULKAN_PATH}")

	# Check if manual set path exists
	if(NOT EXISTS "${VULKAN_PATH}")
		message("Error: Unable to locate this Vulkan SDK path VULKAN_PATH: ${VULKAN_PATH}, please specify correct path.
		For more information on correct installation process, please refer to subsection 'Getting started with Lunar-G SDK'
		and 'Setting up first project with CMake' in Chapter 3, 'Shaking hands with the device' in this book 'Learning Vulkan', ISBN - 9781786469809.")
	   return()
	endif()
endif()

# Initializing project
project(IncenerateRenderEngine)

set(VULKAN_LIB_LIST "vulkan-1")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# Include Vulkan header files from Vulkan SDK
	include_directories(AFTER ${VULKAN_PATH}/Include)

	# Link directory for vulkan-1
	link_directories(${VULKAN_PATH}/Bin;${VULKAN_PATH}/Lib;)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

file(GLOB_RECURSE CPP_FILES_CORE ${CMAKE_CURRENT_SOURCE_DIR}/source/core/*.cpp)
file(GLOB_RECURSE CPP_FILES_UTIL ${CMAKE_CURRENT_SOURCE_DIR}/source/util/*.cpp)
file(GLOB_RECURSE CPP_FILES_FS ${CMAKE_CURRENT_SOURCE_DIR}/source/filesystem/*.cpp)
file(GLOB_RECURSE CPP_FILES_SERIALIZE ${CMAKE_CURRENT_SOURCE_DIR}/source/serializer/*.cpp)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h)
file(GLOB_RECURSE CPP_FILES_IMGUI ${CMAKE_CURRENT_SOURCE_DIR}/source/external/imgui/*.cpp)

set(CPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CPP_FILES_CORE}
	${CPP_FILES_UTIL}
	${CPP_FILES_FS}
	${CPP_FILES_SERIALIZE}
	${CPP_FILES_IMGUI}
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/external/glfw)
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/external/glfw/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/external/imgui)
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/external/imgui)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/external/glm)
set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/external/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/external/ktx)
set(KTX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/external/ktx/include)

# set(ENABLE_GLSLANG_WEBMIN OFF)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/external/glslang)
# set(GLSLANG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/external/glslang)
find_package(glslang REQUIRED)

add_executable(${PROJECT_NAME} ${CPP_FILES} ${HPP_FILES})
# Link the debug and release libraries to the project
target_link_libraries( ${PROJECT_NAME} PUBLIC 
	glfw
	${VULKAN_LIB_LIST} 
	imgui
	libktx
	glslang
	SPIRV
)

target_include_directories( ${PROJECT_NAME} PRIVATE 
	${GLM_INCLUDE_DIR} 
	${GLFW_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/source/ 
	${CMAKE_CURRENT_SOURCE_DIR}/source/core
	${IMGUI_INCLUDE_DIR}
	${GLSLANG_INCLUDE_DIR}
	${SPIRV_INCLUDE_DIR}
)

target_precompile_headers( ${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common.h)

# Compile shaders
set(SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders)
file(GLOB_RECURSE VERT_SHADER_SOURCES ${SHADER_PATH}/*.vert)
file(GLOB_RECURSE FRAG_SHADER_SOURCES ${SHADER_PATH}/*.frag)

foreach(cur_path IN LISTS VERT_SHADER_SOURCES)
	get_filename_component(SHADER_FILE_NAME ${cur_path} NAME)
	string(REPLACE ".vert" "" OUTPUT_DIRECTORY_NAME ${SHADER_FILE_NAME})
	get_filename_component(SHADER_FILE_PATH ${cur_path} DIRECTORY)
	set(OUTPUT_DIRECTORY_PATH "${SHADER_FILE_PATH}/${OUTPUT_DIRECTORY_NAME}")
	if(NOT EXISTS ${OUTPUT_DIRECTORY_PATH})
	file(MAKE_DIRECTORY ${OUTPUT_DIRECTORY_PATH})
	endif()
	set(OUTPUT_DIRECTORY_PATH ${OUTPUT_DIRECTORY_PATH}/vert.spv)
	add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND glslangValidator.exe -V ${cur_path} -o ${OUTPUT_DIRECTORY_PATH})
	message(STATUS "executing glslangValidator.exe -V ${cur_path} -o ${OUTPUT_DIRECTORY_PATH}")
endforeach()

foreach(cur_path IN LISTS FRAG_SHADER_SOURCES)
	get_filename_component(SHADER_FILE_NAME ${cur_path} NAME)
	string(REPLACE ".frag" "" OUTPUT_DIRECTORY_NAME ${SHADER_FILE_NAME})
	get_filename_component(SHADER_FILE_PATH ${cur_path} DIRECTORY)
	set(OUTPUT_DIRECTORY_PATH "${SHADER_FILE_PATH}/${OUTPUT_DIRECTORY_NAME}")
	if(NOT EXISTS ${OUTPUT_DIRECTORY_PATH})
	file(MAKE_DIRECTORY ${OUTPUT_DIRECTORY_PATH})
	endif()
	set(OUTPUT_DIRECTORY_PATH ${OUTPUT_DIRECTORY_PATH}/frag.spv)
	add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND glslangValidator.exe -V ${cur_path} -o ${OUTPUT_DIRECTORY_PATH})
	message(STATUS "executing glslangValidator.exe -V ${cur_path} -o ${OUTPUT_DIRECTORY_PATH}")
endforeach()


# Define project properties
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Define C++ version to be used for building the project
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# Define C version to be used for building the project
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED ON)