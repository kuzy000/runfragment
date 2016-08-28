#pragma once

#include "AppConfig.h"
#include "OnScopeEnd.h"
#include "Renderer.h"

#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace RunFragment {

class Application {
public:
	Application(AppConfig config);
	
	void run();
private:
	static void onGlfwError(int error, const char* description);
	static void onWindowResize(GLFWwindow*, int width, int height);
	
	const AppConfig config;
	
	std::unique_ptr<Renderer> image;
	std::array<std::unique_ptr<Renderer>, 4> bufs {};
	
	GLFWwindow* window = nullptr;
	
	OnScopeEnd onDestruction;
};

}
