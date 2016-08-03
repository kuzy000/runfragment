project(runfragment)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Boost REQUIRED COMPONENTS program_options filesystem)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(FreeImagePlus REQUIRED)

set(CPP-NETLIB_BUILD_TESTS       OFF CACHE BOOL "")
set(CPP-NETLIB_BUILD_EXPERIMENTS OFF CACHE BOOL "")
set(CPP-NETLIB_BUILD_EXAMPLES    OFF CACHE BOOL "")
add_subdirectory(cpp-netlib)
find_package(cppnetlib REQUIRED)

set(RAPIDJSON_BUILD_DOC      OFF CACHE BOOL "")
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "")
set(RAPIDJSON_BUILD_TESTS    OFF CACHE BOOL "")
add_subdirectory(rapidjson)
find_package(RapidJSON REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

include_directories(
	${Boost_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}
	${CPPNETLIB_INCLUDE_DIRS}
	${OPENSSL_INCLUDE_DIR}
	${RAPIDJSON_INCLUDE_DIRS}
	${FREEIMAGEPLUS_INCLUDE_DIRS}
	rapidjson/include/ # FIXME
)

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
	${RAPIDJSON_LIBRARIES}
	${FREEIMAGEPLUS_LIBRARIES}
)
