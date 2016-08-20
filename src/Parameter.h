#pragma once

#include <string>

namespace RunFragment {

class Parameter {
public:
	Parameter(const char* value)
		: value {value}, boostPoValue {value} {}
		
	Parameter(std::string value)
		: value {std::move(value)}, boostPoValue {this->value} {}
	
	Parameter(std::string value, char letter)
		: value {std::move(value)}, boostPoValue {this->value + "," + letter} {}
	
	bool operator ==(const Parameter& rhs) const {
		return value == rhs.value;
	}
	
	bool operator !=(const Parameter& rhs) const {
		return !(*this == rhs);
	}
	
	operator std::string() const {
		return value;
	}
	
	operator const char*() const {
		return value.c_str();
	}
	
	const char* getPo() const {
		return boostPoValue.c_str();
	}
	
private:
	const std::string value;
	const std::string boostPoValue;
};

}
