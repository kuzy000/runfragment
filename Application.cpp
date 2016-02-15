#include "Application.h"

#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <ctime>

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
	
	
	main = std::unique_ptr<Renderer> {new Renderer {config, Renderer::Target::Main, window}};
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
	main->run();
	
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		main->render();
		
		glfwSwapBuffers(window);
	}
}

void Application::onGlfwError(int error, const char* description) {
	std::cerr << "GLFW error: " << description << std::endl;
}

void Application::onWindowResize(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height); // FIXME
}

}
