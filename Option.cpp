#include "Option.h"

#include "AllowedURI.h"
#include "Channel.h"

namespace RunFragment {
namespace Option {

const Parameter image {"Image", 'i'};
const std::array<Parameter, 4> imageChannels = {{
	"Image-c0", "Image-c1", "Image-c2", "Image-c3"
}};

const std::array<Parameter, 4> bufs = {"BufA", "BufB", "BufC", "BufD"};
const std::array<std::array<Parameter, 4>, 4> bufChannels = 
{{
	{"BufA-c0", "BufA-c1", "BufA-c2", "BufA-c3"},
	{"BufB-c0", "BufB-c1", "BufB-c2", "BufB-c3"},
	{"BufC-c0", "BufC-c1", "BufC-c2", "BufC-c3"},
	{"BufD-c0", "BufD-c1", "BufD-c2", "BufD-c3"},
}};

const Parameter config {"config", 'c'};
const Parameter download = {"download", 'd'};
const Parameter time {"time", 't'};
const Parameter help {"help", 'h'};
const Parameter format {"format", 'f'};
const Parameter main = "main";
const Parameter addUniform = "add-uniforms";

const Parameter iResolution = "iResolution";
const Parameter iGlobalTime = "iGlobalTime";
const Parameter iGlobalDelta = "iGlobalDelta";
const Parameter iGlobalFrame = "iGlobalFrame";
const Parameter iChannelTime = "iChannelTime";
const Parameter iMouse = "iMouse";
const Parameter iDate = "iDate";
const Parameter iSampleRate = "iSampleRate";
const Parameter iChannelResolution = "iChannelResolution";
const Parameter iChannel = "iChannel";
const Parameter iSurfacePosition = "iSurfacePosition";

namespace po = boost::program_options;

const po::options_description helpOptions = [] {
	po::options_description general {"General"};
	general.add_options()
		(config.getPo(),   po::value<std::string>(), "Config file")
//		(download.getPo(), po::value<AllowedURI*>(),  "Download a project from shadertoy.com or glslsanbox.com")
		(download.getPo(), po::value<AllowedURI*>(),  "Download a project from glslsanbox.com")
		(help.getPo(),                               "Display help message");
	
	po::options_description buffersChannels {"Buffers and their channels"};
	buffersChannels.add_options()
		(image.getPo(),           po::value<std::string>(), "Set Image shader file")
		("Image-c{0 1 2 3}",        po::value<std::string>(), "Set Image channel {0 1 2 3}:\n    Buf{A B C D} : buffer as a channel (multipass)")
		("Buf{A B C D}",            po::value<std::string>(), "Set Buf{A B C D} shader file; 'none' if not used")
		("Buf{A B C D}-c{0 1 2 3}", po::value<std::string>(), "Set Buf{A B C D} channel {0 1 2 3}:\n    Buf{A B C D} : buffer as a channel (multipass)");
	
	
	po::options_description other {"Other"};
	other.add_options()
		(format.getPo(),             po::value<std::string>(), "Format of uniforms:\n    g : GLSLSandbox\n    s : ShaderToy (default)")
		(time.getPo(),               po::value<float>(),       "Set value that multiplies the time")
		(main.getPo(),               po::value<std::string>(), "Set name of mainImage(out vec4, in vec2) function; 'none' if not used")
		(addUniform.getPo(),         po::bool_switch(),        "Add uniforms to beginning of the file")
		(iResolution.getPo(),        po::value<std::string>(), "Set name of iResolution uniform")
		(iGlobalTime.getPo(),        po::value<std::string>(), "Set name of iGlobalTime uniform")
//		(iGlobalDelta.getPo(),       po::value<std::string>(), "Set name of iGlobalDelta uniform")
//		(iGlobalFrame.getPo(),       po::value<std::string>(), "Set name of iGlobalFrame uniform")
//		(iChannelTime.getPo(),       po::value<std::string>(), "Set name of iChannelTime uniform")
		(iMouse.getPo(),             po::value<std::string>(), "Set name of iMouse uniform")
		(iDate.getPo(),              po::value<std::string>(), "Set name of iDate uniform")
//		(iSampleRate.getPo(),        po::value<std::string>(), "Set name of iSampleRate uniform")
//		(iChannelResolution.getPo(), po::value<std::string>(), "Set name of iChannelResolution uniform")
		(iChannel.getPo(),           po::value<std::string>(), "Set name of iChannelN uniform")
		(iSurfacePosition.getPo(),   po::value<std::string>(), "Set name of iSurfacePosition varying");
	
	po::options_description desc;
	desc.add(general).add(buffersChannels).add(other);
	return desc;
} ();

const po::options_description parsingOptions = [] {
	po::options_description general {"General"};
	general.add_options()
		(config.getPo(),   po::value<std::string>(), "Config file")
		(download.getPo(), po::value<AllowedURI*>(),  "Download a project from shadertoy.com or glslsanbox.com")
		(help.getPo(),                               "Display help message");
	
	po::options_description buffersChannels {"Buffers and their channels"};
	buffersChannels.add_options()
		(image.getPo(),                     po::value<std::string>(), "Set Image shader file");
	
	for(std::size_t i = 0; i < imageChannels.size(); i++) {
		buffersChannels.add_options()
			(imageChannels.at(i).getPo(), po::value<Channel*>());
	}
	
	for(std::size_t i = 0; i < bufs.size(); i++) {
		buffersChannels.add_options()
			(bufs.at(i).getPo(), po::value<std::string>());
		
		for(std::size_t j = 0; j < std::tuple_size<decltype(bufChannels)::value_type>::value; j++) {
			buffersChannels.add_options()
				(bufChannels.at(i).at(j).getPo(), po::value<Channel*>());
		}
	}
	
	po::options_description other {"Other"};
	other.add_options()
		(format.getPo(),             po::value<std::string>(), "Format of uniforms:\n    g : GLSLSandbox\n    s : ShaderToy (default)")
		(time.getPo(),               po::value<float>(),       "Set value that multiplies the time")
		(main.getPo(),               po::value<std::string>(), "Set name of mainImage(out vec4, in vec2) function; 'none' if not used")
		(addUniform.getPo(),         po::bool_switch(),        "Add uniforms to beginning of the file")
		(iResolution.getPo(),        po::value<std::string>(), "Set name of iResolution uniform")
		(iGlobalTime.getPo(),        po::value<std::string>(), "Set name of iGlobalTime uniform")
		(iGlobalDelta.getPo(),       po::value<std::string>(), "Set name of iGlobalDelta uniform")
		(iGlobalFrame.getPo(),       po::value<std::string>(), "Set name of iGlobalFrame uniform")
		(iChannelTime.getPo(),       po::value<std::string>(), "Set name of iChannelTime uniform")
		(iMouse.getPo(),             po::value<std::string>(), "Set name of iMouse uniform")
		(iDate.getPo(),              po::value<std::string>(), "Set name of iDate uniform")
		(iSampleRate.getPo(),        po::value<std::string>(), "Set name of iSampleRate uniform")
		(iChannelResolution.getPo(), po::value<std::string>(), "Set name of iChannelResolution uniform")
		(iChannel.getPo(),           po::value<std::string>(), "Set name of iChannelN uniform")
		(iSurfacePosition.getPo(),   po::value<std::string>(), "Set name of iSurfacePosition varying");
	
	po::options_description desc;
	desc.add(general).add(buffersChannels).add(other);
	return desc;
} ();

}
}
