project(run_fragment)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(glfw)
include_directories(glfw/include)

add_subdirectory(efsw)
include_directories(efsw/include)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_definitions(-std=c++11)

file(GLOB SOURCES "*.cpp")
# for QtCreator project tree
file(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
	glfw ${GLFW_LIBRARIES}
	efsw
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)
