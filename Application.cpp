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
	
	image = std::unique_ptr<Renderer> {new Renderer {config, Renderer::Target::Image, window}};
	for(std::size_t i = 0; i < bufs.size(); i++) {
		auto& buf = bufs[i];
		if(config.channels[i]) {
			buf = std::unique_ptr<Renderer> {new Renderer {config, static_cast<Renderer::Target>(i), window}};
		}
	}
}

void Application::run() {
	FileWatcher fileWatcher;
	
	auto watchIfDefined = [&fileWatcher] (const boost::optional<std::string>& channel, Renderer* renderer) {
		if(renderer) {
			fileWatcher.add(*channel, [renderer] {
				renderer->reloadFile();
			});
		}
	};
	
	fileWatcher.add(config.file, [this] {
		image->reloadFile();
	});
	for(std::size_t i = 0; i < bufs.size(); i++) {
		auto& buf = bufs[i];
		const auto& file = config.channels[i];
		
		if(buf) {
			assert(file);
			fileWatcher.add(*file, [&buf] {
				buf->reloadFile();
			});
		}
	}
	
	auto fileWatcherThread = fileWatcher.spawn();
	fileWatcherThread.detach();
	
	image->start();
	for(auto& buf : bufs) {
		if(buf) {
			buf->start();
		}
	}
	
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		image->render();
		for(auto& buf : bufs) {
			if(buf) {
				buf->render();
			}
		}
		
		glfwSwapBuffers(window);
	}
	
}

void Application::onGlfwError(int error, const char* description) {
	std::cerr << "GLFW error: " << description << std::endl;
}

void Application::onWindowResize(GLFWwindow*, int width, int height) {
	glViewport(0, 0, width, height);
	Renderer::onWindowResize(width, height);
}

}
