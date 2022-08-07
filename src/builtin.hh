#pragma once
#include "_components.hh"
#include "language.hh"

namespace BuiltIn {
	void Print(Language::LanguageComponents& lc);
	void Return(Language::LanguageComponents& lc);
	void Exit(Language::LanguageComponents& lc);
	void Goto(Language::LanguageComponents& lc);
	void Sleep(Language::LanguageComponents& lc);
	void Add(Language::LanguageComponents& lc);
	void Sub(Language::LanguageComponents& lc);
	void Mul(Language::LanguageComponents& lc);
	void Div(Language::LanguageComponents& lc);
	void Mod(Language::LanguageComponents& lc);
	void Include(Language::LanguageComponents& lc);
	void GotoIf(Language::LanguageComponents& lc);
	void IsEqual(Language::LanguageComponents& lc);
	void GetChar(Language::LanguageComponents& lc);
	void Unpass(Language::LanguageComponents& lc);
	void CharToAscii(Language::LanguageComponents& lc);
	void StrResize(Language::LanguageComponents& lc);
}
