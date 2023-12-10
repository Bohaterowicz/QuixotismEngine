cmake_minimum_required(VERSION 3.5.0)
project(Quixotism VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-MTd)
message("COMPILE OPTION ADDED: -MTd")
add_compile_definitions(GLEW_STATIC)
message("COMPILE DEFINITION ADDED: GLEW_STATIC ----> Build needs to link GLEW as static library!")

add_compile_definitions(BUILD_WIN32)
message("COMPILE DEFINITION ADDED: BUILD_WIN32 ----> Build will be built as Win32 app")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
message("---------- DEBUG BUILD TARGET -----------")
add_compile_definitions(_DEBUG=1)
message("COMPILE DEFINITION ADDED: BUILD_INTERNAL=1 ----> Build will be build in debug mode")
add_compile_options(-W4)
message("COMPILE OPTION ADDED: -W4 -WX ----> Build will use warning level 4 and treat warnings as errors")
add_compile_options(-Od)
message("COMPILE OPTION ADDED: -Od ----> Build will be built with no optimizations")
add_compile_options(/ZI)
message("COMPILE OPTION ADDED: /ZI")
endif()

add_compile_options(-Oi)
message("COMPILE OPTION ADDED: -Oi ----> Build will be built with intrinsic support")

add_compile_options(-wd4201 -wd4100)
message("COMPILE OPTION ADDED: -wd4201 -wd4100 ----> Those warnings will be disabled")

add_subdirectory(quixotism_engine)

include_directories(./quixotism_engine/src ./third_party ./third_party/GLEW/include ./quixotism_engine/src/common)

set(WIN32_SOURCES 
quixotism_engine/src/win32/win32_quixotism_engine.cpp 
quixotism_engine/src/win32/win32_quixotism_io.cpp  
quixotism_engine/src/win32/win32_quixotism_opengl.cpp 
quixotism_engine/src/win32/win32_quixotism_time.cpp 
quixotism_engine/src/win32/win32_quixotism_window.cpp 
)

add_executable(win32_QuixotismEngine WIN32 ${WIN32_SOURCES})
target_link_libraries(win32_QuixotismEngine QuixotismEngine user32.lib gdi32.lib winmm.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)