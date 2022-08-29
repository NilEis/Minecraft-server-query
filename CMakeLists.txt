cmake_minimum_required(VERSION 3.0.0)
project(Minecraft-server-query VERSION 0.1.0  LANGUAGES C)

add_subdirectory(extern/plibsys)

include_directories(include)

add_library(Minecraft-server-query src/main.c src/error.c)

target_link_libraries(${PROJECT_NAME} plibsys)