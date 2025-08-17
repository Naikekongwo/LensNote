cmake_minimum_required(VERSION 3.10)

project(LensNote CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_EXTENSIONS OFF)

# 所要编译的源文件

file(GLOB_RECURSE SOURCE_FILES 
    "${CMAKE_SOURCE_DIR}/project/*.cpp" 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# 链接外部库
set(SDL2_LIBRARIES mingw32 SDL2main SDL2 SDL2_ttf)
target_link_libraries(LensNote PRIVATE ${SDL2_LIBRARIES})

# 加上图标（即resources连接）
# set(RESOURCE_OBJECT "${CMAKE_SOURCE_DIR}/resources/resource.o")
# target_link_libraries(${PROJECT_NAME} PRIVATE ${RESOURCE_OBJECT})


# 拷贝 assets 文件夹
set(ENVIRONMENT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/environment")
set(ENVIRONMENT_DEST_DIR "${CMAKE_SOURCE_DIR}/build")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying environments to target folder..."
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ENVIRONMENT_DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENVIRONMENT_SOURCE_DIR}" "${ENVIRONMENT_DEST_DIR}"
)