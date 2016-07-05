#pragma once

#include <string>
#include <array>
#include <boost/program_options/options_description.hpp>

namespace RunFragment {
namespace Option {

class WithPostfix {
public:
	WithPostfix(const char* value)
		: value {value}, full {value} {}
		
	WithPostfix(std::string value)
		: value {std::move(value)}, full {value} {}
	
	WithPostfix(std::string value, std::string postfix)
		: value {std::move(value)}, full {this->value + "," + std::move(postfix)} {}
	
	bool operator ==(const WithPostfix& rhs) const {
		return value == rhs.value;
	}
	
	operator std::string() const {
		return value;
	}
	
	operator const char*() const {
		return value.c_str();
	}
	
	const char* getFull() const {
		return full.c_str();
	}
	
private:
	std::string value;
	std::string full;
};


const WithPostfix image {"Image", "i"};
const std::array<WithPostfix, 4> imageChannels = {{
	"Image-c0", "Image-c1", "Image-c2", "Image-c3"
}};

const std::array<WithPostfix, 4> bufs = {"BufA", "BufB", "BufC", "BufD"};
const std::array<std::array<WithPostfix, 4>, 4> bufChannels = 
{{
	{"BufA-c0", "BufA-c1", "BufA-c2", "BufA-c3"},
	{"BufB-c0", "BufB-c1", "BufB-c2", "BufB-c3"},
	{"BufC-c0", "BufC-c1", "BufC-c2", "BufC-c3"},
	{"BufD-c0", "BufD-c1", "BufD-c2", "BufD-c3"},
}};

const WithPostfix config {"config", "c"};
const WithPostfix time {"time", "t"};
const WithPostfix help {"help", "h"};
const WithPostfix format {"format", "f"};
const WithPostfix main = "main";
const WithPostfix addUniform = "add-uniforms";

const WithPostfix iResolution = "iResolution";
const WithPostfix iGlobalTime = "iGlobalTime";
const WithPostfix iGlobalDelta = "iGlobalDelta";
const WithPostfix iGlobalFrame = "iGlobalFrame";
const WithPostfix iChannelTime = "iChannelTime";
const WithPostfix iMouse = "iMouse";
const WithPostfix iDate = "iDate";
const WithPostfix iSampleRate = "iSampleRate";
const WithPostfix iChannelResolution = "iChannelResolution";
const WithPostfix iChannel = "iChannel";
const WithPostfix iSurfacePosition = "iSurfacePosition";

namespace po = boost::program_options;

const po::options_description helpOptions = [] {
	po::options_description general {"General"};
	general.add_options()
		(config.getFull(), po::value<std::string>(), "Config file")
		(help.getFull(),                             "Display help message");
	
	po::options_description buffersChannels {"Buffers and their channels"};
	buffersChannels.add_options()
		(image.getFull(),           po::value<std::string>(), "Set Image shader file")
		("Image-c{0 1 2 3}",        po::value<std::string>(), "Set Image channel {0 1 2 3}:\n    Buf{A B C D} : buffer as a channel (multipass)")
		("Buf{A B C D}",            po::value<std::string>(), "Set Buf{A B C D} shader file; 'none' if not used")
		("Buf{A B C D}-c{0 1 2 3}", po::value<std::string>(), "Set Buf{A B C D} channel {0 1 2 3}:\n    Buf{A B C D} : buffer as a channel (multipass)");
	
	
	po::options_description other {"Other"};
	other.add_options()
		(format.getFull(),             po::value<std::string>(), "Format of uniforms:\n    g : GLSLSandbox\n    s : ShaderToy (default)")
		(time.getFull(),               po::value<float>(),       "Set value that multiplies the time")
		(main.getFull(),               po::value<std::string>(), "Set name of mainImage(out vec4, in vec2) function; 'none' if not used")
		(addUniform.getFull(),         po::bool_switch(),        "Add uniforms to beginning of the file")
		(iResolution.getFull(),        po::value<std::string>(), "Set name of iResolution uniform")
		(iGlobalTime.getFull(),        po::value<std::string>(), "Set name of iGlobalTime uniform")
//		(iGlobalDelta.getFull(),       po::value<std::string>(), "Set name of iGlobalDelta uniform")
//		(iGlobalFrame.getFull(),       po::value<std::string>(), "Set name of iGlobalFrame uniform")
//		(iChannelTime.getFull(),       po::value<std::string>(), "Set name of iChannelTime uniform")
		(iMouse.getFull(),             po::value<std::string>(), "Set name of iMouse uniform")
		(iDate.getFull(),              po::value<std::string>(), "Set name of iDate uniform")
//		(iSampleRate.getFull(),        po::value<std::string>(), "Set name of iSampleRate uniform")
//		(iChannelResolution.getFull(), po::value<std::string>(), "Set name of iChannelResolution uniform")
		(iChannel.getFull(),           po::value<std::string>(), "Set name of iChannelN uniform")
		(iSurfacePosition.getFull(),   po::value<std::string>(), "Set name of iSurfacePosition varying");
	
	po::options_description desc;
	desc.add(general).add(buffersChannels).add(other);
	return desc;
} ();

const po::options_description parsingOptions = [] {
	po::options_description general {"General"};
	general.add_options()
		(config.getFull(), po::value<std::string>(), "Config file")
		(help.getFull(),                             "Display help message");
	
	po::options_description buffersChannels {"Buffers and their channels"};
	buffersChannels.add_options()
		(image.getFull(),                     po::value<std::string>(), "Set Image shader file");
	
	for(std::size_t i = 0; i < imageChannels.size(); i++) {
		buffersChannels.add_options()(imageChannels.at(i).getFull(), po::value<std::string>());
	}
	
	for(std::size_t i = 0; i < bufs.size(); i++) {
		buffersChannels.add_options()(bufs.at(i).getFull(), po::value<std::string>());
		
		for(std::size_t j = 0; j < std::tuple_size<decltype(bufChannels)::value_type>::value; j++) {
			buffersChannels.add_options()(bufChannels.at(i).at(j).getFull(), po::value<std::string>());
		}
	}
	
	po::options_description other {"Other"};
	other.add_options()
		(format.getFull(),             po::value<std::string>(), "Format of uniforms:\n    g : GLSLSandbox\n    s : ShaderToy (default)")
		(time.getFull(),               po::value<float>(),       "Set value that multiplies the time")
		(main.getFull(),               po::value<std::string>(), "Set name of mainImage(out vec4, in vec2) function; 'none' if not used")
		(addUniform.getFull(),         po::bool_switch(),        "Add uniforms to beginning of the file")
		(iResolution.getFull(),        po::value<std::string>(), "Set name of iResolution uniform")
		(iGlobalTime.getFull(),        po::value<std::string>(), "Set name of iGlobalTime uniform")
		(iGlobalDelta.getFull(),       po::value<std::string>(), "Set name of iGlobalDelta uniform")
		(iGlobalFrame.getFull(),       po::value<std::string>(), "Set name of iGlobalFrame uniform")
		(iChannelTime.getFull(),       po::value<std::string>(), "Set name of iChannelTime uniform")
		(iMouse.getFull(),             po::value<std::string>(), "Set name of iMouse uniform")
		(iDate.getFull(),              po::value<std::string>(), "Set name of iDate uniform")
		(iSampleRate.getFull(),        po::value<std::string>(), "Set name of iSampleRate uniform")
		(iChannelResolution.getFull(), po::value<std::string>(), "Set name of iChannelResolution uniform")
		(iChannel.getFull(),           po::value<std::string>(), "Set name of iChannelN uniform")
		(iSurfacePosition.getFull(),   po::value<std::string>(), "Set name of iSurfacePosition varying");
	
	po::options_description desc;
	desc.add(general).add(buffersChannels).add(other);
	return desc;
} ();


}
}

