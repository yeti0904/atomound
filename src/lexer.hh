#pragma once
#include "_components.hh"

namespace Lexer {
	enum class TokenType {
		Label = 0,
		FunctionCall,
		String,
		Integer,
		Float,
		Identifier,
		Bool,
		Keyword,
		Type,
		Equals,
		End
	};
	struct Token {
		TokenType   type;
		std::string content;
		size_t      line, column;
	};
	std::vector <Token> Lex(std::string code, std::string fname);
	std::string         TypeAsString(Token token);
	void                Visualise(std::vector <Token> tokens);
	std::string         EscapeString(std::string str);
}
