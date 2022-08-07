#include "repl.hh"
#include "language.hh"
#include "interpreter.hh"

void Repl() {
	std::string                  input;
	Language::LanguageComponents lc;
	lc.Init({}, "REPL");
	while (true) {
		fputs("> ", stdout);
		std::getline(std::cin, input, '\n');
		if (input.empty()) {
			continue;
		}
		std::vector <Lexer::Token> tokens = Lexer::Lex(input, "REPL");
		lc.i = lc.tokens.size();
		for (auto& token : tokens) {
			lc.tokens.push_back(token);
		}

		Interpret(lc, false);

		if (!lc.returnValues.empty()) {
			Language::Variable ret = lc.returnValues.back();
			lc.returnValues.pop_back();

			switch (ret.type) {
				case Language::Type::String: {
					puts(std::get <std::string>(ret.value).c_str());
					break;
				}
				case Language::Type::Integer: {
					printf("%i\n", std::get <int32_t>(ret.value));
					break;
				}
				case Language::Type::Float: {
					printf("%g\n", std::get <double>(ret.value));
					break;
				}
				case Language::Type::Bool: {
					puts(std::get <bool>(ret.value)? "true" : "false");
					break;
				}
				case Language::Type::Word: {
					printf("%lld\n", (long long int) std::get <size_t>(ret.value));
					break;
				}
				default: break;
			}
		}
	}
}
