#pragma once

#include <string>

namespace RunFragment {

struct Configuration {
	std::string file;
	float time;

	std::string iResolution;
	std::string iGlobalTime;
	std::string iGlobalDelta;
	std::string iGlobalFrame;
	std::string iChannelTime;
	std::string iMouse;
	std::string iDate;
	std::string iSampleRate;
	std::string iChannelResolution;
	std::string iChanneli;
	std::string iSurfacePosition;
};

}
