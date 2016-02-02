#include "GlfwContext.h"
#include "GlfwWindow.h"

int main() {
	RunFragment::GlfwContext context;
	RunFragment::GlfwWindow window {"Run fragment", 640, 480};

	window.makeCurrent();

	/* Loop until the user closes the window */
	while(!window.isEnd()) {
		/* Render here */

		/* Swap front and back buffers */
		window.swapBuffers();

		/* Poll for and process events */
		glfwPollEvents();
	}

	return 0;
}
