#include "builtin.hh"

void BuiltIn::Print(Language::LanguageComponents& lc) {
	for (auto& arg : lc.passStack) {
		switch (arg.type) {
			case Language::Type::String: {
				fputs(std::get <std::string>(arg.value).c_str(), stdout);
				break;
			}
			case Language::Type::Integer: {
				printf("%i", std::get <int32_t>(arg.value));
				break;
			}
			case Language::Type::Float: {
				printf("%f", std::get <double>(arg.value));
				break;
			}
			case Language::Type::Bool: {
				printf("%s", std::get <bool>(arg.value)? "true" : "false");
				break;
			}
			case Language::Type::Word: {
				printf("%lli", (long long int) std::get <size_t>(arg.value));
				break;
			}
			default: {
				fputs("[ERR]", stdout);
			}
		}
	}
}

void BuiltIn::Return(Language::LanguageComponents& lc) {
	if (lc.returnStack.empty()) {
		fprintf(stderr, "[ERROR] return: nowhere to return to\n");
		exit(EXIT_FAILURE);
	}
	lc.i = lc.returnStack.back();
	lc.returnStack.pop_back();
}

void BuiltIn::Exit(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		exit(EXIT_SUCCESS);
	}
	Language::Variable exitCode = lc.passStack.back();
	if (exitCode.type != Language::Type::Integer) {
		fprintf(stderr, "[ERROR] Can't use non-integer as escape code\n");
		exit(EXIT_FAILURE);
	}
	exit(std::get <int32_t>(exitCode.value));
}

void BuiltIn::Goto(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] Goto: not enough arguments\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable jumpTo = lc.passStack.back();
	lc.passStack.pop_back();
	if (jumpTo.type != Language::Type::Word) {
		fprintf(
			stderr, "[ERROR] Can't goto to index of type %s\n",
			Language::TypeToString(jumpTo.type).c_str()
		);
		exit(EXIT_FAILURE);
	}

	lc.i = std::get <size_t>(jumpTo.value);
}
