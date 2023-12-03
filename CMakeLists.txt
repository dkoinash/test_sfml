cmake_minimum_required(VERSION 3.7)

################################################################################
# vcpkg
################################################################################
include (${CMAKE_SOURCE_DIR}/cmake/vcpkg.cmake)

set(CMAKE_CXX_STANDARD 17)

project(OpenIt
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Test SFML"
)

################################################################################
# Libraries
################################################################################
################################################################################
# - SFML
################################################################################
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

################################################################################
# Sources
################################################################################
set(DIR_INCLUDE ${CMAKE_SOURCE_DIR}/include)
set(DIR_SRC ${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE include_h ${DIR_INCLUDE}/*.h)
file(GLOB_RECURSE src_h ${DIR_SRC}/*.h)
file(GLOB_RECURSE src_cpp ${DIR_SRC}/*.cpp)

set(soucres ${include_h} ${src_h} ${src_cpp})

################################################################################
# Executable
################################################################################
if (WIN32)
  add_executable(${PROJECT_NAME} WIN32 ${soucres})
else()
  add_executable(${PROJECT_NAME} ${soucres})
endif()

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${DIR_INCLUDE}
)

target_link_libraries(${PROJECT_NAME} 
  PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
)

# target_link_directories(${PROJECT_NAME}
#   PRIVATE
# 
# )

################################################################################
# Resources
################################################################################
set(RESOURCES_DIR resources)
file(GLOB files ${RESOURCES_DIR}/*.*)

foreach(item IN LISTS files)
  get_filename_component(dest_filename ${item} NAME)

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy ${item}
                             $<TARGET_FILE_DIR:${PROJECT_NAME}>/${dest_filename}
  )
endforeach()

################################################################################
# Compilation flags
################################################################################
include (${CMAKE_SOURCE_DIR}/cmake/compilation_flags.cmake)