cmake_minimum_required(VERSION 3.9.0)
project(Minecraft-server-query VERSION 0.1.0  LANGUAGES C)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC")
    add_compile_options(-ffunction-sections -fdata-sections)
endif()

add_subdirectory(extern/plibsys)

include_directories(include)

add_library(Minecraft-server-query src/main.c src/error.c)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_link_options(Minecraft-server-query PRIVATE -Wl,--gc-sections)
endif()

include(CheckIPOSupported)

check_ipo_supported(RESULT result OUTPUT output LANGUAGES C)
if(result)
message(STATUS "IPO is supported")
set_property(TARGET Minecraft-server-query PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
message(WARNING "IPO is not supported: ${output}")
endif()

target_link_libraries(Minecraft-server-query plibsys)


option(BUILD_EXAMPLE_IMPLEMENTATION "Build example" ON)
if (BUILD_EXAMPLE_IMPLEMENTATION)
    message("Build example")
    add_executable(msq_example example/main.c)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        target_link_options(msq_example PRIVATE -Wl,--gc-sections -s)
    endif()
    if(result)
        set_property(TARGET msq_example PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
    target_link_libraries(msq_example Minecraft-server-query)
endif()
