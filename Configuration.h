#pragma once

#include <utility>

#include <boost/optional.hpp>
#include <boost/program_options/variables_map.hpp>

namespace RunFragment {

namespace po = boost::program_options;

struct Configuration {
	boost::optional<std::string> file;
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
};

}
