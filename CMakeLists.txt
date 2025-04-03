cmake_minimum_required(VERSION 3.15)
project(Fastdeploy-UIE-Windows
    VERSION 1.0.0
    DESCRIPTION "UIE Inference Demo"
    LANGUAGES CXX
)

#------------------------------ 编译器配置 ------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "_d")  # Debug版本添加后缀

#------------------------------ 第三方依赖配置 ------------------------------
# 设置 FastDeploy SDK 路径（推荐使用项目相对路径）
set(DFASTDEPLOY_INSTALL_DIR "${CMAKE_SOURCE_DIR}/build/fastdeploy-win-x64-1.0.7")

# 架构检测（自动处理 x86/x64）
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
    message(STATUS "Building for 64-bit architecture")
else()
    set(ARCH_DIR "x86")
    message(STATUS "Building for 32-bit architecture")
endif()

#------------------------------ 可执行文件配置 ------------------------------
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/uie/uie_predictor.cpp
)

#------------------------------ 包含目录配置 ------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/uie  # 优先本地头文件
    ${DFASTDEPLOY_INSTALL_DIR}/include
)

#------------------------------ 链接配置 ------------------------------
target_link_directories(${PROJECT_NAME} PRIVATE
    ${DFASTDEPLOY_INSTALL_DIR}/lib
    ${DFASTDEPLOY_INSTALL_DIR}/lib/${ARCH_DIR}
)

# 假设需要链接的库文件列表
target_link_libraries(${PROJECT_NAME} PRIVATE
    fastdeploy
    # 其他依赖库...
)

#------------------------------ 运行时依赖处理 ------------------------------
# 自动复制 DLL 到输出目录（支持 Debug/Release 配置）
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${DFASTDEPLOY_INSTALL_DIR}/lib/${ARCH_DIR}/"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copying runtime dependencies..."
)

#------------------------------ 构建类型配置 ------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

#------------------------------ 安装配置（可选） ------------------------------
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)
