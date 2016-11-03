// sub_create.cpp : 定义控制台应用程序的入口点。
//
#include "sub_create.h"
#include "symbol.h"
#include "Output.h"

#include <fstream>
#include <tuple>
#include <vector>
#include <assert.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
#define PAUSE()			system("pause")
class CPause
{
public:
	~CPause() { PAUSE(); }
};

string g_strExe;

void DumpUsage() 
{
	std::cerr	<< "usage: " << std::endl
				<< g_strExe 
				<< " <input file> [-a <append file > | -o <output file>]" << std::endl
				<< "        [-t <trans file>]" << std::endl
				<< "        [--debug]" << std::endl
				<< std::endl
				<< "-a: 在指定的文件中符加数据。" << std::endl
				<< "    符加的位置开始于 //@begin <input file>" << std::endl
				<< "    结束于 //@end" << std::endl
				<< "-c: 输出文件名" << std::endl
				<< "-t: 转换规则文件名" << std::endl
				<< "--debug: 开启调试模式，用于调试程序自身的工作" << std::endl;
			
}
const string GetFileName(const string & str)
{
	size_t nBegin1 = str.rfind('\\') + 1;
	size_t nBegin2 = str.rfind('/') + 1;

	size_t nBegin = std::max(nBegin1, nBegin2);
	if(nBegin == str.length())
		return "";

	size_t nEnd = str.rfind('.');
	if(nEnd == 0 || nEnd < nBegin)
		nEnd = str.length();

	return str.substr(nBegin, nEnd - nBegin);
}

bool Work(
	const string & strInFile, 
	const string & strOutFile, 
	bool bAppend,
	const string & strTransPath
	)
{
	CGrammarTable * pGrammar = GetGrammarTable();
	SYMBOL_PROCESS_CONTEXT spc = {0};

	if(bAppend) {
		CAppendOutput * pOutput = new CAppendOutput;
		pOutput->SetTag(GetFileName(strInFile));
		SetOutput(pOutput);
	} else {
		COutput * pOutput = new COutput;
		SetOutput(pOutput);
	}

	spc.pTable = new CTranslateTable(strTransPath);
	spc.pOutput = GetOutput();
	if(!spc.pOutput->Open(strOutFile + ".h"))
		goto ext;

	if(!lexical_init(strInFile.c_str()))
		goto ext;
	yyparse();

	/// 头文件
	// wide
	spc.Type = SymProcWide;
	for(int i = 0; i < pGrammar->GetCount(); i++) {
		CSymbol * pSym = pGrammar->GetAt(i);
		pSym->OnProcess(&spc);		
		spc.pOutput->Write(";\n");
	}

	// ansi
	spc.Type = SymProcAnsi;
	for(int i = 0; i < pGrammar->GetCount(); i++) {
		CSymbol * pSym = pGrammar->GetAt(i);
		pSym->OnProcess(&spc);
		spc.pOutput->Write(";\n");
	}

	spc.pOutput->Write("\n#ifdef _UNICODE\n\n");
	// macro wide
	spc.Type = SymProcMacroWide;
	for(int i = 0; i < pGrammar->GetCount(); i++) {
		CSymbol * pSym = pGrammar->GetAt(i);
		pSym->OnProcess(&spc);
	}

	spc.pOutput->Write("\n#else\n\n");

	// macro ansi
	spc.Type = SymProcMacroAnsi;
	for(int i = 0; i < pGrammar->GetCount(); i++) {
		CSymbol * pSym = pGrammar->GetAt(i);
		pSym->OnProcess(&spc);
	}
	spc.pOutput->Write("\n#endif\n");

	/// 源文件
	spc.pOutput->Close();
	if(!spc.pOutput->Open(strOutFile + ".cpp"))
		goto ext;
	spc.pOutput->Write(
		"#include <atlconv.h>\n\n"
		);

	// wide
	for(int i = 0; i < pGrammar->GetCount(); i++) {
		CSymbol * pSym = pGrammar->GetAt(i);

		spc.Type = SymProcWide;
		pSym->OnProcess(&spc);		
		spc.pOutput->Write(
			"\n"
			"{\n"
			"\tUSES_CONVERSION;\n"
			);
		spc.Type = SymProcCallWide;
		pSym->OnProcess(&spc);
		spc.pOutput->Write(
			"}\n\n"
			);
	}

	// ansi
	for(int i = 0; i < pGrammar->GetCount(); i++) {
		CSymbol * pSym = pGrammar->GetAt(i);

		spc.Type = SymProcAnsi;
		pSym->OnProcess(&spc);		
		spc.pOutput->Write(
			"\n"
			"{\n"
			"\tUSES_CONVERSION;\n"
			);
		spc.Type = SymProcCallAnsi;
		pSym->OnProcess(&spc);
		spc.pOutput->Write(
			"}\n\n"
			);
	}

	spc.pOutput->Close();
	lexical_close();

	delete spc.pTable;
	delete spc.pOutput;
	return true;

ext:
	delete spc.pTable;
	delete spc.pOutput;
	return false;
}

int main(int argc, char ** argv)
{
	std::shared_ptr<CPause> _spPause;

	string strAppendFile;
	string strOutFile;
	string strInFile;
	string strTransFile;

	g_strExe = GetFileName(argv[0]);
	g_strExe += ".exe";

	if(argc == 1) {
		DumpUsage();
		return -1;
	}

	for(int i = 1; i < argc; i++) {
		if(string(argv[i]) == "-o") {
			if(i + 1 >= argc) {
				DumpUsage();
				return -2;
			}

			strOutFile = argv[i + 1];
			i++;
		} else if(string(argv[i]) == "-a") {
			if(i + 1 >= argc) {
				DumpUsage();
				return -11;
			}

			strAppendFile = argv[i + 1];
			i++;
		} else if(string(argv[i]) == "-t") {
			if(i + 1 >= argc) {
				DumpUsage();
				return -10;
			}

			strTransFile = argv[i + 1];
			i++;
		} else if(string(argv[i]) == "--debug") {
			SetDebug(true);
			_spPause.reset(new CPause);
		} else {
			if(!strInFile.empty()) {
				DumpUsage();
				return -3;
			}
			strInFile = argv[i];
		}
	}

	if(strInFile.empty()) {
		DumpUsage();
		return -4;
	}

	if(!strOutFile.empty() && !strAppendFile.empty()) {
		DumpUsage();
		return -12;
	}

	if(strOutFile.empty() && strAppendFile.empty())
		strOutFile = GetFileName(strInFile) + ".tim";
	else
		strOutFile = strAppendFile;
	
	if(!Work(
		strInFile, 
		strOutFile, 
		strAppendFile.empty() ? false : true,
		strTransFile)) 
	{
		DumpUsage();
		return -6;
	}

	return 0;
}
