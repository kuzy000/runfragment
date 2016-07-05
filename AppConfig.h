#pragma once

#include <string>
#include <array>

#include <boost/optional.hpp>

#include "RenderConfig.h"

namespace RunFragment {

struct AppConfig {
	boost::optional<RenderConfig> image;
	std::array<boost::optional<RenderConfig>, 4> bufs;

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
};

}
