cmake_minimum_required(VERSION 3.19.1)
project(libktx)

set(KTX_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/texture.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hashlist.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/checkheader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/swap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/memstream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/filestream.c
)

add_library(${PROJECT_NAME} STATIC ${KTX_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/other_include
)