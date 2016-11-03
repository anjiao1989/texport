#pragma once

#include "symbol.h"

#include <memory>
#include <iostream>
#include <fstream>
#include <string>

#include <boost/format.hpp>
using boost::format;

////////////////////////////////////////////////////////////////////////////////////////// 
// error report
void ReportError(const std::string & str);

void LexicalReportError(const std::string & str);
void ParserReportError(const std::string & str);

void AbortParse();

void SetDebug(bool bDebug);
bool IsDebug();

//////////////////////////////////////////////////////////////////////////////////////////
// ������������
CSymbolTable * GetSymbolTable();
CGrammarTable * GetGrammarTable();

void SetOutput(IOutput * pOutput);
IOutput * GetOutput();

//////////////////////////////////////////////////////////////////////////////////////////
// �ʷ��������ص��ӿ�
void SetTokenProperty(const char * p);

//////////////////////////////////////////////////////////////////////////////////////////
// �ʷ��������ӿ�
bool lexical_init(const char * psz_file);
void lexical_close();
int yylex();

//////////////////////////////////////////////////////////////////////////////////////////
// �﷨�������ص��ӿ�
CSymbol * ParserFuncState(
	string * type, 
	string * prefix,
	string * name, 
	CSymbol * p_params
	);
CSymbol * ParserFuncPrefix(string * pStr);
CSymbol * ParserParamItems(CSymbol * pFirstVar, CSymbol * pOtherParam);
CSymbol * ParserVarDefine(string * type, string * name);
CSymbol * ParserParamOther(CSymbol * pVar, CSymbol * pParamsOther);

/**
 * @brief �����﷨�������������
 */ 
void yyerror(char const *);

//////////////////////////////////////////////////////////////////////////////////////////
// �﷨�������ӿ�
int yyparse (void);
