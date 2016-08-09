#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <thread>

#include <boost/filesystem/path.hpp>

namespace RunFragment {

namespace fs = boost::filesystem;

class FileWatcher {
	// TODO: Use boost::multi_index for inotify wd storing
	using Map = std::unordered_map<std::string, std::function<void()>>;
public:
	FileWatcher() = default;
	
	void add(fs::path path, std::function<void()> callback);
	std::thread spawn();
	
private:
	void run();
	
	Map pathOnChange;
};

}
