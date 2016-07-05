#pragma once

#include <string>
#include <algorithm>
#include <iterator>

namespace RunFragment {
namespace Utils {

inline bool isFileAccessible(std::string path) {
	return (access(path.c_str(), F_OK) != -1);
}

template <typename Iter>
std::size_t indexOf(Iter first, Iter last, const typename std::iterator_traits<Iter>::value_type& value) {
	return std::distance(first, std::find(first, last, value));
}

}
}
