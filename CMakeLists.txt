cmake_minimum_required(VERSION 3.28.0)
project(remake_cavestory VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/head)
file(GLOB SOURCES "./src/*.cpp")

add_executable(remake_cavestory ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    tinyxml2::tinyxml2
)