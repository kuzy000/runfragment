#include "FileWatcher.h"

#include <sys/inotify.h>
#include <climits>
#include <unistd.h>
#include <iostream>

#include "Utils.h"

namespace RunFragment {

void FileWatcher::add(fs::path path, std::function<void()> callback) {
	pathOnChange.emplace(std::move(path.string()), std::move(callback));
}

std::thread FileWatcher::spawn() {
	return std::thread {&FileWatcher::run, this};
}

void FileWatcher::run() {
	auto fd = inotify_init();
	if(fd == -1) {
		throw std::runtime_error {"inotify_init returned -1"};
	}
	
	std::unordered_map<int, Map::iterator> wdIt;
	
	for(Map::iterator it = pathOnChange.begin(); it != pathOnChange.end(); it++) {
		const std::string& path = it->first;
		
		auto wd = inotify_add_watch(fd, path.c_str(), IN_CLOSE_WRITE);
		if(wd == -1) {
			bool accessible = Utils::isFileAccessible(path);
			if(!accessible) {
				std::cerr << "Error: can't monitor file '" << path << "'" << std::endl;
				continue;
			}
			throw std::runtime_error {"inotify_add_watch returned -1"};
		}
		else {
			wdIt.emplace(wd, it);
		}
	}
	
	while(true) {
		const size_t bufferLength = (sizeof(inotify_event) + NAME_MAX + 1) * 10;
		char buffer[bufferLength];
	
		auto numRead = read(fd, buffer, bufferLength);
		if(numRead == 0) {
			throw std::runtime_error {"read() from inotify fd returned 0"};
		}
	
		if(numRead == -1) {
			throw std::runtime_error {"read() from inotify fd returned -1"};
		}
	
		for(char* p = buffer; p < buffer + numRead; ) {
			auto event = reinterpret_cast<inotify_event*>(p);
			auto wd = event->wd;
			auto it = wdIt.at(wd);
			const std::string& path = it->first;
			
			if(event->mask & IN_IGNORED) {
				inotify_rm_watch(fd, wd);
				auto newWd = inotify_add_watch(fd, path.c_str(), IN_MODIFY);
				if(wd == -1) {
					throw std::runtime_error {"inotify_add_watch returned -1"};
				}
				auto itOfWdIt = wdIt.find(wd);
				std::swap(wdIt.at(newWd), itOfWdIt->second);
				wdIt.erase(wd);
			}
			pathOnChange.at(path)();
	
			p += sizeof(inotify_event) + event->len;
		}
	}

	for(auto pair : wdIt) {
		auto wd = pair.first;
		inotify_rm_watch(fd, wd);
	}
	close(fd);
}

}
