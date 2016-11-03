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
// 公共工厂函数
CSymbolTable * GetSymbolTable();
CGrammarTable * GetGrammarTable();

void SetOutput(IOutput * pOutput);
IOutput * GetOutput();

//////////////////////////////////////////////////////////////////////////////////////////
// 词法分析器回调接口
void SetTokenProperty(const char * p);

//////////////////////////////////////////////////////////////////////////////////////////
// 词法分析器接口
bool lexical_init(const char * psz_file);
void lexical_close();
int yylex();

//////////////////////////////////////////////////////////////////////////////////////////
// 语法分析器回调接口
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
 * @brief 用于语法分析器报告错误
 */ 
void yyerror(char const *);

//////////////////////////////////////////////////////////////////////////////////////////
// 语法分析器接口
int yyparse (void);
