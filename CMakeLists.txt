
cmake_minimum_required(VERSION 3.22)

project(neo_char_convert)

if(CMAKE_CXX_COMPILER MATCHES "/em\\+\\+(-[a-zA-Z0-9.])?$")
  message(" * C++ compiler: Emscripten")
  set(CMAKE_CXX_COMPILER_ID "Emscripten")
else()
  message(" * C++ cimpiler: ${CMAKE_CXX_COMPILER_ID}")
endif()


add_executable(
    neo_char_convert
    src/main.cpp
    src/viewhtml.cpp
    )

target_compile_features(neo_char_convert PRIVATE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index.html")
    SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${CMAKE_SOURCE_DIR}/shell.html")
endif()

