#pragma once
#include "_components.hh"
#include "lexer.hh"

namespace Language {
	constexpr const char* keywords[] = {
		"let",
		"del"
	};
	enum class Type {
		String = 0,
		Integer,
		Float,
		Bool,
		Word,
		Err
	};
	Type        StringToType(std::string type);
	std::string TypeToString(Type type);
	typedef std::variant <std::string, int32_t, double, bool, size_t> Value;
	struct Variable {
		std::string name;
		Type        type;
		Value       value;
	};
	class LanguageComponents;
	typedef void (*CXXFunction)(LanguageComponents&);
	struct Function {
		std::string name;
		CXXFunction function;	
	};
	class LanguageComponents {
		public:
			// variables
			std::vector <Variable>     variables;
			std::vector <Variable>     passStack;
			std::vector <Function>     functions;
			std::vector <Lexer::Token> tokens;
			std::vector <size_t>       returnStack;
			std::vector <Variable>     returnValues;
			std::vector <size_t>       argStart;
			size_t                     i;
			std::string                fileName;

			// functions
			LanguageComponents();

			// util functions
			void     Init(std::vector <Lexer::Token> p_tokens, std::string p_fileName);
			void     RegisterFunction(Function function);
			void     JumpToLabel(std::string name);
			Variable GetVariable(std::string name);
			void     SetVariable(Variable variable);
			void     AddVariable(Variable variable);
			void     DeleteVariable(std::string name);
			bool     VariableExists(std::string name);
			bool     LabelExists(std::string name);
			size_t   GetLabel(std::string name);
			void     CreateVariable(Type type, std::string name);
			void     CallCXXFunction(std::string name);
			bool     CXXFunctionExists(std::string name);
			void     AssignVariable(std::string name);
			void     FunctionCall();
			void     CopyNewLC(LanguageComponents& lc);
	};
}
