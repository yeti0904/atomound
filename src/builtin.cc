#include "builtin.hh"
#include "lexer.hh"
#include "fs.hh"
#include "util.hh"
#include "interpreter.hh"

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
				printf("%g", std::get <double>(arg.value));
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
	lc.passStack = {};
}

void BuiltIn::Return(Language::LanguageComponents& lc) {
	if (lc.returnStack.empty()) {
		fprintf(stderr, "[ERROR] return: nowhere to return to\n");
		exit(EXIT_FAILURE);
	}
	lc.i = lc.returnStack.back();
	lc.returnStack.pop_back();

	if (!lc.passStack.empty()) {
		lc.returnValues.push_back(lc.passStack.back());
		lc.passStack.pop_back();
	}
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

void BuiltIn::Sleep(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] Sleep: not enough arguments\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable sleepTime = lc.passStack.back();
	lc.passStack.pop_back();

	switch (sleepTime.type) {
		case Language::Type::Integer: {
			int32_t sleep = std::get <int32_t>(sleepTime.value);
			std::this_thread::sleep_for(std::chrono::seconds(sleep));
			break;
		}
		case Language::Type::Float: {
			double sleep = std::get <double>(sleepTime.value);
			size_t time = (size_t) round(sleep * 1000000000);

			std::this_thread::sleep_for(std::chrono::nanoseconds(time));
			break;
		}
		default: {
			fprintf(
				stderr,"[ERROR] Invalid sleep time type %s\n",
				Language::TypeToString(sleepTime.type).c_str()
			);
			exit(EXIT_FAILURE);
		}
	}
}

void Operation(std::string name, Language::LanguageComponents& lc) {
	Language::Variable second = lc.passStack.back();
	lc.passStack.pop_back();
	Language::Variable first = lc.passStack.back();
	lc.passStack.pop_back();

	if (first.type != second.type) {
		fprintf(stderr, "[ERROR] %s: parameters not of the same type\n", name.c_str());
		exit(EXIT_FAILURE);
	}
	if (
		(first.type != Language::Type::Integer) &&
		(first.type != Language::Type::Word) &&
		(first.type != Language::Type::Float)
	) {
		fprintf(stderr, "[ERROR] %s: unsupported type in parameters\n", name.c_str());
		exit(EXIT_FAILURE);
	}

	Language::Variable result;
	result.type = first.type;

	switch (result.type) {
		case Language::Type::Float: {
			double firstNum  = std::get <double>(first.value);
			double secondNum = std::get <double>(second.value);
			if (name == "Add") {
				result.value = (double) (firstNum + secondNum);
			}
			else if (name == "Sub") {
				result.value = (double) (firstNum - secondNum);
			}
			else if (name == "Mul") {
				result.value = (double) (firstNum * secondNum);
			}
			else if (name == "Div") {
				result.value = (double) (firstNum / secondNum);
			}
			else if (name == "Mod") {
				result.value = (double) (fmod(firstNum, secondNum));
			}
			break;
		}
		case Language::Type::Integer: {
			int32_t firstNum  = std::get <int32_t>(first.value);
			int32_t secondNum = std::get <int32_t>(second.value);
			if (name == "Add") {
				result.value = (int32_t) (firstNum + secondNum);
			}
			else if (name == "Sub") {
				result.value = (int32_t) (firstNum - secondNum);
			}
			else if (name == "Mul") {
				result.value = (int32_t) (firstNum * secondNum);
			}
			else if (name == "Div") {
				result.value = (int32_t) (firstNum / secondNum);
			}
			else if (name == "Mod") {
				result.value = (int32_t) (firstNum % secondNum);
			}
			break;
		}
		default: break;
	}

	lc.returnValues.push_back(result);
}

void BuiltIn::Add(Language::LanguageComponents& lc) {
	Operation("Add", lc);
}

void BuiltIn::Sub(Language::LanguageComponents& lc) {
	Operation("Sub", lc);
}

void BuiltIn::Mul(Language::LanguageComponents& lc) {
	Operation("Mul", lc);
}

void BuiltIn::Div(Language::LanguageComponents& lc) {
	Operation("Div", lc);
}

void BuiltIn::Mod(Language::LanguageComponents& lc) {
	Operation("Mod", lc);
}

void BuiltIn::Include(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] Include: no file given to include\n");
		exit(EXIT_FAILURE);
	}
	Language::Variable toInclude = lc.passStack.back();
	lc.passStack.pop_back();
	if (toInclude.type != Language::Type::String) {
		fprintf(stderr, "[ERROR] Include: a string must be passed to include\n");
		exit(EXIT_FAILURE);
	}

	std::string fileName = std::get <std::string>(toInclude.value);
	fileName = Util::DirName(lc.fileName) + "/" + fileName;

	std::string code = FS::File::Read(fileName);
	std::vector <Lexer::Token> tokens = Lexer::Lex(code, fileName);

	Language::LanguageComponents newLc;
	newLc.Init(tokens, fileName);

	Interpret(newLc, false);
	lc.CopyNewLC(newLc);
}

