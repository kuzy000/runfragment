#pragma once

#include <memory>

namespace RunFragment {

class Channel {
public:
	virtual ~Channel() = default;
	
	static std::unique_ptr<Channel> fromString(std::string str);
};

class ChannelBuf : public Channel {
public:
	enum class Kind { BufA = 0, BufB, BufC, BufD };
	
	ChannelBuf(Kind kind)
		: kind {kind} {}
	
	const Kind kind;
};

}

