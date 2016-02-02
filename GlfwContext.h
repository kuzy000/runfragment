#pragma once

#include <stdexcept>

#include <GLFW/glfw3.h>

namespace RunFragment {

class GlfwContext {
public:
	GlfwContext() {
		status = glfwInit();
		if(status == GLFW_FALSE) {
			throw std::runtime_error {"glfwInit() has returned GLFW_FALSE"};
		}
	}

	~GlfwContext() {
		if(status == GLFW_TRUE) {
			glfwTerminate();
		}
	}

	GlfwContext(GlfwContext&& that)
		: status {that.status} {
		that.status = GLFW_FALSE;
	}

	GlfwContext& operator =(GlfwContext&& rhs) {
		this->status = rhs.status;
		rhs.status = GLFW_FALSE;

		return *this;
	}

private:
	int status = GLFW_FALSE;
};

}
