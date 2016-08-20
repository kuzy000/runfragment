#pragma once

#include <functional>
#include <stack>

namespace RunFragment {

class OnScopeEnd {
public:
	OnScopeEnd() = default;
	
	~OnScopeEnd() {
		while (!functions.empty()) {
			functions.top()();
			functions.pop();
		}
	}
	
	OnScopeEnd(const OnScopeEnd&) = delete;
	OnScopeEnd& operator =(const OnScopeEnd&) = delete;
	
	OnScopeEnd(OnScopeEnd&&) = default;
	OnScopeEnd& operator =(OnScopeEnd&&) = default;
	
	void push(std::function<void()> value) {
		functions.push(value);
	}
	
	void pop() {
		functions.pop();
	}
private:
	std::stack<std::function<void()>> functions;
};

}
