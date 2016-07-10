project(runfragment)
cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED COMPONENTS program_options filesystem)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

option(CPP-NETLIB_BUILD_TESTS "Build the cpp-netlib project tests." OFF)
option(CPP-NETLIB_BUILD_EXPERIMENTS "Build the cpp-netlib project experiments." OFF)
option(CPP-NETLIB_BUILD_EXAMPLES "Build the cpp-netlib project examples." OFF)
add_subdirectory(cpp-netlib)
find_package(cppnetlib REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

include_directories(${Boost_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${CPPNETLIB_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})

add_definitions(-std=c++11)

file(GLOB SOURCES "*.cpp")
# for QtCreator project tree
file(GLOB HEADERS "*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME}
	${GLFW_LIBRARIES}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${OPENGL_LIBRARY}
	${GLEW_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${OPENSSL_LIBRARIES}
	${CPPNETLIB_LIBRARIES}
)
