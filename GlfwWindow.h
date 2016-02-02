#pragma once

#include <stdexcept>
#include <string>

#include <GLFW/glfw3.h>

namespace RunFragment {

class GlfwWindow {
	friend class GlfwContext;
public:
	GlfwWindow(std::string title, int width, int height) {
		window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		if(!window) {
			throw std::runtime_error {"glfwCreateWindow has returned nullptr"};
		}
	}

	~GlfwWindow() {
		if(!window) {
			glfwDestroyWindow(window);
		}
	}

	GlfwWindow(GlfwWindow&& that)
		: window {that.window} {
		that.window = nullptr;
	}

	GlfwWindow& operator =(GlfwWindow&& rhs) {
		this->window = rhs.window;
		rhs.window = nullptr;

		return *this;
	}

	void makeCurrent() {
		if(window) {
			glfwMakeContextCurrent(window);
		}
		else {
			throw std::runtime_error {"GlfwWindow::makeCurrent() called from moved object"};
		}
	}

	void swapBuffers() {
		if(window) {
			glfwSwapBuffers(window);
		}
		else {
			throw std::runtime_error {"GlfwWindow::swapBuffers() called from moved object"};
		}
	}

	bool isEnd() {
		if(window) {
			return glfwWindowShouldClose(window);
		}
		else {
			throw std::runtime_error {"GlfwWindow::isEnd() called from moved object"};
		}
	}

private:
	GLFWwindow* window = nullptr;
};

}
