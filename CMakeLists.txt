cmake_minimum_required(VERSION 3.5.0)
project(Quixotism VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(./quixotism_engine/src/util ./quixotism_engine/src/win32 ./quixotism_engine/src/core ./external/GLEW/include ./external/GLM)

add_compile_definitions(GLEW_STATIC)
message("COMPILE DEFINITION ADDED: GLEW_STATIC ----> Build needs to link GLEW as static library!")

add_compile_definitions(BUILD_WIN32)
message("COMPILE DEFINITION ADDED: WIN32 ----> Build will be built as Win32 app")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
message("---------- DEBUG BUILD TARGET -----------")
add_compile_definitions(BUILD_INTERNAL=1)
message("COMPILE DEFINITION ADDED: BUILD_INTERNAL=1 ----> Build will be build in debug mode")
add_compile_options(-W4  -WX)
message("COMPILE OPTION ADDED: -W4 -WX ----> Build will use warning level 4 and treat warnings as errors")
add_compile_options(-Od)
message("COMPILE OPTION ADDED: -Od ----> Build will be built with no optimizations")
endif()

add_compile_options(-Oi)
message("COMPILE OPTION ADDED: -Oi ----> Build will be built with intrinsic support")

add_compile_options(-MTd)
message("COMPILE OPTION ADDED: -MTd")

add_compile_options(-wd4201 -wd4100)
message("COMPILE OPTION ADDED: -wd4201 -wd4100 ----> Those warnings will be disabled")

file(GLOB WIN32_SOURCES "quixotism_engine/src/win32/*.cpp")
file(GLOB CORE_SOURCES "quixotism_engine/src/core/*.cpp")

add_library(QuixotismEngine ${CORE_SOURCES} ${MICROGL_SOURCES})


find_library(GLEW_LIB glew32s HINTS external/GLEW/lib)
find_package(OpenGL REQUIRED)

TARGET_LINK_LIBRARIES(QuixotismEngine ${GLEW_LIB} opengl32)

add_executable(win32_QuixotismEngine WIN32 ${WIN32_SOURCES})
target_link_libraries(win32_QuixotismEngine QuixotismEngine user32.lib gdi32.lib winmm.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)