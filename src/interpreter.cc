#include "interpreter.hh"

void Interpret(Language::LanguageComponents& lc, bool exitOnReturn) {
	for (; lc.i < lc.tokens.size(); ++ lc.i) {
		auto& token = lc.tokens[lc.i];
		switch (token.type) {
			case Lexer::TokenType::Label:
			case Lexer::TokenType::End: {
				break;
			}
			case Lexer::TokenType::FunctionCall: {
				bool quit = false;
				if (exitOnReturn && (token.content == "return")) {
					quit = true;
				}
				lc.FunctionCall();
				if (quit) {
					return;
				}
				break;
			}
			case Lexer::TokenType::Keyword: {
				if (token.content == "let") {
					Language::Variable newVar;
					++ lc.i;
					Language::Type type =
						Language::StringToType(lc.tokens[lc.i].content);
					if (type == Language::Type::Err) {
						fprintf(
							stderr, "[ERROR] Unknown type %s at %s:%i:%i\n",
							lc.tokens[lc.i].content.c_str(),
							lc.fileName.c_str(),
							(int) lc.tokens[lc.i].line, (int) lc.tokens[lc.i].column
						);
						exit(EXIT_FAILURE);
					}
					newVar.type = type;

					++ lc.i;
					newVar.name = lc.tokens[lc.i].content;

					if (lc.VariableExists(newVar.name)) {
						fprintf(
							stderr,
							"[ERROR] "
							"Trying to declare variable that already exists at %s:%i:%i\n",
							lc.fileName.c_str(),
							(int) lc.tokens[lc.i].line,
							(int) lc.tokens[lc.i].column
						);
						exit(EXIT_FAILURE);
					}

					++ lc.i;
					if (lc.tokens[lc.i].type != Lexer::TokenType::Equals) {
						fprintf(
							stderr, "[ERROR] (3) Unexpected token %s at %s:%i:%i\n"
							"    Unitialised variables are not allowed\n",
							Lexer::TypeAsString(lc.tokens[lc.i]).c_str(),
							lc.fileName.c_str(),
							(int) lc.tokens[lc.i].line, (int) lc.tokens[lc.i].column
						);
						exit(EXIT_FAILURE);
					}

					++ lc.i;
					lc.CreateVariable(newVar.type, newVar.name);
					lc.AssignVariable(newVar.name);
				}
				else if (token.content == "del") {
					++ lc.i;
					if (lc.tokens[lc.i].type != Lexer::TokenType::Identifier) {
						fprintf(
							stderr, "[ERROR] (3) Unexpected token %s at %s:%i:%i\n",
							Lexer::TypeAsString(lc.tokens[lc.i]).c_str(),
							lc.fileName.c_str(),
							(int) lc.tokens[lc.i].line, (int) lc.tokens[lc.i].column
						);
						exit(EXIT_FAILURE);
					}

					lc.DeleteVariable(lc.tokens[lc.i].content);
				}
				break;
			}
			case Lexer::TokenType::Identifier: {
				Language::Variable var = lc.GetVariable(token.content);

				++ lc.i;
				if (lc.tokens[lc.i].type != Lexer::TokenType::Equals) {
					fprintf(
						stderr, "[ERROR] (4) Unexpected token %s at %s:%i:%i\n",
						Lexer::TypeAsString(lc.tokens[lc.i]).c_str(),
						lc.fileName.c_str(),
						(int) lc.tokens[lc.i].line, (int) lc.tokens[lc.i].column
					);
					exit(EXIT_FAILURE);
				}

				++ lc.i;
				lc.AssignVariable(var.name);
				break;
			}
			default: {
				fprintf(
					stderr, "[ERROR] (5) Unexpected token %s at %s:%i:%i\n",
					Lexer::TypeAsString(token).c_str(),
					lc.fileName.c_str(),
					(int) token.line, (int) token.column
				);
				exit(EXIT_FAILURE);
			}
		}
	}
}
