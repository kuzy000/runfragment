#include "Application.h"

#include "FileWatcher.h"

#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <ctime>
#include <thread>

#include <boost/optional.hpp>

namespace RunFragment {

Application::Application(const Configuration& config)
	: config (config) {
	
	glfwSetErrorCallback(&Application::onGlfwError);
	
	if(!glfwInit()) {
		throw std::runtime_error {"Failed to initialize GLFW"};
	}
	onDestruction.push([this] {
		glfwTerminate();
	});
	
	window = glfwCreateWindow(640, 480, "Run fragment", nullptr, nullptr);
	if(!window) {
		throw std::runtime_error {"Failed to create GLFWwindow"};
	}
	onDestruction.push([this] {
		glfwDestroyWindow(window);
	});
	
	glfwMakeContextCurrent(window);
	
	glewExperimental = true;
	GLenum status = glewInit();
	if(status != GLEW_OK) {
		std::stringstream ss;
		ss << "Failed to initialize GLEW" << std::endl;
		ss << "GLEW error: " << glewGetErrorString(status) << std::endl;
		throw std::runtime_error {ss.str()};
	}
	
	glfwSwapInterval(1);
	
	glfwSetWindowSizeCallback(window, &Application::onWindowResize);
	
	main = std::unique_ptr<Renderer> {new Renderer {config, Renderer::Target::Main, window}};
}

void Application::run() {
	FileWatcher fileWatcher;
	
	auto watchIfDefined = [&fileWatcher] (const boost::optional<std::string>& channel, Renderer* renderer) {
		if(channel) {
			fileWatcher.add(*channel, [renderer] {
				renderer->reloadFile();
			});
		}
	};
	watchIfDefined(config.file, main.get());
	watchIfDefined(config.channel0, channel0.get());
	watchIfDefined(config.channel1, channel1.get());
	watchIfDefined(config.channel2, channel2.get());
	watchIfDefined(config.channel3, channel3.get());
	
	auto fileWatcherThread = fileWatcher.spawn();
	fileWatcherThread.detach();
	
	auto runIfDefined = [] (Renderer* renderer) {
		if(renderer) {
			renderer->run();
		}
	};
	runIfDefined(main.get());
	runIfDefined(channel0.get());
	runIfDefined(channel1.get());
	runIfDefined(channel2.get());
	runIfDefined(channel3.get());
	
	auto renderIfDefined = [] (Renderer* renderer) {
		if(renderer) {
			renderer->render();
		}
	};
	
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		renderIfDefined(main.get());
		renderIfDefined(channel0.get());
		renderIfDefined(channel1.get());
		renderIfDefined(channel2.get());
		renderIfDefined(channel3.get());
		
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
