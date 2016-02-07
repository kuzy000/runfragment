#include <stdexcept>
#include <fstream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "GlfwContext.h"
#include "GlfwWindow.h"
#include "Configuration.h"


const std::string shaderToy = 
R"raw(
time = 1

iResolution = iResolution
iGlobalTime = iGlobalTime
iGlobalDelta = iGlobalDelta
iGlobalFrame = iGlobalFrame
iChannelTime = iChannelTime
iMouse = iMouse
iDate = iDate
iSampleRate = iSampleRate
iChannelResolution = iChannelResolution
iChanneli = iChanneli
)raw";

const std::string glslSandbox =  R"raw(
time = 1

iResolution = resolution
iGlobalTime = time
iGlobalDelta = iGlobalDelta
iGlobalFrame = iGlobalFrame
iChannelTime = iChannelTime
iMouse = mouse
iDate = iDate
iSampleRate = iSampleRate
iChannelResolution = iChannelResolution
iChanneli = iChanneli
)raw";


namespace po = boost::program_options;

int main(int argc, char* argv[]) {
	po::options_description gen;
	gen.add_options()
		("file",     po::value<std::string>(), "Input file")
		("config,c", po::value<std::string>(), "Config file")
		("time,t",   po::value<float>(),       "Set value that multiplies the time")
		("help,h",                             "Display help message")
		("format,f", po::value<std::string>(), "Format of uniforms:\n    g : GLSLSandbox\n    s : ShaderToy (default)");

	po::options_description uni;
	uni.add_options()
		("iResolution",        po::value<std::string>(), "Set name of iResolution uniform")
		("iGlobalTime",        po::value<std::string>(), "Set name of iGlobalTime uniform")
		("iGlobalDelta",       po::value<std::string>(), "Set name of iGlobalDelta uniform")
		("iGlobalFrame",       po::value<std::string>(), "Set name of iGlobalFrame uniform")
		("iChannelTime",       po::value<std::string>(), "Set name of iChannelTime uniform")
		("iMouse",             po::value<std::string>(), "Set name of iMouse uniform")
		("iDate",              po::value<std::string>(), "Set name of iDate uniform")
		("iSampleRate",        po::value<std::string>(), "Set name of iSampleRate uniform")
		("iChannelResolution", po::value<std::string>(), "Set name of iChannelResolution uniform")
		("iChanneli",          po::value<std::string>(), "Set name of iChanneli uniform");

	po::options_description desc;
	desc.add(gen).add(uni);

	po::positional_options_description pos;
	pos.add("file", 1);

	po::variables_map vm;
	auto parser = po::command_line_parser(argc, argv).options(desc).positional(pos);
	try {
		po::store(parser.run(), vm);
	}
	catch(std::exception& e) {
		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;
	}
	
	if(vm.count("help")) {
		std::cout << "Usage: run_fragment [options] file" << std::endl;
		std::cout << desc << std::endl;

		return EXIT_SUCCESS;
	}
	
	if(vm.count("config")) {
		std::ifstream iniFile {vm["config"].as<std::string>()};
		
		try {
			po::store(po::parse_config_file(iniFile, desc, true), vm);  
		}
		catch(std::exception& e) {
			std::cerr << e.what() << std::endl;
	
			return EXIT_FAILURE;
		}
	}
	
	if(vm.count("format")) {
		auto arg = vm["format"].as<std::string>();
		if(arg == "s") {
			std::istringstream ss {shaderToy};
			po::store(po::parse_config_file(ss, desc, true), vm);
		}
		else if(arg == "g") {
			std::istringstream ss {glslSandbox};
			po::store(po::parse_config_file(ss, desc, true), vm);
		}
		else {
			std::cerr << "invalid argument of '--format'" << std::endl;

			return EXIT_FAILURE;
		}
	}
	else {
		std::istringstream ss {shaderToy};
		po::store(po::parse_config_file(ss, desc, true), vm);
	}
	
	RunFragment::Configuration config;
	
	config.file = vm.count("file") 
	            ? boost::optional<std::string> {vm["file"].as<std::string>()} 
	            : boost::none;
	config.time = vm["time"].as<float>();

	config.iResolution = vm["iResolution"].as<std::string>();
	config.iGlobalTime = vm["iGlobalTime"].as<std::string>();
	config.iGlobalDelta = vm["iGlobalDelta"].as<std::string>();
	config.iGlobalFrame = vm["iGlobalFrame"].as<std::string>();
	config.iChannelTime = vm["iChannelTime"].as<std::string>();
	config.iMouse = vm["iMouse"].as<std::string>();
	config.iDate = vm["iDate"].as<std::string>();
	config.iSampleRate = vm["iSampleRate"].as<std::string>();
	config.iChannelResolution = vm["iChannelResolution"].as<std::string>();
	config.iChanneli = vm["iChanneli"].as<std::string>();

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
