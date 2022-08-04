#include "language.hh"
#include "builtin.hh"

Language::Type Language::StringToType(std::string type) {
	if (type == "string")  return Language::Type::String;
	if (type == "integer") return Language::Type::Integer;
	if (type == "float")   return Language::Type::Float;
	if (type == "bool")    return Language::Type::Bool;
	if (type == "word")    return Language::Type::Word;
	return Language::Type::Err;
}

std::string Language::TypeToString(Language::Type type) {
	switch (type) {
		case Language::Type::String:  return "string";
		case Language::Type::Integer: return "integer";
		case Language::Type::Float:   return "float";
		case Language::Type::Bool:    return "bool";
		case Language::Type::Word:    return "word";
		default:                      break;
	}
	return "err";
}

Language::LanguageComponents::LanguageComponents() {
	RegisterFunction({"print",  BuiltIn::Print});
	RegisterFunction({"return", BuiltIn::Return});
	RegisterFunction({"exit",   BuiltIn::Exit});
	RegisterFunction({"goto",   BuiltIn::Goto});
}

void Language::LanguageComponents::Init(std::vector <Lexer::Token> p_tokens) {
	tokens = p_tokens;
	i      = 0;
}

void Language::LanguageComponents::RegisterFunction(Function function) {
	functions.push_back(function);
}

void Language::LanguageComponents::JumpToLabel(std::string name) {
	for (size_t j = 0; i < tokens.size(); ++j) {
		if ((tokens[j].type == Lexer::TokenType::Label) && (tokens[j].content == name)) {
			i = j;
			return;
		}
	}
	fprintf(stderr, "[ERROR] Couldn't jump to label %s\n", name.c_str());
	exit(EXIT_FAILURE);
}

Language::Variable Language::LanguageComponents::GetVariable(std::string name) {
	for (size_t j = 0; j < variables.size(); ++j) {
		if (variables[j].name == name) {
			return variables[j];
		}
	}
	fprintf(stderr, "[ERROR] Tried to access undefined variable %s\n", name.c_str());
	exit(EXIT_FAILURE);
}

void Language::LanguageComponents::SetVariable(Variable variable) {
	for (auto& var : variables) {
		if (var.name == variable.name) {
			var = variable;
			return;
		}
	}
	variables.push_back(variable);
}

void Language::LanguageComponents::AddVariable(Language::Variable variable) {
	variables.push_back(variable);
}

bool Language::LanguageComponents::VariableExists(std::string name) {
	for (auto& var : variables) {
		if (var.name == name) {
			return true;
		}
	}
	return false;
}

bool Language::LanguageComponents::LabelExists(std::string name) {
	for (auto& token : tokens) {
		if ((token.type == Lexer::TokenType::Label) && (token.content == name)) {
			return true;
		}
	}
	return false;
}

size_t Language::LanguageComponents::GetLabel(std::string name) {
	for (size_t i = 0; i < tokens.size(); ++i) {
		if ((tokens[i].type == Lexer::TokenType::Label) && (tokens[i].content == name)) {
			return i;
		}
	}
	fprintf(stderr, "[ERROR] Tried to get non-existent label %s\n", name.c_str());
	exit(EXIT_FAILURE);
}

void Language::LanguageComponents::CreateVariable(Type type, std::string name) {
	Language::Variable newVar;
	newVar.name = name;
	newVar.type = type;
	switch (type) {
		case Language::Type::String: {
			newVar.value = std::string("");
			break;
		}
		case Language::Type::Integer: {
			newVar.value = (int32_t) 0;
			break;
		}
		case Language::Type::Float: {
			newVar.value = (double) 0.0;
			break;
		}
		case Language::Type::Bool: {
			newVar.value = (bool) false;
			break;
		}
		case Language::Type::Word: {
			newVar.value = (size_t) 0;
			break;
		}
		default: {
			break;
		}
	}
	variables.push_back(newVar);
}

void
Language::LanguageComponents::AssignVariable(std::string name, Lexer::Token token) {
	Language::Variable newVar = GetVariable(name);
	switch (token.type) {
		case Lexer::TokenType::String: {
			if (newVar.type != Language::Type::String) {
				fprintf(
					stderr,
					"[ERROR] Type error at %i:%i: "
					"rvalue doesnt match type of lvalue\n",
					(int) token.line,
					(int) token.column
				);
				exit(EXIT_FAILURE);
			}
			newVar.value = token.content;
			break;
		}
		case Lexer::TokenType::Integer: {
			if (
				(newVar.type != Language::Type::Integer) &&
				(newVar.type != Language::Type::Word)
			) {
				fprintf(
					stderr,
					"[ERROR] Type error at %i:%i: "
					"rvalue doesnt match type of lvalue\n",
					(int) token.line,
					(int) token.column
				);
				exit(EXIT_FAILURE);
			}
			switch (newVar.type) {
				case Language::Type::Integer: {
					newVar.value = (int32_t) std::stoi(token.content);
					break;
				}
				case Language::Type::Word: {
					newVar.value = (size_t) std::stol(token.content);
					break;
				}
				default: break;
			}
			break;
		}
		case Lexer::TokenType::Float: {
			if (newVar.type != Language::Type::Float) {
				fprintf(
					stderr,
					"[ERROR] Type error at %i:%i: "
					"rvalue doesnt match type of lvalue\n",
					(int) token.line,
					(int) token.column
				);
				exit(EXIT_FAILURE);
			}
			newVar.value = std::stod(token.content);
			break;
		}
		case Lexer::TokenType::Bool: {
			if (newVar.type != Language::Type::Bool) {
				fprintf(
					stderr,
					"[ERROR] Type error at %i:%i: "
					"rvalue doesnt match type of lvalue\n",
					(int) token.line,
					(int) token.column
				);
				exit(EXIT_FAILURE);
			}
			newVar.value = token.content == "true";
			break;
		}
		case Lexer::TokenType::Identifier: {
			Language::Variable set =
				GetVariable(token.content);
			if (set.type != newVar.type) {
				fprintf(
					stderr,
					"[ERROR] Type error at %i:%i: "
					"rvalue doesnt match type of lvalue\n",
					(int) token.line,
					(int) token.column
				);
				exit(EXIT_FAILURE);
			}
			newVar = set;
			break;
		}
		default: {
			fprintf(
				stderr, "[ERROR] Unexpected token %s at %i:%i\n",
				Lexer::TypeAsString(token).c_str(),
				(int) token.line,
				(int) token.column
			);
			exit(EXIT_FAILURE);
		}
	}

	SetVariable(newVar);
}
