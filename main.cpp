#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <memory>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#define BOOST_NETWORK_ENABLE_HTTPS
#include <boost/network/include/http/client.hpp>

#include "Application.h"
#include "AppConfig.h"
#include "Channel.h"
#include "Option.h"
#include "StandartConfig.h"
#include "Utils.h"

namespace po = boost::program_options;

po::variables_map parseArguments(int argc, char* argv[], po::options_description desc) {
	using namespace RunFragment;
	
	po::positional_options_description pos;
	pos.add(Option::image, 1);

	po::variables_map vm;
	auto parser = po::command_line_parser(argc, argv).options(desc).positional(pos);
	po::store(parser.run(), vm);
	
	if(vm.count(Option::help) || vm.count(Option::download)) {
		return vm;
	}
	
	if(vm.count(Option::format)) {
		auto arg = vm[Option::format].as<std::string>();
		if(arg == "s") {
			std::istringstream ss {StandartConfig::shaderToy};
			po::store(po::parse_config_file(ss, desc, true), vm);
		}
		else if(arg == "g") {
			std::istringstream ss {StandartConfig::glslSandbox};
			po::store(po::parse_config_file(ss, desc, true), vm);
		}
		else {
			throw po::validation_error {po::validation_error::invalid_option_value, Option::format, arg};
		}
	}
	else {
		std::istringstream ss {StandartConfig::defaultConfig};
		po::store(po::parse_config_file(ss, desc, true), vm);
	}
	
	if(vm.count(Option::config)) {
		auto path = vm[Option::config].as<std::string>();
		if(!Utils::isFileAccessible(path)) {
			throw std::runtime_error {"can't open config file"};
		}
		std::ifstream file {path};
		po::store(po::parse_config_file(file, desc, true), vm);  
	}
	else {
		if(!vm.count(Option::image)) {
			throw std::runtime_error {"neither config nor Image specified"};
		}
	}
	
	return vm;
}

RunFragment::AppConfig vmToConfiguraion(const po::variables_map& vm) {
	using namespace RunFragment;
	
	const auto lookupOptional = [&vm] (std::string name) -> boost::optional<std::string> {
		if(vm.count(name) && vm[name].as<std::string>() != "none") {
			return vm[name].as<std::string>();
		}
		else {
			return boost::none;
		}
	};
	
	const auto lookupString = [&vm] (std::string name) -> std::string {
		return vm[name].as<std::string>();
	};
	
	const auto lookupRenderConfig = [&vm, &lookupOptional] (std::string name, const std::array<Parameter, 4>& optionChannels) -> boost::optional<RenderConfig> {
		auto path = lookupOptional(name);
		if(!path) {
			return boost::none;
		}
		
		RenderConfig renderConfig {*path};
		
		for(std::size_t i = 0; i < renderConfig.channels.size(); i++) {
			auto& channel = renderConfig.channels[i];
			const std::string& channelName = optionChannels[i];
			
			if(vm.count(channelName)) {
				channel = std::unique_ptr<Channel> {vm[channelName].as<Channel*>()};
			}
		}
		return renderConfig;
	};
	
	AppConfig config;
	
	config.image = lookupRenderConfig(Option::image, Option::imageChannels);
	for(std::size_t i = 0; i < config.bufs.size(); i++) {
		config.bufs[i] = lookupRenderConfig(Option::bufs[i], Option::bufChannels[i]);
	}
	
	float time;
	boost::optional<std::string> main;
	bool addUniforms;

	config.time = vm[Option::time].as<float>();
	config.main = lookupOptional(Option::main);
	config.addUniforms = vm[Option::addUniform].as<bool>();

	config.iResolution        = lookupString(Option::iResolution);
	config.iGlobalTime        = lookupString(Option::iGlobalTime);
	config.iGlobalDelta       = lookupString(Option::iGlobalDelta);
	config.iGlobalFrame       = lookupString(Option::iGlobalFrame);
	config.iChannelTime       = lookupString(Option::iChannelTime);
	config.iMouse             = lookupString(Option::iMouse);
	config.iDate              = lookupString(Option::iDate);
	config.iSampleRate        = lookupString(Option::iSampleRate);
	config.iChannelResolution = lookupString(Option::iChannelResolution);
	config.iChannel           = lookupString(Option::iChannel);
	config.iSurfacePosition   = lookupString(Option::iSurfacePosition);
	
	return config;
}

int main(int argc, char* argv[]) {
	using namespace RunFragment;
	
	po::variables_map vm;
	try {
		vm = parseArguments(argc, argv, Option::parsingOptions);
	}
	catch(std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Try '--" << Option::help << "'" << std::endl;

		return EXIT_FAILURE;
	}
	
	if(vm.count(Option::help)) {
		std::cout << "Usage: " << argv[0] << " [options] [Image_shader]" << std::endl;
		std::cout << Option::helpOptions << std::endl;

		return EXIT_SUCCESS;
	}
	
	if(vm.count(Option::download)) {
		auto uri = std::unique_ptr<AllowedURI> {vm[Option::download].as<AllowedURI*>()};
		
		return EXIT_SUCCESS;
	}
	
	try {
		Application application {vmToConfiguraion(vm)};
		application.run();
	}
	catch(std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
