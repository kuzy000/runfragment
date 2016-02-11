#include "Application.h"

#include "FileWatchListenerLambda.h"

#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <ctime>

#include <efsw/efsw.hpp>

namespace RunFragment {

Application::Application(const Configuration& config)
	: config (config) {
	
	glfwSetErrorCallback(&Application::onGlfwError);
	
	glfwInit();
	if(!glfwInit()) {
		throw std::runtime_error {"Failed to initialize GLFW"};
	}
	glfwInitalized = true;
	
	window = glfwCreateWindow(640, 480, "Run fragment", nullptr, nullptr);
	if(!window) {
		glfwTerminate();
		throw std::runtime_error {"Failed to create GLFWwindow"};
	}
	
	glfwMakeContextCurrent(window);
	
	glewExperimental = true;
	GLenum status = glewInit();
	if(status != GLEW_OK) {
		glfwDestroyWindow(window);
		glfwTerminate();
		
		std::stringstream ss;
		ss << "Failed to initialize GLEW" << std::endl;
		ss << "GLEW error: " << glewGetErrorString(status) << std::endl;
		throw std::runtime_error {ss.str()};
	}
	
	glfwSwapInterval(1);
	
	glfwSetWindowSizeCallback(window, &Application::onWindowResize);
}

Application::~Application() {
	if(window) {
		glfwDestroyWindow(window);
	}
	if(glfwInitalized) {
		glfwTerminate();
	}
}

void Application::run() {
	std::string dir = std::regex_replace(config.file, std::regex {"/[^/]*$"}, "");
	std::string file = std::regex_replace(config.file, std::regex {"^.*/"}, "");
	
	efsw::FileWatcher fileWatcher;
	FileWatchListenerLambda listener {
		[this, &file] (efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename) {
			if(filename == file && action == efsw::Actions::Add) {
				this->reloadFile();
				return;
			}
		}
	};
	
	auto watchId = fileWatcher.addWatch(dir, &listener, false);
	fileWatcher.watch();
	
	startTime = std::chrono::high_resolution_clock::now();
	
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		render();
		
		glfwSwapBuffers(window);
	}
	
	fileWatcher.removeWatch(watchId);
}

void Application::onGlfwError(int error, const char* description) {
	std::cerr << "GLFW error: " << description << std::endl;
}

void Application::onWindowResize(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height); // FIXME
}

}
