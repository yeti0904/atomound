#include "interpreter.hh"

void Interpret(Language::LanguageComponents& lc) {
	for (; lc.i < lc.tokens.size(); ++ lc.i) {
		auto& token = lc.tokens[lc.i];
		switch (token.type) {
			case Lexer::TokenType::Label:
			case Lexer::TokenType::End: {
				continue;
			}
			case Lexer::TokenType::FunctionCall: {
				while (lc.tokens[lc.i].type != Lexer::TokenType::End) {
					Language::Variable toPush;
					++ lc.i;
					if (lc.tokens[lc.i].type == Lexer::TokenType::End) {
						break;
					}
					switch (lc.tokens[lc.i].type) {
						case Lexer::TokenType::String: {
							toPush.type  = Language::Type::String;
							toPush.value = lc.tokens[lc.i].content;
							break;
						}
						case Lexer::TokenType::Integer: {
							toPush.type  = Language::Type::Integer;
							toPush.value = std::stoi(lc.tokens[lc.i].content);
							break;
						}
						case Lexer::TokenType::Float: {
							toPush.type  = Language::Type::Float;
							toPush.value = std::stod(lc.tokens[lc.i].content);
							break;
						}
						case Lexer::TokenType::Bool: {
							toPush.type  = Language::Type::Bool;
							toPush.value = lc.tokens[lc.i].content == "true";
							break;
						}
						case Lexer::TokenType::Identifier: {
							if (lc.VariableExists(lc.tokens[lc.i].content)) {
								Language::Variable var =
									lc.GetVariable(lc.tokens[lc.i].content);
								toPush = var;
							}
							else if (lc.LabelExists(lc.tokens[lc.i].content)) {
								toPush.type  = Language::Type::Word;
								toPush.value = lc.GetLabel(lc.tokens[lc.i].content);
							}
							else {
								fprintf(
									stderr,
									"[ERROR] Referenced undefined variable/label %s at %i:%i\n",
									lc.tokens[lc.i].content.c_str(),
									(int) lc.tokens[lc.i].line,
									(int) lc.tokens[lc.i].column
								);
								exit(EXIT_FAILURE);
							}
							break;
						}
						default: {
							printf("%i\n", (int) lc.i);
							fprintf(
								stderr, "[ERROR] (2) Unexpected token %s at %i:%i\n",
								Lexer::TypeAsString(token).c_str(),
								(int) lc.tokens[lc.i].line,
								(int) lc.tokens[lc.i].column
							);
							exit(EXIT_FAILURE);
						}
					}

					lc.passStack.push_back(toPush);
				}

				Language::Function function;
				bool               functionExists = false;
				bool               cxxFunction    = false;
				size_t             labelPos       = 0;
				for (auto& func : lc.functions) {
					if (func.name == token.content) {
						function       = func;
						functionExists = true;
						cxxFunction    = true;
					}
				}
				for (size_t i = 0; i < lc.tokens.size(); ++i) {
					if (
						(lc.tokens[i].type    == Lexer::TokenType::Label) &&
						(lc.tokens[i].content == token.content)
					) {
						functionExists = true;
						labelPos       = i;
					}
				}
				if (!functionExists) {
					fprintf(
						stderr,
						"[ERROR] Referenced undefined function %s at %i:%i\n",
						lc.tokens[lc.i].content.c_str(),
						(int) lc.tokens[lc.i].line,
						(int) lc.tokens[lc.i].column
					);
					exit(EXIT_FAILURE);
				}

				if (cxxFunction) {
					function.function(lc);
					lc.passStack = {};
				}
				else {
					lc.returnStack.push_back(lc.i);
					//printf("jumping from %i to %i\n", (int) lc.i, (int) labelPos);
					lc.i = labelPos;
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
							stderr, "[ERROR] Unknown type %s at %i:%i\n",
							lc.tokens[lc.i].content.c_str(),
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
							"Trying to declare variable that already exists at %i:%i\n",
							(int) lc.tokens[lc.i].line,
							(int) lc.tokens[lc.i].column
						);
						exit(EXIT_FAILURE);
					}

					++ lc.i;
					if (lc.tokens[lc.i].type != Lexer::TokenType::Equals) {
						fprintf(
							stderr, "[ERROR] Unexpected token %s at %i:%i\n"
							"    Unitialised variables are not allowed\n",
							Lexer::TypeAsString(lc.tokens[lc.i]).c_str(),
							(int) lc.tokens[lc.i].line, (int) lc.tokens[lc.i].column
						);
						exit(EXIT_FAILURE);
					}

					++ lc.i;
					lc.CreateVariable(newVar.type, newVar.name);
					lc.AssignVariable(newVar.name, lc.tokens[lc.i]);
				}
				break;
			}
			case Lexer::TokenType::Identifier: {
				Language::Variable var = lc.GetVariable(token.content);

				++ lc.i;
				if (lc.tokens[lc.i].type != Lexer::TokenType::Equals) {
					fprintf(
						stderr, "[ERROR] Unexpected token %s at %i:%i\n",
						Lexer::TypeAsString(lc.tokens[lc.i]).c_str(),
						(int) lc.tokens[lc.i].line, (int) lc.tokens[lc.i].column
					);
					exit(EXIT_FAILURE);
				}

				++ lc.i;
				lc.AssignVariable(var.name, lc.tokens[lc.i]);
				break;
			}
			default: {
				fprintf(
					stderr, "[ERROR] (1) Unexpected token %s at %i:%i\n",
					Lexer::TypeAsString(token).c_str(),
					(int) token.line, (int) token.column
				);
				exit(EXIT_FAILURE);
			}
		}
	}
}
