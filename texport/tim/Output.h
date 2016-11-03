#pragma once
#include "symbol.h"

#include <string>
#include <fstream>

//////////////////////////////////////////////////////////////////////////////////////////
class COutput : public IOutput
{
public:
	bool Open(const std::string & path);
	void Close();

	virtual void Write(const std::string & s);

private:
	std::ofstream m_outFD;
};

//////////////////////////////////////////////////////////////////////////////////////////
class CAppendOutput : public IOutput
{
public:
	CAppendOutput();
	~CAppendOutput();

	virtual bool Open(const std::string & strFile);
	virtual void Write(const std::string & s);
	virtual void Close();

	void SetTag(const std::string & strTag);

private:
	void _PreProcess();
	void _PostProcess();

	void RenameFile();

private:
	std::ifstream m_in;
	std::ofstream m_out;

	std::string m_strFile;
	std::string m_strTag;
};