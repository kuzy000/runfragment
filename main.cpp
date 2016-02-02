#include <stdexcept>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "GlfwContext.h"
#include "GlfwWindow.h"
#include "Configuration.h"

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
	po::options_description gen {"General options"};
	gen.add_options()
		("file",   po::value<std::string>(), "Input file")
		("time,t", po::value<float>(),       "Set value that multiplies the time")
		("help,h",                           "Display help message");

	po::options_description uni {"Uniform options"};
	uni.add_options()
		("format,f", po::value<std::string>(), "Format of uniforms:\n    g : GLSLSandbox\n    s : ShaderToy (default)")
		("utime",    po::value<std::string>(), "Set name of time uniform");

	po::options_description desc;
	desc.add(gen).add(uni);

	po::positional_options_description pos;
	pos.add("file", 1);

	po::variables_map vm;
	auto parser = po::command_line_parser(argc, argv).options(desc).positional(pos);
	try {
		po::store(parser.run(), vm);
		po::notify(vm);
	}
	catch(std::exception& e) {
		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;
	}

	if(vm.count("help")) {
		std::cout << "Usage: run_fragment [options] file" << std::endl << std::endl;
		std::cout << desc << std::endl;

		return EXIT_SUCCESS;
	}


	return 0;

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

	return EXIT_SUCCESS;
}
