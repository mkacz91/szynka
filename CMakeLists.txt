cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Szynka)

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)

set(SUBS_DIR "${CMAKE_SOURCE_DIR}/subs")

include_directories(
  "${SUBS_DIR}/include"
  ${GLFW3_INCLUDE_DIR}
)

file(GLOB SZYNKA_SOURCES src/*.cpp src/*.h)
add_executable(szynka ${SZYNKA_SOURCES})
target_link_libraries(szynka
  glfw3
  ${OPENGL_gl_LIBRARY}
)

if (WIN32)
  get_property(GLFW3_DLL TARGET glfw3 PROPERTY IMPORTED_LOCATION)
  add_custom_command(TARGET szynka POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different "${GLFW3_DLL}" "$<TARGET_FILE_DIR:szynka>")
endif (WIN32)
