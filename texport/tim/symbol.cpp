#include "symbol.h"
#include "sub_create.tab.h"

#include <Windows.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

const string GetExePath()
{
	string strPath;

	strPath.resize(MAX_PATH);

	GetModuleFileName(NULL, &strPath[0], MAX_PATH);

	size_t nPos0 = strPath.rfind('/');
	nPos0 = (nPos0 == string::npos ? 0 : nPos0);
	size_t nPos1 = strPath.rfind('\\');
	nPos1 = (nPos1 == string::npos ? 0 : nPos1);
	size_t nPos = max(nPos0, nPos1);

	return strPath.substr(0, nPos);
}

//////////////////////////////////////////////////////////////////////////////////////////
// class CTranslateTable
CTranslateTable::CTranslateTable(const string & strTranslateFile)
{
	if(strTranslateFile.empty()) {
		_InitStringMap(GetExePath() + "\\trans.txt");
	} else {
		_InitStringMap(strTranslateFile); 
	}
}

void CTranslateTable::Translate(
	const string & s,
	string & strWide,
	string & strAnsi
	)
{
	auto ite = find_if(m_StringMap.begin(), m_StringMap.end(), 
		[&s](MapType::value_type & tp)->bool {
			return (get<0>(tp) == s);
	});

	if(ite == m_StringMap.end()) {
		strWide = strAnsi = s;
	} else {
		strWide = get<1>(*ite);
		strAnsi = get<2>(*ite);
	}
}
const string CTranslateTable::Get(const string & s, bool bWide) 
{
	string strWide, strAnsi;
	Translate(s, strWide, strAnsi);
	if(bWide) {
		return strWide;
	} else {
		return strAnsi;
	}
}

bool CTranslateTable::IsNeedTrans(const string & s)
{
	auto ite = find_if(m_StringMap.begin(), m_StringMap.end(), 
		[&s](MapType::value_type & tp)->bool {
			return (get<0>(tp) == s);
	});

	return ite != m_StringMap.end();
}

/**
 * @brief 转换文件的格式：
 * # {这是注释}
 * ; {这是注释}
 * 原始类型,宽字符类型,窄字符类型
 */
void CTranslateTable::_InitStringMap(const string & strTranslateFile)
{
	ifstream TranslateFd(strTranslateFile);
	if(!TranslateFd.is_open())
		return;

	string strLine;
	string s0, s1, s2;
	char_separator<char> CellSep(" \t,");

	for(std::getline(TranslateFd, strLine); 
		!TranslateFd.fail();
		std::getline(TranslateFd, strLine)) 
	{
		if(strLine.empty())
			continue;
		// 排除注释
		if(strLine[0] == ';' || strLine[0] == '#')
			continue;

		tokenizer<char_separator<char>> CellTok(strLine, CellSep);

		auto ite = CellTok.begin();
		if(ite != CellTok.end()) {
			s0 = *ite;
			ite++;
		} else {
			continue;
		}

		if(ite != CellTok.end()) {
			s1 = *ite;
			ite++;
		} else {
			continue;
		}

		if(ite != CellTok.end()) {
			s2 = *ite;
			ite++;
		} else {
			continue;
		}

		_StringMapInsert(s0, s1, s2);
	}
}
void CTranslateTable::_StringMapInsert(
	const string & strRaw, 
	const string & strWide,
	const string & strAnsi
	)
{
	m_StringMap.push_back(make_tuple(strRaw, strWide, strAnsi));
}

//////////////////////////////////////////////////////////////////////////////////////////
void CSymbolVarible::OnProcess(SYMBOL_PROCESS_CONTEXT * pContext) 
{
	switch(pContext->Type) {
	case SymProcWide:
		pContext->pOutput->Write(
			(format("%s %s") % pContext->pTable->Get(strType) % strName).str()
			);
		break;
	case SymProcAnsi:
		pContext->pOutput->Write(
			(format("%s %s") % pContext->pTable->Get(strType, false) % strName).str()
			);
		break;
	case SymProcCallWide:
		if(pContext->pTable->IsNeedTrans(strType)) {
			pContext->pOutput->Write((format("W2T(%s)") % strName.c_str()).str());
		} else {
			pContext->pOutput->Write(strName.c_str());
		}
		break;
	case SymProcCallAnsi:
		if(pContext->pTable->IsNeedTrans(strType)) {
			pContext->pOutput->Write((format("A2T(%s)") % strName.c_str()).str());
		} else {
			pContext->pOutput->Write(strName.c_str());
		}
		break;
	default:
		break;
	}
}

