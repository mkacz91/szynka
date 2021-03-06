cmake_minimum_required(VERSION 3.4.1 FATAL_ERROR)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 11)

project(Szynka)

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(SUBS_DIR "${CMAKE_SOURCE_DIR}/subs")

set(ASSET_CONFIG "${CMAKE_BINARY_DIR}/szynka_assets.config")

set(GLAD_DIR "${CMAKE_BINARY_DIR}/glad")
set(GLAD_SOURCES "${GLAD_DIR}/glad.c" "${GLAD_DIR}/glad.h")
add_custom_command(
  OUTPUT ${GLAD_SOURCES}
  COMMAND python -m glad --api gl=4.0 --profile core --extensions "${SRC_DIR}/glext.txt" --generator c --no-loader --omit-khrplatform --local-files --out-path "${GLAD_DIR}"
)

add_library(glad ${GLAD_SOURCES})

include_directories(
  "${SUBS_DIR}/include"
  "${GLAD_DIR}"
  ${GLFW3_INCLUDE_DIR}
)

file(GLOB SZYNKA_SOURCES src/*.cpp src/*.h)
add_executable(szynka ${SZYNKA_SOURCES})
target_link_libraries(szynka
  glfw3
  glad
  ${OPENGL_gl_LIBRARY}
)

if (WIN32)
  get_property(GLFW3_DLL TARGET glfw3 PROPERTY IMPORTED_LOCATION)
  add_custom_command(TARGET szynka POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different "${GLFW3_DLL}" "$<TARGET_FILE_DIR:szynka>")
endif (WIN32)

file(WRITE "${ASSET_CONFIG}" "")
file(APPEND "${ASSET_CONFIG}" "${CMAKE_SOURCE_DIR}/assets : generic\n")
file(APPEND "${ASSET_CONFIG}" "${CMAKE_SOURCE_DIR}/src/shaders : shader glsl\n")
