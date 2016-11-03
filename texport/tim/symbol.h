#ifndef _SYMBOL_
#define _SYMBOL_

#include <tuple>
#include <string>
#include <list>
#include <vector>
#include <fstream>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
class CTranslateTable
{
	typedef vector<std::tuple<string, string, string>> MapType;
public:
	CTranslateTable(const string & strTranslateFile = "");

	void Translate(
		const string & s,
		string & strWide,
		string & strAnsi
		);
	const string Get(const string & s, bool bWide = true);
	
	bool IsNeedTrans(const string & s);

private:
	void _InitStringMap(const string & strTranslateFile);
	void _StringMapInsert(
		const string & strRaw, 
		const string & strWide,
		const string & strAnsi
		);

private:
	MapType m_StringMap;
};

class IOutput
{
public:
	virtual ~IOutput() {}

	virtual bool Open(const std::string & strFile) = 0;
	virtual void Close() = 0;
	virtual void Write(const string & s) = 0;
};

enum SYMPROC_TYPE
{
	SymProcWide,
	SymProcAnsi,
	SymProcMacroWide,
	SymProcMacroAnsi,
	SymProcCallWide,
	SymProcCallAnsi
};
struct SYMBOL_PROCESS_CONTEXT
{
	IOutput * pOutput;
	CTranslateTable * pTable;
	SYMPROC_TYPE Type;
};

//////////////////////////////////////////////////////////////////////////////////////////
enum SYMBOL_TYPE
{
	SymbolTypeUnknown = 0,
	SymbolTypeVaribleDefine,
	SymbolTypeFunction,
	SymbolTypeParams,
	SymbolTypeParamOther,
	SymbolTypeText,
};

class CSymbol
{
public:
	CSymbol() { SymbolType = SymbolTypeUnknown; }
	virtual ~CSymbol() {}

	virtual void OnProcess(SYMBOL_PROCESS_CONTEXT * pContext) 
	{}

	string strName;
	SYMBOL_TYPE SymbolType;
};

class CSymbolVarible : public CSymbol
{
public:
	string strType;

	virtual void OnProcess(SYMBOL_PROCESS_CONTEXT * pContext);

	CSymbolVarible() { SymbolType = SymbolTypeVaribleDefine; }
};

class CSymbolFunction : public CSymbol
{
public:
	string strType;
	string strPrefix;
	class CSymbolParams * Params;

	virtual void OnProcess(SYMBOL_PROCESS_CONTEXT * pContext);

	CSymbolFunction() { SymbolType = SymbolTypeFunction; }
};
class CSymbolParams : public CSymbol
{
public:
	CSymbolVarible * FirstParam;
	class CSymbolParamOther * OtherParam;

	virtual void OnProcess(SYMBOL_PROCESS_CONTEXT * pContext);
	CSymbolParams() { SymbolType = SymbolTypeParams; }
};

class CSymbolParamOther : public CSymbol
{
public:
	std::list<class CSymbolVarible *> arrParams;

	virtual void OnProcess(SYMBOL_PROCESS_CONTEXT * pContext);

	CSymbolParamOther() { SymbolType = SymbolTypeParamOther; }
};

class CSymbolTable
{
public:
	typedef std::list<CSymbol *> value_type;

	value_type arrSymbols;

	CSymbol * GetByName(const string & strName);
	/**
	 * @brief 如果重名，将返回 NULL
	 */
	CSymbol * Add(const string & strName, SYMBOL_TYPE Type);

	void RemoveByName(const string & strName);
	void RemoveByPointer(CSymbol * pSymbol);

	void Clear();

	~CSymbolTable();
};

class CGrammarTable
{
public:
	typedef std::vector<CSymbol *> value_type;

	value_type arrItems;

	void Add(CSymbol * pSymbol)
	{
		arrItems.push_back(pSymbol);
	}
	int GetCount()
	{
		return arrItems.size();
	}
	CSymbol * GetAt(int index)
	{
		if(index >= GetCount())
			return NULL;

		return arrItems[index];
	}
	void Clear()
	{
		arrItems.clear();
	}
};



#endif
