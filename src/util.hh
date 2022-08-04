#pragma once
#include "_components.hh"

namespace Util {
	bool        IsInteger(std::string str);
	bool        IsFloat(std::string str);
	bool        IsBool(std::string str);
	std::string BaseName(std::string path);
}
