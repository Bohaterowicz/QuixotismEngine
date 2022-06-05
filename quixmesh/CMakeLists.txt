
file(GLOB MAIN_SOURCES "./src/*.cpp")

add_library(QuixMesh ${MAIN_SOURCES})

target_include_directories(QuixMesh PRIVATE ./src ../common)