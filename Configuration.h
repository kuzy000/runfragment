#pragma once

#include <string>

#include <boost/optional.hpp>
#include <array>

namespace RunFragment {

struct Configuration {
	std::string file;
	float time;
	boost::optional<std::string> main;
	bool addUniforms;

	std::string iResolution;
	std::string iGlobalTime;
	std::string iGlobalDelta;
	std::string iGlobalFrame;
	std::string iChannelTime;
	std::string iMouse;
	std::string iDate;
	std::string iSampleRate;
	std::string iChannelResolution;
	std::string iChannel;
	std::string iSurfacePosition;
	
	std::array<boost::optional<std::string>, 4> channels {};
};

}
