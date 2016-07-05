#pragma once

#include <string>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <array>

namespace RunFragment {

struct Configuration {
	enum class Channel {
		BufA = 0,
		BufB,
		BufC,
		BufD,
		Count,
	};
	
	enum class Buf {
		A = 0,
		B,
		C,
		D,
		Count,
	};
	
	struct ChannelBuf {
		Buf buf;
	};
	
	struct BufType {
		boost::optional<std::string> filename;
		std::array<boost::variant<Buf>, static_cast<std::size_t>(Channel::Count)> channels {};
	};
	
	BufType image {};
	std::array<BufType, static_cast<std::size_t>(Buf::Count)> bufs {};

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
