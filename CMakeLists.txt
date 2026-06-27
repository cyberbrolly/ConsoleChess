cmake_minimum_required(VERSION 3.15)
project(ConsoleChess CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

include_directories(include)

set(SOURCES
    main.cpp
    src/Piece.cpp
    src/Board.cpp
    src/Game.cpp
    src/AI.cpp
    src/Renderer.cpp
)

add_executable(ConsoleChess ${SOURCES})

# Link SDL2 and SDL2_ttf directly
target_link_libraries(ConsoleChess
    SDL2
    SDL2_ttf
)

# Include SDL2 headers
execute_process(COMMAND sdl2-config --cflags OUTPUT_VARIABLE SDL2_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDL2_CFLAGS}")