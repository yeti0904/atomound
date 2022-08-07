#include "language.hh"
#include "builtin.hh"
#include "interpreter.hh"

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
	RegisterFunction({"print",         BuiltIn::Print});
	RegisterFunction({"return",        BuiltIn::Return});
	RegisterFunction({"exit",          BuiltIn::Exit});
	RegisterFunction({"goto",          BuiltIn::Goto});
	RegisterFunction({"sleep",         BuiltIn::Sleep});
	RegisterFunction({"add",           BuiltIn::Add});
	RegisterFunction({"sub",           BuiltIn::Sub});
	RegisterFunction({"mul",           BuiltIn::Mul});
	RegisterFunction({"div",           BuiltIn::Div});
	RegisterFunction({"mod",           BuiltIn::Mod});
	RegisterFunction({"include",       BuiltIn::Include});
	RegisterFunction({"goto_if",       BuiltIn::GotoIf});
	RegisterFunction({"is_equal",      BuiltIn::IsEqual});
	RegisterFunction({"get_char",      BuiltIn::GetChar});
	RegisterFunction({"set_char",      BuiltIn::SetChar});
	RegisterFunction({"unpass",        BuiltIn::Unpass});
	RegisterFunction({"char_to_ascii", BuiltIn::CharToAscii});
	RegisterFunction({"str_resize",    BuiltIn::StrResize});
}

void Language::LanguageComponents::Init
(std::vector <Lexer::Token> p_tokens, std::string p_fileName) {
	tokens   = p_tokens;
	i        = 0;
	fileName = p_fileName;

	/*tokens.push_back({
		Lexer::TokenType::FunctionCall,
		"exit",
		0, 0
	});
	tokens.push_back({
		Lexer::TokenType::Integer,
		"0",
		0, 0
	});
	tokens.push_back({
		Lexer::TokenType::End,
		"", 0, 0
	});*/
}

void Language::LanguageComponents::RegisterFunction(Function function) {
	functions.push_back(function);
}

