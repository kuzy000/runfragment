#pragma once

#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>

namespace RunFragment {
namespace Utils {

inline bool isFileAccessible(std::string path) {
	return (access(path.c_str(), F_OK) != -1);
}

template <typename Iter>
std::size_t indexOf(Iter first, Iter last, const typename std::iterator_traits<Iter>::value_type& value) {
	return std::distance(first, std::find(first, last, value));
}

template <typename T>
std::string toString(const T& value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

template <typename T>
std::string toString(T&& value) {
	std::stringstream ss;
	ss << std::move(value);
	return ss.str();
}

}
}
