# 设置库名称
project(Debugger)

# 包含头文件目录
include_directories(${PROJECT_SOURCE_DIR}/header)

# 添加源文件
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# 添加库
add_library(debugger_lib ${SOURCES})

# 查找测试源文件
file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")

# 为每个测试源文件添加可执行文件
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} debugger_lib)
endforeach()