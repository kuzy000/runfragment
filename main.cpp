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
add-uniforms = true

iResolution = iResolution
iGlobalTime = iGlobalTime
iGlobalDelta = iGlobalDelta
iGlobalFrame = iGlobalFrame
iChannelTime = iChannelTime
iMouse = iMouse
iDate = iDate
iSampleRate = iSampleRate
iChannelResolution = iChannelResolution
iChannel = iChannel
iSurfacePosition = iSurfacePosition
)raw";

const std::string glslSandbox =  R"raw(
time = 1
main = none
add-uniforms = false

iResolution = resolution
iGlobalTime = time
iGlobalDelta = iGlobalDelta
iGlobalFrame = iGlobalFrame
iChannelTime = iChannelTime
iMouse = mouse
iDate = iDate
iSampleRate = iSampleRate
iChannelResolution = iChannelResolution
iChannel = iChannel
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
	
	if(!vm.count("file") && !vm.count("help")) {
		throw std::runtime_error {"file doesn't specified; try '--help'"};
	}
	
	return vm;
}

int main(int argc, char* argv[]) {
	po::options_description gen;
	gen.add_options()
		("file",         po::value<std::string>(),                "Input file")
		("config,c",     po::value<std::string>(),                "Config file")
		("time,t",       po::value<float>(),                      "Set value that multiplies the time")
		("help,h",                                                "Display help message")
		("format,f",     po::value<std::string>(),                "Format of file:\n    g : GLSLSandbox\n    s : ShaderToy (default)")
		("main",         po::value<std::string>(),                "Set name of mainImage(out vec4, in vec2) function; none if not used")
		("add-uniforms", po::bool_switch()->default_value(false), "Add uniforms to begining of the file")
//		("channels",     , "Set list of channels data");
		("channel0",     po::value<std::string>(),                "Set channel0 data (shader or png); none if not used")
		("channel1",     po::value<std::string>(),                "Set channel1 data (shader or png); none if not used")
		("channel2",     po::value<std::string>(),                "Set channel2 data (shader or png); none if not used")
		("channel3",     po::value<std::string>(),                "Set channel3 data (shader or png); none if not used");

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
		("iChannel",           po::value<std::string>(), "Set name of iChannelN uniform")
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
	
	auto lookupOptional = [&vm] (std::string name) -> boost::optional<std::string> {
		if(vm.count(name) && vm[name].as<std::string>() != "none") {
			return vm[name].as<std::string>();
		}
		else {
			return boost::none;
		}
	};
	
	RunFragment::Configuration config;
	
	config.file = vm["file"].as<std::string>();
	config.time = vm["time"].as<float>();
	config.main = lookupOptional("main");
	config.addUniforms = vm["add-uniforms"].as<bool>();
	config.channel0 = lookupOptional("channel0");
	config.channel1 = lookupOptional("channel1");
	config.channel2 = lookupOptional("channel2");
	config.channel3 = lookupOptional("channel3");

	config.iResolution = vm["iResolution"].as<std::string>();
	config.iGlobalTime = vm["iGlobalTime"].as<std::string>();
	config.iGlobalDelta = vm["iGlobalDelta"].as<std::string>();
	config.iGlobalFrame = vm["iGlobalFrame"].as<std::string>();
	config.iChannelTime = vm["iChannelTime"].as<std::string>();
	config.iMouse = vm["iMouse"].as<std::string>();
	config.iDate = vm["iDate"].as<std::string>();
	config.iSampleRate = vm["iSampleRate"].as<std::string>();
	config.iChannelResolution = vm["iChannelResolution"].as<std::string>();
	config.iChannel = vm["iChannel"].as<std::string>();
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
