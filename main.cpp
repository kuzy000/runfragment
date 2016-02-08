#include <stdexcept>
#include <fstream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include "Application.h"
#include "Configuration.h"

const std::string shaderToy = 
R"raw(
time = 1
main = mainImage

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
iSurfacePosition = iSurfacePosition
)raw";

const std::string glslSandbox =  R"raw(
time = 1
main = none

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
iSurfacePosition = surfacePosition
)raw";


namespace po = boost::program_options;

po::variables_map parseArguments(int argc, char* argv[], po::options_description desc) {
	po::positional_options_description pos;
	pos.add("file", 1);

	po::variables_map vm;
	auto parser = po::command_line_parser(argc, argv).options(desc).positional(pos);
	po::store(parser.run(), vm);
	
	if(vm.count("config")) {
		std::ifstream iniFile {vm["config"].as<std::string>()};
		po::store(po::parse_config_file(iniFile, desc, true), vm);  
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
			throw std::runtime_error {"invalid argument of '--format'"};
		}
	}
	else {
		std::istringstream ss {shaderToy};
		po::store(po::parse_config_file(ss, desc, true), vm);
	}
	
	if(!vm.count("file")) {
		throw std::runtime_error {"file doesn't specified; try '--help'"};
	}
	
	return vm;
}

int main(int argc, char* argv[]) {
	po::options_description gen;
	gen.add_options()
		("file",     po::value<std::string>(), "Input file")
		("config,c", po::value<std::string>(), "Config file")
		("time,t",   po::value<float>(),       "Set value that multiplies the time")
		("main",     po::value<std::string>(), "Set name of mainImage(out vec4 fragColor, in vec2 fragCoord) function; none if not used")
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
		("iChanneli",          po::value<std::string>(), "Set name of iChanneli uniform")
		("iSurfacePosition",   po::value<std::string>(), "Set name of iSurfacePosition varying");

	po::options_description desc;
	desc.add(gen).add(uni);

	po::variables_map vm;
	try {
		vm = parseArguments(argc, argv, desc);
	}
	catch(std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;

		return EXIT_FAILURE;
	}
	
	if(vm.count("help")) {
		std::cout << "Usage: run_fragment [options] file" << std::endl;
		std::cout << desc << std::endl;

		return EXIT_SUCCESS;
	}
	
	RunFragment::Configuration config;
	
	config.file = vm["file"].as<std::string>();
	config.time = vm["time"].as<float>();
	if(vm.count("main") && vm["main"].as<std::string>() != "none") {
		config.main = vm["main"].as<std::string>();
	}
	else {
		config.main = boost::none;
	}

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
	config.iSurfacePosition = vm["iSurfacePosition"].as<std::string>();

	try {
		RunFragment::Application application {config};
		application.run();
	}
	catch(std::exception& e) {
		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
