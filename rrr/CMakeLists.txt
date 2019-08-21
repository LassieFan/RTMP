cmake_minimum_required (VERSION 2.8)
project(demo)
aux_source_directory(. DIR_SRCS)

add_definitions(-std=c++11)

# 添加子目录
#add_subdirectory(src)

# 指定生成目标
add_executable(demo ${DIR_SRCS})

target_link_libraries(demo pthread event)