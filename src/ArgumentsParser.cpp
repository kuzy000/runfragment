#include "ArgumentsParser.h"

#include <fstream>

#include <boost/filesystem.hpp>

#include "Option.h"
#include "StandartConfig.h"
#include "Utils.h"
#include "Format.h"

namespace RunFragment {
namespace ArgumentsParser {

namespace po = boost::program_options;
namespace fs = boost::filesystem;

boost::program_options::variables_map argsToVm(int argc, char* argv[], po::options_description desc) {
	po::positional_options_description pos;
	pos.add(Option::image, 1);

	po::variables_map vm;
	
	auto parser = po::command_line_parser(argc, argv).options(desc).positional(pos);
	po::store(parser.run(), vm);
	
	if(vm.count(Option::help) || vm.count(Option::download) || vm.count(Option::create)) {
		return vm;
	}
	
	if(vm.count(Option::config)) {
		const auto path = vm[Option::config].as<fs::path>();
		if(!Utils::isFileAccessible(path.string())) {
			throw std::runtime_error {"can't open config file"};
		}
		fs::ifstream file {path};
		
		
		const auto dir = fs::absolute(fs::path {path}).parent_path();
		const auto current = fs::current_path();
		
		fs::current_path(dir);
		po::variables_map vmConfig;
		po::store(po::parse_config_file(file, desc, true), vmConfig);  
		fs::current_path(current);
		
		vm.insert(vmConfig.begin(), vmConfig.end());
	}
	else {
		if(!vm.count(Option::image)) {
			throw std::runtime_error {"neither config nor Image specified"};
		}
	}
	
	const auto format = vm.count(Option::format) ? vm[Option::format].as<Format>() : defaultFormat;
	switch(format) {
	case Format::ShaderToy: {
		std::istringstream ss {StandartConfig::shaderToy};
		po::store(po::parse_config_file(ss, desc, true), vm);
		break;
	}
	case Format::GLSLSandbox: {
		std::istringstream ss {StandartConfig::glslSandbox};
		po::store(po::parse_config_file(ss, desc, true), vm);
		break;
	}
	}
	
	return vm;
}

RunFragment::AppConfig vmToAppConfig(const boost::program_options::variables_map& vm) {
	const auto lookupOptional = [&vm] (std::string name) -> boost::optional<std::string> {
		if(vm.count(name) && vm[name].as<std::string>() != Option::none) {
			return vm[name].as<std::string>();
		}
		else {
			return boost::none;
		}
	};
	
	const auto lookupOptionalPath = [&vm] (std::string name) -> boost::optional<fs::path> {
		if(vm.count(name)) {
			return vm[name].as<fs::path>();
		}
		else {
			return boost::none;
		}
	};
	
	const auto lookupString = [&vm] (std::string name) -> std::string {
		return vm[name].as<std::string>();
	};
	
	const auto lookupPath = [&vm] (std::string name) -> fs::path {
		return vm[name].as<fs::path>();
	};
	
	const auto lookupRenderConfig = [&vm, &lookupOptionalPath] (std::string name, const std::array<Parameter, 4>& optionChannels) -> boost::optional<RenderConfig> {
		auto path = lookupOptionalPath(name);
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
	
	config.time = vm[Option::time].as<float>();
	config.main = lookupOptional(Option::main);
	config.addUniforms = vm[Option::addUniform].as<bool>();

	config.iResolution        = lookupString(Option::iResolution);
	config.iGlobalTime        = lookupString(Option::iGlobalTime);
	config.iTimeDelta         = lookupString(Option::iTimeDelta);
	config.iFrame             = lookupString(Option::iFrame);
	config.iFrameRate         = lookupString(Option::iFrameRate);
	config.iChannelTime       = lookupString(Option::iChannelTime);
	config.iChannelResolution = lookupString(Option::iChannelResolution);
	config.iMouse             = lookupString(Option::iMouse);
	config.iDate              = lookupString(Option::iDate);
	config.iSampleRate        = lookupString(Option::iSampleRate);
	config.iChannel           = lookupString(Option::iChannel);
	config.iSurfacePosition   = lookupString(Option::iSurfacePosition);
	
	return config;
}

}
}