void BuiltIn::GotoIf(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] GotoIf: expected 1 argument of type identifier\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable jumpTo = lc.passStack.back();
	lc.passStack.pop_back();
	if (jumpTo.type != Language::Type::Word) {
		fprintf(
			stderr, "[ERROR] GotoIf: Expected argument of type word, got %s\n",
			Language::TypeToString(jumpTo.type).c_str()
		);
		exit(EXIT_FAILURE);
	}

	if (lc.returnValues.empty()) {
		fprintf(
			stderr, "[ERROR] GotoIf: no return value to compare\n"
		);
		exit(EXIT_FAILURE);
	}

	Language::Variable boolean = lc.returnValues.back();
	bool run = false;
	switch (boolean.type) {
		case Language::Type::Integer: {
			run = std::get <int32_t>(boolean.value) != 0;
			break;
		}
		case Language::Type::Bool: {
			run = std::get <bool>(boolean.value);
			break;
		}
		default: {
			fprintf(
				stderr, "[ERROR] GotoIf: Cannot jump based on value of type %s\n",
				Language::TypeToString(boolean.type).c_str()
			);
			exit(EXIT_FAILURE);
		}
	}

	if (run) {
		lc.i = std::get <size_t>(jumpTo.value);
	}
}

void BuiltIn::IsEqual(Language::LanguageComponents& lc) {
	Language::Variable second = lc.passStack.back();
	lc.passStack.pop_back();
	Language::Variable first = lc.passStack.back();
	lc.passStack.pop_back();

	if (first.type != second.type) {
		fprintf(stderr, "[ERROR] IsEqual: parameters not of the same type\n");
		exit(EXIT_FAILURE);
	}
	if (
		(first.type != Language::Type::Integer) &&
		(first.type != Language::Type::Word) &&
		(first.type != Language::Type::Float) &&
		(first.type != Language::Type::String)
	) {
		fprintf(stderr, "[ERROR] IsEqual: unsupported type in parameters\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable ret;
	ret.type = Language::Type::Bool;

	switch (first.type) {
		case Language::Type::Integer: {
			ret.value =
				std::get <int32_t>(first.value) == std::get <int32_t>(second.value);
			break;
		}
		case Language::Type::Word: {
			ret.value =
				std::get <size_t>(first.value) == std::get <size_t>(second.value);
			break;
		}
		case Language::Type::Float: {
			ret.value =
				std::get <int32_t>(first.value) == std::get <int32_t>(second.value);
			break;
		}
		case Language::Type::String: {
			ret.value =
				std::get <std::string>(first.value) ==
				std::get <std::string>(second.value);
		}
		default: break;
	}

	lc.returnValues.push_back(ret);
}

void BuiltIn::GetChar(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] GetChar: Expected 2 arguments of type string, integer\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable index = lc.passStack.back();
	lc.passStack.pop_back();
	if (
		(index.type != Language::Type::Word) &&
		(index.type != Language::Type::Integer)
	) {
		fprintf(
			stderr, "[ERROR] GetChar: Expected Word as 2nd argument, got %s\n",
			Language::TypeToString(index.type).c_str()
		);
		exit(EXIT_FAILURE);
	}

	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] GetChar: Expected 2 arguments of type string, integer\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable str = lc.passStack.back();
	lc.passStack.pop_back();

	if (str.type != Language::Type::String) {
		fprintf(
			stderr, "[ERROR] Expected String as first argument, got %s\n",
			Language::TypeToString(str.type).c_str()
		);
		exit(EXIT_FAILURE);
	}

	size_t indexValue = 0;
	switch (index.type) {
		case Language::Type::Integer: {
			indexValue = (size_t) std::get <int32_t>(index.value);
			break;
		}
		case Language::Type::Word: {
			indexValue = std::get <size_t>(index.value);
			break;
		}
		default: break;
	}

	Language::Variable ret;
	ret.type = Language::Type::String;
	ret.value = std::string(1, std::get <std::string>(str.value)[indexValue]);

	lc.returnValues.push_back(ret);
}

void BuiltIn::Unpass(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] Unpass: Nothing to pop out of pass stack\n");
		exit(EXIT_FAILURE);
	}

	lc.returnValues.push_back(lc.passStack.back());
	lc.passStack.pop_back();
}

void BuiltIn::CharToAscii(Language::LanguageComponents& lc) {
	if (lc.passStack.empty()) {
		fprintf(stderr, "[ERROR] AsciiToChar: Expected 1 argument of type string\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable ch = lc.passStack.back();
	lc.passStack.pop_back();
	if (ch.type != Language::Type::String) {
		fprintf(stderr, "[ERROR] AsciiToChar: Expected 1 argument of type string\n");
		exit(EXIT_FAILURE);
	}

	Language::Variable ret;
	ret.type = Language::Type::Integer;
	ret.value = (int32_t) std::get <std::string>(ch.value)[0];
	lc.returnValues.push_back(ret);
}
