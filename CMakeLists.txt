cmake_minimum_required(VERSION 3.25)
project(stock_minimalism C)

set(CMAKE_C_STANDARD 99)

# The raylib submodule. BUILD_EXAMPLES defaults to PROJECT_IS_TOP_LEVEL, so
# raylib's own examples stay off automatically when it is a subdirectory.
add_subdirectory(raylib)

file(GLOB GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_executable(stock-minimalism ${GAME_SOURCES})
target_link_libraries(stock-minimalism PRIVATE raylib)

# Put assets/ next to the .exe so LoadTexture("assets/...") resolves the same
# way it does in the web build's --preload-file layout.
add_custom_command(TARGET stock-minimalism POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:stock-minimalism>/assets)
