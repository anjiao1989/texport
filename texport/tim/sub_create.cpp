#include "sub_create.h"
#include "symbol.h"
#include "sub_create.tab.h"

using namespace std;

#define SafeCase(v, p, t, s) \
	do { \
	(v) = (t)(p); assert((v)->SymbolType == s); \
	} while(0)

#define CheckStringCantNull(s, msg) \
	do { \
		if((s) == nullptr) {\
			ParserReportError(msg);\
			AbortParse();\
		}\
	} while(0)

#define SafeDelete(p) \
	do { delete (p); (p) = nullptr; } while(0)

///////////////////////////////////////////////////////////////////////////
// error report
void ReportError(const std::string & str)
{
	cerr << str << endl;
}

void LexicalReportError(const std::string & str)
{
	ReportError(string("lexical: ") + str);
}
void ParserReportError(const std::string & str)
{
	ReportError(string("parser: " + str));
}

void AbortParse()
{
	ReportError("end process");
	abort();
}

static bool s_bDebug = false;
void SetDebug(bool bDebug)
{
	s_bDebug = bDebug;
}
bool IsDebug() 
{
	return s_bDebug;
}

//////////////////////////////////////////////////////////////////////////////////////////
CSymbolTable * GetSymbolTable()
{
	static CSymbolTable s_SymbolTable;
	return &s_SymbolTable;
}
CGrammarTable * GetGrammarTable()
{
	static CGrammarTable s_GrammarTable;
	return &s_GrammarTable;
}

IOutput * g_pOutput = NULL;
void SetOutput(IOutput * pOutput)
{
	g_pOutput = pOutput;
}
IOutput * GetOutput()
{
	return g_pOutput;
}

void yyerror(const char * msg)
{
	extern int yylineno;
	ParserReportError((format("%s. at line: %d") % msg % yylineno).str());
}

//////////////////////////////////////////////////////////////////////////////////////////
CSymbol * ParserFuncState(
	string * type, 
	string * prefix,
	string * name, 
	CSymbol * p_params
	)
{
	CheckStringCantNull(type, "error: define function without type");
	CheckStringCantNull(name, "error: define function without name");

	CSymbolFunction * pSymFunc = 
		(CSymbolFunction *)GetSymbolTable()->Add(*name, SymbolTypeFunction);
	pSymFunc->strType = *type;
	if(prefix != nullptr) {
		pSymFunc->strPrefix = *prefix;
		SafeDelete(prefix);
	}

	SafeDelete(name);
	SafeDelete(type);

	if(p_params != nullptr) {
		CSymbolParams * pSymParams = nullptr;
		SafeCase(pSymParams, p_params, CSymbolParams *, SymbolTypeParams);

		pSymFunc->Params = pSymParams;
	}

	GetGrammarTable()->Add(pSymFunc);
	return pSymFunc;
}
CSymbol * ParserVarDefine(string * type, string * name)
{
	CheckStringCantNull(type, "1");
	CheckStringCantNull(name, "2");

	CSymbolVarible * pSymVar = 
		(CSymbolVarible *)GetSymbolTable()->Add(*name, SymbolTypeVaribleDefine);
	pSymVar->strType = *type;

	SafeDelete(type);
	SafeDelete(name);

	return pSymVar;
}
CSymbol * ParserParamItems(CSymbol * pFirstVar, CSymbol * pOtherParam)
{
	CSymbolParams * pSymParams = (CSymbolParams *)
		GetSymbolTable()->Add("", SymbolTypeParams);;

	if(pFirstVar != nullptr) {
		CSymbolVarible * pSymFirst = nullptr;
		SafeCase(pSymFirst, pFirstVar, CSymbolVarible *, SymbolTypeVaribleDefine);

		pSymParams->FirstParam = pSymFirst;
	}

	if(pOtherParam != nullptr) {
		CSymbolParamOther * pSymOther = nullptr;
		SafeCase(pSymOther, pOtherParam, CSymbolParamOther *, SymbolTypeParamOther);

		pSymParams->OtherParam = pSymOther;
	}

	return pSymParams;
}

CSymbol * ParserParamOther(CSymbol * pVar, CSymbol * pParamsOther)
{
	CSymbolParamOther * pSymOtherParam = nullptr;

	if(pParamsOther == nullptr) {
		pSymOtherParam = (CSymbolParamOther *)
			GetSymbolTable()->Add("", SymbolTypeParamOther);
	} else {
		pSymOtherParam = (CSymbolParamOther *)pParamsOther;
	}

	if(pVar != nullptr) {
		CSymbolVarible * pSymVar = nullptr;
		SafeCase(pSymVar, pVar, CSymbolVarible *, SymbolTypeVaribleDefine);

		pSymOtherParam->arrParams.push_front(pSymVar);
	}

	return pSymOtherParam;
}

void SetTokenProperty(const char * p)
{
	if(IsDebug()) {
		std::cout << "Token: " << p << std::endl;
	}
	yylval.p_string = new std::string(p);
}
