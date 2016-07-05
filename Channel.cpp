#include "Channel.h"

#include <array>
#include <algorithm>

namespace RunFragment {

std::unique_ptr<Channel> Channel::fromString(std::string str) {
	std::array<std::string, 4> bufs {{ "BufA", "BufB", "BufC", "BufD" }};
	const auto it = std::find(bufs.begin(), bufs.end(), str);

	if(it != bufs.end()) {
		auto kind = static_cast<ChannelBuf::Kind>(std::distance(bufs.begin(), it));
		
		return std::unique_ptr<Channel> {new ChannelBuf {kind}};
	}

	return {nullptr};
}

}
