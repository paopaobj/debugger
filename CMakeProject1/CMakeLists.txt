cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(CMakeProject1)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含子目录
add_subdirectory(Debugger)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/Debugger/header)

# 查找源文件
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/Debugger/src/*.cpp)

# 添加可执行文件
add_executable(CMakeProject1 ${SOURCES})

# 链接库
target_link_libraries(CMakeProject1 debugger_lib)