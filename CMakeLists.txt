cmake_minimum_required(VERSION 3.10)
project(ThsFactorSdkSync)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(include)

# 定义导出宏
add_definitions(-DCOSMOS_THSFACTOR_SDK_DLL_EXPORT)

# 库目录（平台相关）
if(WIN32)
    link_directories(lib)
elseif(UNIX)
    link_directories(so)
endif()

# 查找平台相关的动态库
if(WIN32)
    find_file(LHSESSION_DLL LHSession.dll PATHS dll NO_DEFAULT_PATH)
    if(NOT LHSESSION_DLL)
        message(WARNING "未找到LHSession.dll，请确保dll目录中存在该文件")
    endif()
elseif(UNIX)
    find_file(LHSESSION_SO libLHSession.so PATHS so NO_DEFAULT_PATH)
    if(NOT LHSESSION_SO)
        message(WARNING "未找到libLHSession.so，请确保so目录中存在该文件")
    endif()
endif()

# 编译同步接口示例程序
add_executable(demo_sync
    demo/main_sync.cpp
)

# 设置运行时库路径，确保能找到平台相关的动态库
if(WIN32)
    # Windows平台：链接 .lib 文件
    target_link_libraries(demo_sync PRIVATE "${CMAKE_SOURCE_DIR}/lib/LHSession.lib")
elseif(UNIX)
    # Linux平台：链接 .so 文件
    target_link_libraries(demo_sync PRIVATE "${CMAKE_SOURCE_DIR}/so/libLHSession.so")
    # 设置运行时库路径
    set_target_properties(demo_sync PROPERTIES
        BUILD_RPATH "${CMAKE_SOURCE_DIR}/so"
        INSTALL_RPATH "${CMAKE_SOURCE_DIR}/so"
    )
endif()