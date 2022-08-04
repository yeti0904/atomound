#include "util.hh"

bool Util::IsInteger(std::string str) {
	if (str == "-") {
		return false;
	}
	if (str == "") {
		return false;
	}
	for (size_t i = 0; i < str.length(); ++i) {
		if (!(
			isdigit(str[i]) ||
			((i == 0) && (str[i] == '-'))
		)) {
			return false;
		}
	}
	return true;
}

bool Util::IsFloat(std::string str) {
	if (str == "-") {
		return false;
	}
	if (str == "") {
		return false;
	}
	if (str.back() == '.') {
		return false;
	}
	if (std::count(str.begin(), str.end(), '-') > 1) {
		return false;
	}

	for (size_t i = 0; i < str.length(); ++i) {
		if (!(
			isdigit(str[i]) ||
			((i == 0) && (str[i] == '-')) ||
			(str[i] == '.')
		)) {
			return false;
		}
	}
	return true;
}

bool Util::IsBool(std::string str) {
	return (str == "true") || (str == "false");
}

std::string Util::BaseName(std::string path) {
	size_t pos = path.rfind('/');
	if (pos == std::string::npos) {
		return path;
	}
	return path.substr(pos + 1);
}
