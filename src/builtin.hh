#pragma once
#include "_components.hh"
#include "language.hh"

namespace BuiltIn {
	void Print(Language::LanguageComponents& lc);
	void Return(Language::LanguageComponents& lc);
	void Exit(Language::LanguageComponents& lc);
	void Goto(Language::LanguageComponents& lc);
}
