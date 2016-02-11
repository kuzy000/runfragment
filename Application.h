#pragma once

#include "Configuration.h"
#include "Renderer.h"

#include <mutex>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace RunFragment {

class Application {
public:
	Application(const Configuration& config);
	~Application();
	
	void run();
private:
	static void onGlfwError(int error, const char* description);
	static void onWindowResize(GLFWwindow*, int width, int height);
	const Configuration config;
	
	GLFWwindow* window = nullptr;
	bool glfwInitalized = false;
};

}
