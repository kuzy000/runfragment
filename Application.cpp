#include "Application.h"

#include "FileWatchListenerLambda.h"

#include <sstream>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>

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
	
	const GLfloat vertices[] = {
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f,  1.0f,
	};
	const GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
	
	const std::string vertexSource = R"glsl(
		#version 330 core
		layout(location = 0) in vec2 position;
		varying vec2 )glsl" + config.iSurfacePosition + R"glsl(;
		void main() {
			)glsl" + config.iSurfacePosition + R"glsl( = position;
			gl_Position = vec4(position, 0, 1);
		}
	)glsl";
	
	vertex = compileShader(GL_VERTEX_SHADER, vertexSource);
	
	reloadFile();
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
	glViewport(0, 0, width, height);
}

void Application::render() {
	reloadShader();
	
	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);
	glUniform2f(iResolution, static_cast<GLfloat>(width), static_cast<GLfloat>(height));
	
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	glUniform2f(iMouse, static_cast<GLfloat>(1. - x / width), static_cast<GLfloat>(y / height));
	
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program);
	glBindVertexArray(vao);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Application::reloadFile() {
	std::lock_guard<std::mutex> guard {sourceChanging};
	
	std::ifstream file {config.file};
	
	if(file.is_open()) {
		std::stringstream ss;
		ss << file.rdbuf();
		
		fragmentSource = ss.str();
		changed = true;
	}
	else {
		std::cerr << "Error: can't open file '" << config.file << "'" << std::endl;
	}
}

void Application::reloadShader() {
	std::lock_guard<std::mutex> guard {sourceChanging};

	if(changed) {
		GLuint newFragment = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
		if(newFragment != 0) {
			std::cout << "OK" << std::endl;
			if(fragment != 0) {
				glDeleteShader(fragment);
			}
			fragment = newFragment;

			if(program != 0) {
				glDeleteProgram(program);
			}
			program = glCreateProgram();
			glAttachShader(program, vertex);
			glAttachShader(program, fragment);
			glLinkProgram(program);

			iGlobalTime = glGetUniformLocation(program, config.iGlobalTime.c_str());
			iResolution = glGetUniformLocation(program, config.iResolution.c_str());
			iMouse      = glGetUniformLocation(program, config.iMouse.c_str());
		}
	}

	changed = false;
}

GLuint Application::compileShader(GLenum type, const std::string& source) {
	GLuint id = glCreateShader(type);

	const char* cStr = source.c_str();
	glShaderSource(id, 1, &cStr, nullptr);
	glCompileShader(id);

	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if(success == GL_FALSE) {
		GLint length = 1024;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		if(length == 0) { 
			length = 1024;
		}

		std::unique_ptr<GLchar[]> errorLog = std::unique_ptr<GLchar[]>(new GLchar[length]);
		glGetShaderInfoLog(id, sizeof(GLchar) * length, nullptr, errorLog.get());

		std::cerr << "Shader compilation failed:\n" << errorLog.get() << std::endl;

		id = 0;
	}

	return id;
}

}
