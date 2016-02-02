project(run_fragment)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(glfw)
include_directories(glfw/include)

file(GLOB SOURCES "*.cpp")
# for QtCreator project tree
file(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HADERS})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES})
