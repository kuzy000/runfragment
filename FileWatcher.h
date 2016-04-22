#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <thread>

namespace RunFragment {

class FileWatcher {
	// TODO: Use boost::multi_index for inotify wd storing
	using Map = std::unordered_map<std::string, std::function<void()>>;
public:
	FileWatcher() = default;
	
	void add(std::string path, std::function<void()> callback);
	std::thread spawn();
	
private:
	void run();
	
	Map pathOnChange;
};

}