void CSymbolFunction::OnProcess(SYMBOL_PROCESS_CONTEXT * pContext) 
{
	switch(pContext->Type) {
	case SymProcWide:
		if(strPrefix.empty()) {
			pContext->pOutput->Write(
				(format("%s %s") % pContext->pTable->Get(strType) %
				(strName + "W")).str()
				);
		} else {
			pContext->pOutput->Write(
				(format("%s %s %s") % pContext->pTable->Get(strType) %
				strPrefix.c_str() %
				(strName + "W")).str()
				);
		}
		pContext->pOutput->Write("(");
		Params->OnProcess(pContext);
		pContext->pOutput->Write(")");
		break;
	case SymProcAnsi:
		if(strPrefix.empty()) {
			pContext->pOutput->Write(
				(format("%s %s") % pContext->pTable->Get(strType) %
				(strName + "A")).str()
				);
		} else {
			pContext->pOutput->Write(
				(format("%s %s %s") % pContext->pTable->Get(strType) %
				strPrefix.c_str() %
				(strName + "A")).str()
				);
		}

		pContext->pOutput->Write("(");
		Params->OnProcess(pContext);
		pContext->pOutput->Write(")");
		break;
	case SymProcMacroWide:
		pContext->pOutput->Write(
			(format("#define %s\t\t%s\n") % strName % (strName + "W")).str()
			);
		break;
	case SymProcMacroAnsi:
		pContext->pOutput->Write(
			(format("#define %s\t\t%s\n") % strName % (strName + "A")).str()
			);
		break;
	case SymProcCallWide:
	case SymProcCallAnsi:
		pContext->pOutput->Write((format("\treturn %s(") % strName.c_str()).str());
		Params->OnProcess(pContext);
		pContext->pOutput->Write(");\n");
		break;
	default:
		break;
	}
}

void CSymbolParams::OnProcess(SYMBOL_PROCESS_CONTEXT * pContext) 
{
	FirstParam->OnProcess(pContext);
	OtherParam->OnProcess(pContext);
}

void CSymbolParamOther::OnProcess(SYMBOL_PROCESS_CONTEXT * pContext) 
{
	for_each(arrParams.begin(), arrParams.end(),
		[pContext](CSymbolVarible * pVar) {
			pContext->pOutput->Write(", ");
			pVar->OnProcess(pContext);
	});
}

///////////////////////////////////////////////////////////////////////////
// class CSymbolTable
CSymbol * CSymbolTable::GetByName(const string & strName)
{
	value_type::iterator ite = find_if(
		arrSymbols.begin(), arrSymbols.end(), 
		[&strName](CSymbol * pSym)->bool{
			return (pSym->strName == strName);
	});

	if(ite == arrSymbols.end())
		return nullptr;
	else
		return *ite;
}
CSymbol * CSymbolTable::Add(const string & strName, SYMBOL_TYPE Type)
{
	CSymbol * pSym = nullptr;
	switch(Type) {
	case SymbolTypeUnknown:
	case SymbolTypeText:
		pSym = new CSymbol;
		break;

	case SymbolTypeVaribleDefine:
		pSym = new CSymbolVarible;
		break;

	case SymbolTypeFunction:
		pSym = new CSymbolFunction;
		break;

	case SymbolTypeParams:
		pSym = new CSymbolParams;
		break;

	case SymbolTypeParamOther:
		pSym = new CSymbolParamOther;
		break;

	default:
		pSym = nullptr;
		assert(false);
		break;
	}
	if(pSym == nullptr)
		return nullptr;

	pSym->strName = strName;
	arrSymbols.push_back(pSym);
	return pSym;
}

void CSymbolTable::RemoveByName(const string & strName)
{
	CSymbol * pSym = GetByName(strName);
	RemoveByPointer(pSym);
}
void CSymbolTable::RemoveByPointer(CSymbol * pSymbol)
{
	if(pSymbol == nullptr)
		return;

	delete pSymbol;
	arrSymbols.remove(pSymbol);
}

void CSymbolTable::Clear()
{
	for_each(arrSymbols.begin(), arrSymbols.end(),
		[](CSymbol * pSym){
			delete pSym;
	});
	arrSymbols.clear();
}

CSymbolTable::~CSymbolTable()
{
	Clear();
}

///////////////////////////////////////////////////////////////////////////
const std::string symbol_type_to_name(SYMBOL_TYPE t)
{
	switch(t)
	{
	case SymbolTypeUnknown:
		return "SymbolTypeUnknown";

	case SymbolTypeVaribleDefine:
		return "SymbolTypeVaribleDefine";

	case SymbolTypeFunction:
		return "SymbolTypeFunction";

	case SymbolTypeParams:
		return "SymbolTypeParams";

	case SymbolTypeText:
		return "SymbolTypeText";

	default:
		return "nuknown";
	}
}

