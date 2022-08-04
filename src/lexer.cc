#include "lexer.hh"
#include "language.hh"
#include "util.hh"

std::vector <Lexer::Token> Lexer::Lex(std::string code, std::string fname) {
	size_t                     line   = 0;
	size_t                     column = 1;
	std::vector <Lexer::Token> ret;
	std::string                reading;
	bool                       err    = false;
	bool                       inString = false;
	for (size_t i = 0; i <= code.length(); ++i) {
		if ((i == 0) && (code[i] == '#')) {
			while (code[i] != '\n') {
				++ i;
			}
		}
		if (code[i] == '\n') {
			++ line;
			column = 1;
		}
		else {
			++ column;
		}
		switch (code[i]) {
			case '=': {
				if (inString) {
					reading += code[i];
					break;
				}
				if (!reading.empty()) {
					fprintf(
						stderr, "[ERROR] at %s:%i:%i : no space before = token\n",
						fname.c_str(), (int) line, (int) column
					);
					err = true;
					continue;
				}
				if (ret.empty()) {
					fprintf(
						stderr, "[ERROR] at %s:%i:%i : nothing to assign to\n",
						fname.c_str(), (int) line, (int) column
					);
					err = true;
					continue;
				}
				if (ret.back().type == Lexer::TokenType::FunctionCall) {
					ret.back().type = Lexer::TokenType::Identifier;
				}
				ret.push_back({
					Lexer::TokenType::Equals,
					"",
					line, column
				});
				break;
			}
			case '"': {
				inString =  !inString;
				reading  += code[i];
				break;
			}
			case '/': {
				++ i;
				if (code[i] == '/') {
					while (code[i] != '\n') {
						++ i;
					}
				}
				else if (code[i] == '*') {
					while (true) {
						if ((code[i] == '*') && (code[i + 1] == '/')) {
							break;
						}
					}
					++ i;
					break;
				}
			}
			// fall through
			case '\t':
			case ' ':
			case '\n':
			case '\0': {
				if (inString) {
					reading += code[i];
					break;
				}
				if (reading.empty()) {
					continue;
				}
				if ((ret.size() == 0) || (ret.back().type == Lexer::TokenType::End)) {
					// might be a label
					if (reading[0] == '@') {
						ret.push_back({
							Lexer::TokenType::Label,
							reading.substr(1),
							line, column
						});
						reading = "";
						ret.push_back({
							Lexer::TokenType::End,
							"",
							line, column
						});
						continue;
					}
					// either a function or a keyword
					bool isKeyword = false;
					for (
						size_t i = 0; i < sizeof(Language::keywords) / sizeof(char*);
						++i
					) {
						if (reading == Language::keywords[i]) {
							isKeyword = true;
							break;
						}
					}

					if (isKeyword) {
						ret.push_back({
							Lexer::TokenType::Keyword,
							reading,
							line, column
						});
						reading = "";
					}
					else {
						// function
						ret.push_back({
							Lexer::TokenType::FunctionCall,
							reading,
							line, column
						});
						reading = "";
					}
					if ((code[i] == '\n') || (code[i] == '\0')) {
						ret.push_back({
							Lexer::TokenType::End,
							"",
							line, column
						});
					}
					continue;
				}
				else if (
					(ret.back().type == Lexer::TokenType::Keyword) ||
					(ret.back().content == "let")
				) {
					ret.push_back({
						Lexer::TokenType::Type,
						reading,
						line, column
					});
					reading = "";
				}
				else {
					// argument
					if (Util::IsInteger(reading)) {
						ret.push_back({
							Lexer::TokenType::Integer,
							reading,
							line, column
						});
						reading = "";
					}
					else if (Util::IsFloat(reading)) {
						ret.push_back({
							Lexer::TokenType::Float,
							reading,
							line, column
						});
						reading = "";
					}
					else if (Util::IsBool(reading)) {
						ret.push_back({
							Lexer::TokenType::Bool,
							reading,
							line, column
						});
						reading = "";
					}
					else if ((reading[0] == '"') && (reading.back() == '"')) {
						ret.push_back({
							Lexer::TokenType::String,
							EscapeString(reading.substr(1, reading.length() - 2)),
							line, column
						});
						reading = "";
					}
					else {
						ret.push_back({
							Lexer::TokenType::Identifier,
							reading,
							line, column
						});
						reading = "";
					}
				}

				if ((code[i] == '\n') || (code[i] == '\0')) {
					ret.push_back({
						Lexer::TokenType::End,
						"",
						line, column
					});
				}
				break;
			}
			default: {
				reading += code[i];
				break;
			}
		}
	}

	if (err) {
		return {};
	}
	return ret;
}

std::string Lexer::TypeAsString(Token token) {
	switch (token.type) {
		case Lexer::TokenType::Label:        return "label";
		case Lexer::TokenType::FunctionCall: return "functionCall";
		case Lexer::TokenType::String:       return "string";
		case Lexer::TokenType::Integer:      return "integer";
		case Lexer::TokenType::Float:        return "float";
		case Lexer::TokenType::Identifier:   return "identifier";
		case Lexer::TokenType::Bool:         return "bool";
		case Lexer::TokenType::Keyword:      return "keyword";
		case Lexer::TokenType::Type:         return "type";
		case Lexer::TokenType::Equals:       return "equals";
		case Lexer::TokenType::End:          return "end";
	}
	return "error";
}

void Lexer::Visualise(std::vector <Lexer::Token> tokens) {
	for (size_t i = 0; i < tokens.size(); ++i) {
		printf(
			"%i: %s, %s (%i:%i)\n",
			(int) i, Lexer::TypeAsString(tokens[i]).c_str(), tokens[i].content.c_str(),
			(int) tokens[i].line, (int) tokens[i].column
		);
	}
}

std::string Lexer::EscapeString(std::string str) {
	std::string ret;
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '\\') {
			continue;
		}
		if ((i != 0) && (str[i - 1] == '\\')) {
			switch (str[i]) {
				case 'n': {
					ret += '\n';
					break;
				}
				case 'e': {
					ret += '\x1b';
					break;
				}
				case 't': {
					ret += '\t';
					break;
				}
			}
			continue;
		}
		ret += str[i];
	}
	return ret;
}
