#pragma once

#include <efsw/efsw.hpp>

#include <functional>

namespace RunFragment {

class FileWatchListenerLambda : public efsw::FileWatchListener {
public:
	FileWatchListenerLambda(std::function<void(efsw::WatchID, const std::string&, const std::string&, efsw::Action, std::string)> callback)
		: callback {callback} {}
	
	void handleFileAction(efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "") override {
		callback(watchid, dir, filename, action, oldFilename);
	}
private:
	std::function<void(efsw::WatchID, const std::string&, const std::string&, efsw::Action, std::string)> callback;
};


}
