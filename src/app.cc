#include "app.hh"
#include "constants.hh"
#include "fs.hh"
#include "lexer.hh"
#include "util.hh"
#include "language.hh"
#include "interpreter.hh"

App::App(int argc, char** argv) {
	for (int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	std::string programPath = "";
	bool        lexerDebug  = false;
	if (argc > 1) {
		for (size_t i = 1; i < args.size(); ++i) {
			if ((programPath == "") && (args[i][0] == '-')) {
				if ((args[i] == "-h") || (args[i] == "--help")) {
					printf(
						"Usage: %s [options/path]\n"
						"    -h / --help    : show this menu\n"
						"    -v / --version : show version\n"
						"    -d / --debug   : debug lexer tokens\n",
						argv[0]
					);
					return;
				}
				else if ((args[i] == "-v") || (args[i] == "--version")) {
					printf("%s %s\n", APP_TITLE, APP_VERSION);
					return;
				}
				else if ((args[i] == "-d") || (args[i] == "--debug")) {
					lexerDebug = true;
				}
			}
			else {
				programPath = args[i];
				break;
			}
		}
	}

	std::string                code = FS::File::Read(programPath);
	std::vector <Lexer::Token> tokens = Lexer::Lex(code, Util::BaseName(programPath));

	if (lexerDebug) {
		Lexer::Visualise(tokens);
		return;
	}

	if (tokens.empty()) {
		return;
	}

	Language::LanguageComponents lc;
	lc.Init(tokens);
	lc.JumpToLabel("main");

	Interpret(lc);
}
