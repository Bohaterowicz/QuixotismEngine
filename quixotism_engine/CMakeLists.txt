file(GLOB CORE_SOURCES "src/core/*.cpp")
file(GLOB CONTAINER_SOURCES "src/containers/*.cpp")
file(GLOB FONT_SOURCES "src/fonts/*.cpp")
file(GLOB BITMAP_PROCESS_SOURCES "src/bitmap/*.cpp")
file(GLOB RENDERER_SOURCES "src/renderer/*.cpp")
file(GLOB OBJ_PARSER_SOURCES "src/file_processing/obj_parser/*.cpp")
file(GLOB PNG_PARSER_SOURCES "src/file_processing/png_parser/*.cpp")
file(GLOB MATH_SOURCES "src/math/*.cpp")

add_library(QuixotismEngine ${CORE_SOURCES} ${FONT_SOURCES} ${BITMAP_PROCESS_SOURCES} ${RENDERER_SOURCES} ${MATH_SOURCES} ${OBJ_PARSER_SOURCES} ${PNG_PARSER_SOURCES})

target_include_directories(QuixotismEngine PUBLIC ./src ./src/util ../third_party ../third_party/GLEW/include ../third_party/GLM)

find_library(GLEW_LIB glew32s HINTS ../third_party/GLEW/lib)
find_package(OpenGL REQUIRED)

target_link_libraries(QuixotismEngine ${GLEW_LIB} opengl32)