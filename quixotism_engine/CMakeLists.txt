file(GLOB CORE_SOURCES "src/core/*.cpp" "src/core/components/*.cpp")
file(GLOB OPENGL_SOURCES "src/opengl/*.cpp")
file(GLOB RENDERER_SOURCES "src/renderer/*.cpp")
file(GLOB STB_IMAGE "src/util/stb_image.cpp")

add_library(QuixotismEngine ${CORE_SOURCES} ${MICROGL_SOURCES} ${OPENGL_SOURCES} ${RENDERER_SOURCES} ${STB_IMAGE})

target_include_directories(QuixotismEngine PUBLIC ./src/util ./src/renderer ./src/math ./src/win32 ./src/core ./src/opengl ../external/GLEW/include ../external/GLM ../common ../)

find_library(GLEW_LIB glew32s HINTS external/GLEW/lib)
find_package(OpenGL REQUIRED)

target_link_libraries(QuixotismEngine QuixOBJ ${GLEW_LIB} opengl32)