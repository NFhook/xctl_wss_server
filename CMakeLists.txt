cmake_minimum_required(VERSION 3.10)
project(XctlWssServer)


set(CMAKE_C_STANDARD 11)


include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

# 查找所需库
find_package(PkgConfig REQUIRED)
pkg_check_modules(LWS REQUIRED libwebsockets)
pkg_check_modules(YYJSON REQUIRED yyjson)

# 添加目标
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接库
target_include_directories(${PROJECT_NAME} PUBLIC ${LWS_INCLUDE_DIRS} ${YYJSON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LWS_LIBRARIES} ${YYJSON_LIBRARIES})
target_compile_options(${PROJECT_NAME} PUBLIC ${LWS_CFLAGS_OTHER} ${YYJSON_CFLAGS_OTHER})
