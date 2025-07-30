cmake_minimum_required(VERSION 3.10)
project(ThsFactorSdkDemo)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/rapidjson)

# 设置可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# 查找 demo 源文件
set(DEMO_SRC demo/main.cpp)

# 添加可执行文件
add_executable(demo ${DEMO_SRC})

# 链接 Windows 下的动态库目录
if(WIN32)
    # 明确指定 LHSession.lib 的绝对路径，避免找不到库
    target_link_libraries(demo PRIVATE "${CMAKE_SOURCE_DIR}/lib/LHSession.lib")
endif()

# 拷贝 dll 到输出目录（仅 Windows）
if(WIN32)
    file(GLOB DLLS "${CMAKE_SOURCE_DIR}/dll/*.dll")
    add_custom_command(TARGET demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLLS}
        $<TARGET_FILE_DIR:demo>
    )
endif()