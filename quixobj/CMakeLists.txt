
file(GLOB MAIN_SOURCES "./src/*.cpp")

add_library(QuixOBJ ${MAIN_SOURCES})

target_include_directories(QuixOBJ PRIVATE ./src ../common)