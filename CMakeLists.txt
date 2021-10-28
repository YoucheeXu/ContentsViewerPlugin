# 最低要求的CMake版本
cmake_minimum_required(VERSION 3.0.0)

# 项目名称
project(ContentsViewerPlugin)

set(CMAKE_CXX_STANDARD 17)

ADD_DEFINITIONS(-DUNICODE)

#向工程添加多个特定的头文件搜索路径，路径之间用空格分隔
include_directories(C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0 third_party/Win32xx_860/include third_party/NPP third_party)

#添加子目录
# add_subdirectory(src)
# add_subdirectory(third_party)

# 建立变量SRC_FILE为目录下.c文件列表
# file(GLOB SRC_FILE *.c)

aux_source_directory(./src MAIN_SRCS)
aux_source_directory(./third_party THIRD_SRCS)

# 命令：add_library(libname [SHARED|STATIC|MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)
# libname:生成的库文件的名字
# ​[SHARED|STATIC|MODULE]：生成库文件的类型（动态库|静态库|模块）
# [EXCLUDE_FROM_ALL]：有这个参数表示该库不会被默认构建​
# source2 ... sourceN：生成库依赖的源文件，如果源文件比较多，可以使用​aux_sourcr_directory命令获取路径下所有源文件.
add_library(${PROJECT_NAME} SHARED ${MAIN_SRCS} ${THIRD_SRCS})

# 要求编译可执行文件
# add_executable (${PROJECT_NAME} ${SRC_FILE})

#指定生成可执行文件的目录   PROJECT_BINARY_DIR 运行cmake命令的目录,通常是${PROJECT_SOURCE_DIR}/build
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 添加链接库  可执行文件 ${PROJECT_NAME} 需要连接一个名为 Lib 的链接库
# target_link_libraries(${PROJECT_NAME} LIB_SRCS)