cmake_minimum_required(VERSION 3.10)
project(RPG_Project)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES 
    "main.cpp"
    "character/*.cpp"
    "player/*.cpp"
    "enemy/*.cpp"
    "item/*.cpp"
    "skills/*.cpp"
    "save.cpp"
)

include_directories(character player enemy item skills save)

add_executable(RPG_Game ${SOURCES})