void Language::LanguageComponents::JumpToLabel(std::string name) {
	for (size_t j = 0; j < tokens.size(); ++j) {
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

void Language::LanguageComponents::DeleteVariable(std::string name) {
	for (auto i = variables.begin(); i < variables.end(); ++i) {
		if (i->name == name) {
			variables.erase(i);
			return;
		}
	}
	fprintf(
		stderr, "[ERROR] Tried to remove undefined variable %s\n",
		name.c_str()
	);
	exit(EXIT_FAILURE);
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
	size_t j = i;
	if (name[0] == ':') {
		// sub-label
		while (true) {
			if (
				(tokens[j].type == Lexer::TokenType::Label) &&
				(tokens[j].content[0] != ':')
			) {
				break;
			}
		}
	}
	// first try starting at i
	for (; j < tokens.size(); ++j) {
		if ((tokens[j].type == Lexer::TokenType::Label) && (tokens[j].content == name)) {
			return j;
		}
	}
	
	for (size_t j = 0; j < tokens.size(); ++j) {
		if ((tokens[j].type == Lexer::TokenType::Label) && (tokens[j].content == name)) {
			return j;
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

void Language::LanguageComponents::CallCXXFunction(std::string name) {
	for (auto& func : functions) {
		if (func.name == name) {
			func.function(*this);
			return;
		}
	}
	fprintf(stderr, "[ERROR] Tried to call undefined funtcion %s\n", name.c_str());
	exit(EXIT_FAILURE);
}

bool Language::LanguageComponents::CXXFunctionExists(std::string name) {
	for (auto& func : functions) {
		if (func.name == name) {
			return true;
		}
	}
	return false;
}

void
Language::LanguageComponents::AssignVariable(std::string name) {
	Language::Variable newVar = GetVariable(name);
	auto& token = tokens[i];
	switch (token.type) {
		case Lexer::TokenType::String: {
			if (newVar.type != Language::Type::String) {
				fprintf(
					stderr,
					"[ERROR] Type error at %s:%i:%i: "
					"rvalue doesnt match type of lvalue\n",
					fileName.c_str(),
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
					"[ERROR] Type error at %s:%i:%i: "
					"rvalue doesnt match type of lvalue\n",
					fileName.c_str(),
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
					"[ERROR] Type error at %s:%i:%i: "
					"rvalue doesnt match type of lvalue\n",
					fileName.c_str(),
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
					"[ERROR] Type error at %s:%i:%i: "
					"rvalue doesnt match type of lvalue\n",
					fileName.c_str(),
					(int) token.line,
					(int) token.column
				);
				exit(EXIT_FAILURE);
			}
			newVar.value = token.content == "true";
			break;
		}
		case Lexer::TokenType::FunctionOrIdentifier: {
			if (LabelExists(token.content) || CXXFunctionExists(token.content)) {
				// function
				//if (LabelExists(token.content)) puts("label");
				//if (CXXFunctionExists(token.content)) puts("function");
				FunctionCall();
				if (returnValues.empty()) {
					fprintf(
						stderr,
						"[ERROR] No value to assign at %s:%i:%i (function returned nothing)\n",
						fileName.c_str(),
						(int) token.line,
						(int) token.column
					);
					exit(EXIT_FAILURE);
				}
				Language::Variable ret = returnValues.back();
				returnValues.pop_back();
				if (ret.type != newVar.type) {
					fprintf(
						stderr,
						"[ERROR] Return value doesnt match type of lvalue at %s:%i:%i\n",
						fileName.c_str(),
						(int) token.line,
						(int) token.column
					);
					exit(EXIT_FAILURE);
				}
				newVar      = ret;
				newVar.name = name;
			}
			else { // identifier
				Language::Variable set =
					GetVariable(token.content);
				if (set.type != newVar.type) {
					fprintf(
						stderr,
						"[ERROR] Type error at %s:%i:%i: "
						"rvalue doesnt match type of lvalue\n",
						fileName.c_str(),
						(int) token.line,
						(int) token.column
					);
					exit(EXIT_FAILURE);
				}
				newVar = set;
				newVar.name = name;
			}
			break;
		}
		default: {
			fprintf(
				stderr, "[ERROR] (1) Unexpected token %s at %s:%i:%i\n",
				Lexer::TypeAsString(token).c_str(),
				fileName.c_str(),
				(int) token.line,
				(int) token.column
			);
			exit(EXIT_FAILURE);
		}
	}

	SetVariable(newVar);
}

void Language::LanguageComponents::FunctionCall() {
	auto& token = tokens[i];
	while (tokens[i].type != Lexer::TokenType::End) {
		Language::Variable toPush;
		++ i;
		if (tokens[i].type == Lexer::TokenType::End) {
			break;
		}
		switch (tokens[i].type) {
			case Lexer::TokenType::String: {
				toPush.type  = Language::Type::String;
				toPush.value = tokens[i].content;
				break;
			}
			case Lexer::TokenType::Integer: {
				toPush.type  = Language::Type::Integer;
				toPush.value = std::stoi(tokens[i].content);
				break;
			}
			case Lexer::TokenType::Float: {
				toPush.type  = Language::Type::Float;
				toPush.value = std::stod(tokens[i].content);
				break;
			}
			case Lexer::TokenType::Bool: {
				toPush.type  = Language::Type::Bool;
				toPush.value = tokens[i].content == "true";
				break;
			}
			case Lexer::TokenType::Identifier: {
				if (VariableExists(tokens[i].content)) {
					Language::Variable var =
						GetVariable(tokens[i].content);
					toPush = var;
				}
				else if (LabelExists(tokens[i].content)) {
					toPush.type  = Language::Type::Word;
					toPush.value = GetLabel(tokens[i].content);
				}
				else {
					fprintf(
						stderr,
						"[ERROR] Referenced undefined variable/label %s at %s:%i:%i\n",
						tokens[i].content.c_str(),
						fileName.c_str(),
						(int) tokens[i].line,
						(int) tokens[i].column
					);
					exit(EXIT_FAILURE);
				}
				break;
			}
			default: {
				printf("%i\n", (int) i);
				fprintf(
					stderr, "[ERROR] (2) Unexpected token %s at %s:%i:%i\n",
					Lexer::TypeAsString(token).c_str(),
					fileName.c_str(),
					(int) tokens[i].line,
					(int) tokens[i].column
				);
				exit(EXIT_FAILURE);
			}
		}

		passStack.push_back(toPush);
	}

	bool               functionExists = false;
	bool               cxxFunction    = false;
	if (CXXFunctionExists(token.content)) {
		functionExists = true;
		cxxFunction    = true;
	}
	else if (LabelExists(token.content)) {
		functionExists = true;
	}
	if (!functionExists) {
		fprintf(
			stderr,
			"[ERROR] Referenced undefined function %s at %s:%i:%i\n",
			token.content.c_str(),
			fileName.c_str(),
			(int) tokens[i].line,
			(int) tokens[i].column
		);
		exit(EXIT_FAILURE);
	}

	if (cxxFunction) {
		//puts("cxxFunction");
		CallCXXFunction(token.content);
	}
	else {
		//puts("label function");
		returnStack.push_back(i);
		//printf("jumping from %i to %i\n", (int) i, (int) labelPos);
		JumpToLabel(token.content);
		Interpret(*this, true);
	}
}

void Language::LanguageComponents::CopyNewLC(LanguageComponents& lc) {
	for (auto& var : lc.variables) {
		variables.push_back(var);
	}
	for (auto& function : lc.functions) {
		functions.push_back(function);
	}
	for (auto& token : lc.tokens) {
		tokens.push_back(token);
	}
}
