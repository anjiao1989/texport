#include "Output.h"
#include <windows.h>

//////////////////////////////////////////////////////////////////////////////////////////
// class COutput : public IOutput
bool COutput::Open(const std::string & path) 
{ 
	m_outFD.open(path, std::ios_base::out); 
	return m_outFD.is_open();
}
void COutput::Close() { m_outFD.close(); }

void COutput::Write(const std::string & s)
{
	m_outFD << s;
}

//////////////////////////////////////////////////////////////////////////////////////////
// class CStrCat
static const char * s_PreString = "//@begin ";
static const char * s_PostString = "//@end";

CAppendOutput::CAppendOutput()
{}
CAppendOutput::~CAppendOutput()
{}

bool CAppendOutput::Open(const string & strFile)
{
	m_in.open(strFile);
	m_out.open(strFile + ".tmp");

	if(m_strTag.empty()) {
		m_strTag = strFile;
	}
	m_strFile = strFile;

	// out �ļ��򿪣����ǳɹ�
	bool bResult = m_out.is_open();
	if(bResult) {
		_PreProcess();
	}

	return bResult;
}


void CAppendOutput::Write(const string & s)
{
	m_out << s;
}

void CAppendOutput::_PreProcess()
{
	// ��ʼtag���
	string strTag(s_PreString);
	strTag += m_strTag;

	// û�������ļ���ֻ��� tag ���
	if(!m_in.is_open()) {
		m_out << strTag << endl;
		return;
	}

	// ����ļ���ʼ���ݺ���� tag ���
	string strLine;
	getline(m_in, strLine);
	while(!m_in.fail()) {
		if(strLine.compare(0, strLine.length(), strTag, 0, strTag.length()) == 0)
			break;
		else
			m_out << strLine << endl;

		getline(m_in, strLine);
	}

	m_out << strTag << endl;
}
void CAppendOutput::_PostProcess()
{
	string strTag(s_PostString);

	if(m_in.is_open()) {
		/// ����ת�����ݣ������β tag ��Ǻ��ļ���β����
		string strLine;
		getline(m_in, strLine);
		while(!m_in.fail()) {
			if(strLine.compare(0, strLine.length(), strTag, 0, strTag.length()) == 0)
				break;
			getline(m_in, strLine);
		}
		m_out << strTag << endl;

		if(!m_in.fail())
			goto ext;

		getline(m_in, strLine);
		while(!m_in.fail()) {
			m_out << strLine << endl;
			getline(m_in, strLine);
		}
	} else {
		/// û�������ļ���ֱ�������β tag ���
		m_out << strTag << endl;
	}

ext:
	m_in.close();
	m_out.close();

	RenameFile();
}

void CAppendOutput::Close()
{
	_PostProcess();
}

void CAppendOutput::RenameFile()
{
	CopyFileA((m_strFile + ".tmp").c_str(), m_strFile.c_str(), FALSE);
	DeleteFileA((m_strFile + ".tmp").c_str());
}

void CAppendOutput::SetTag(const std::string & strTag)
{
	m_strTag = strTag;
}
