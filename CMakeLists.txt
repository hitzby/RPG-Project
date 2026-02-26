cmake_minimum_required(VERSION 3.10)

# 项目名称
project(RPG_Project)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 自动搜索所有子文件夹中的源文件 (.cpp)
# 这行代码会把 main.cpp 以及 character, enemy, player 文件夹下的所有 cpp 都找出来
file(GLOB_RECURSE SOURCES 
    "main.cpp"
    "character/*.cpp"
    "player/*.cpp"
    "enemy/*.cpp"
    jobs/*.cpp
)

# 告诉编译器头文件的搜索路径
# 这样你在写 #include 时，编译器能更容易找到它们
include_directories(character player enemy jobs)

# 生成可执行文件
add_executable(RPG_Game ${SOURCES})