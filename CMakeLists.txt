cmake_minimum_required(VERSION 3.29)
project(Tetris)

include_directories(/opt/homebrew/include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64")

set(SFML_DIR "/opt/homebrew/Cellar/sfml@2/2.6.2/lib/cmake/SFML")
find_package(SFML 2 REQUIRED COMPONENTS graphics window system)
add_executable(Tetris src/main.cpp)
target_include_directories(Tetris PRIVATE /opt/homebrew/include)
target_link_libraries(Tetris sfml-graphics sfml-window sfml-system)
