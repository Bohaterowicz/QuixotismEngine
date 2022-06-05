
file(GLOB MAIN_SOURCES "./src/*.cpp")

add_library(QuixImg ${MAIN_SOURCES})

target_include_directories(QuixImg PRIVATE ./src ../common)