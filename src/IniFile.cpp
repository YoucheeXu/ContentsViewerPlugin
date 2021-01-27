//************************* begin of -- include ******************************
#include <stdlib.h>
#include <iostream>
#include "IniFile.h"
//#include "LogFile.h"
//************************* **end of -- include ******************************

using namespace std;

CIniFile::CIniFile() {
	m_pFile = NULL;
	m_pChanged = false;
	m_bDebug = false;
}

CIniFile::CIniFile(const TCHAR* tszfile) {
	m_pFile = NULL;
	m_pChanged = false;

	m_tszfile = const_cast<TCHAR*>(tszfile);
	switch(OpenFile(tszfile)) {
		case 0:
			break;
		case 1:
			//LOGINFO(_T("fail to open %s file"), tszfile);
			break;
		case 2:
			//LOGINFO(_T("fail to read %s file to buffer"), tszfile);
			break;
	}
}

CIniFile::~CIniFile() {
	CloseFile();
}

/*******************************************************************************
 *  desc: open ini file
 *------------------------------------------------------------------------------
 * param: char *tszfile  -- file to open
 *------------------------------------------------------------------------------
 * return: 0  -- file succefully opened
 *	-1  -- fail to open ini file
 *	-2  -- fail to read file to buffer
 *	-3	-- fail to close last file
*******************************************************************************/
int CIniFile::OpenFile(const TCHAR* tszfile)
{
	// LOGFUNBGN;
	
	int ret = 0;
	
	if(m_pFile != NULL) {
		
		ret = CloseFile();
		
		if(ret != 0) return -3;
	}

	//LOGINFO(_T("ready to Open ini File: %s\n"), tszfile);

	// open file
	//m_pFile = _tfopen(tszfile, _T("r+"));
	// _tfopen_s(&m_pFile, tszfile, _T("a+"));
	// _tfopen_s(&m_pFile, tszfile, _T("r"));
	m_pFile = _tfopen(tszfile, _T("a+,ccs=UNICODE"));
	assert(m_pFile);

	TCHAR tszLine[256];

	TCHAR *tszKey, *tszValue;
	
	tString tSection;
	//std::basic_string<TCHAR> tSection;

	//while(!fin.eof())
	while(_fgetts(tszLine, 256, m_pFile))
	{
		//LOGINFO(_T("Line£º%s"), tszLine);

		_tcscpy(tszLine, _tcstrim(tszLine));	//trim bank in head and tail
		//if(_tcscmp(tszLine[0], _T('[')) == 0)
		if(tszLine[0] == _T('['))
		{
			//tszLine = Mid(tszLine, 1, _tcslen(tszLine)-2);
			tSection = tszLine;
			//tSection = tSection.substr(1, _tcslen(tszLine) - 3);
			tSection = tSection.substr(1, _tcslen(tszLine) - 2);
			//map<tString, tString> mapSection;
			//m_mapData[tSection] = mapSection;
			//LOGINFO(_T("Section: %s"), tSection.c_str());
		}
		//else if((_tcscmp(tszLine[0], _T('#')) != 0) || (_tcscmp(tszLine[0], _T(';')) != 0))
		else if (tszLine[0] != _T('#') && tszLine[0] != _T(';') && tszLine[0] != _T('\n'))
		{
			tszKey = _tcstok(tszLine, _T("="));
			tszValue = _tcstok(NULL, _T("="));

			if (tszValue == NULL)	tszValue = _T("");
			_tcscpy(tszKey, _tcstrim(tszKey));
			_tcscpy(tszValue, _tcstrim(tszValue));
			m_mapData[tSection][tszKey] = tszValue;
			//LOGINFO(_T("tszKey: %s,\t tszValue: %s"), tszKey, tszValue);
		}
	}
	// LOGFUNEND;
	return ret;
}

/*******************************************************************************
 *  desc: close ini file
 *------------------------------------------------------------------------------
 * param: none
 *------------------------------------------------------------------------------
 * return: 0  -- file successfully closed
 *   -1  -- fail to close the opened file
*******************************************************************************/
int CIniFile::CloseFile(void)
{
	// LOGFUNBGN;
	// file not opened
	if(m_pFile != NULL) {

		// save file if buffer changed
		if (m_pChanged) {
			// close file
			fclose(m_pFile);
			m_pFile = NULL;
			m_pFile = _tfopen(m_tszfile, _T("w+"));

			//LOGINFO(_T("\nCloseFile -- wrtie file"));
			//_tfwrite(m_tszFilebuffer, m_lFilesize, 1, m_pFile);
			map<tString, map<tString, tString>>::iterator multitr;
			map<tString, tString>::iterator intertr;
			for (multitr = m_mapData.begin(); multitr != m_mapData.end(); multitr++)
			{
				//std::wcout << _T("[") << multitr->first << _T("]") <<endl;
				_fputts(_T("["), m_pFile);
				_fputts(multitr->first.c_str(), m_pFile);
				_fputts(_T("]\n"), m_pFile);
				for (intertr = multitr->second.begin(); intertr != multitr->second.end(); intertr++)
				{
					//std::wcout << intertr->first << " = " << intertr->second << endl;
					_fputts(intertr->first.c_str(), m_pFile);
					_fputts(_T(" = "), m_pFile);
					_fputts(intertr->second.c_str(), m_pFile);
					_fputts(_T("\n"), m_pFile);
				}
					
			}
		}
		
		// close file
		if(fclose(m_pFile) != -1) {
			m_pFile = NULL;
		}
		else {
			return -1;
		}
		
		//clear map
		map<tString, map<tString, tString>>::iterator multitr;
		//map<tString, tString>::iterator intertr;
		for (multitr = m_mapData.begin(); multitr != m_mapData.end(); multitr++) {
			multitr->second.clear();
		}
		
		m_mapData.clear();		
	}

	return 0;
}

/*******************************************************************************
 *  desc: get a string value by key
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   char *  value  -- key value
 *------------------------------------------------------------------------------
 * return: true  -- key value found
 *   false -- key value not found
*******************************************************************************/
bool CIniFile::GetString(const TCHAR* section, const TCHAR* key, TCHAR* value)
{
	// LOGFUNBGN;
	//keymap map = m_mapData[section];
	//_tcscpy(value, map[key].c_str());
	//m_mapData[section].find(key)->second
	//_tcscpy(value, map->find(key)->second.c_str());
	//tString* v = &m_mapData[section][key];
	;
	//tString v = static_cast<keymap>(m_mapData[section])[key];
	/*map<tString, tString> * keymap = &m_mapData[section];
	intertr = m_mapData[section].find(key);
	tString v;
	//v = m_mapData[section].at(key);
	v = keymap->at(key);
	//_tcscpy(value, m_mapData[section][key].c_str());*/
	//map<tString, map<tString, tString>>::iterator multitr;

	map<tString, tString>::iterator intertr;
	/*for (multitr = m_mapData.begin(); multitr != m_mapData.end(); multitr++)
	{
	std::wcout << "[" << multitr->first << "]" <<endl;
	for (intertr = multitr->second.begin(); intertr != multitr->second.end(); intertr++)
	std::wcout << intertr->first << " = " << intertr->second << endl;
	}*/
	intertr = m_mapData[section].find(key);
	//std::wcout << _T("start to Get section: ") << (*intertr).first << " = " << (*intertr).second << endl;
	if (intertr != m_mapData[section].end())
	{
		_tcscpy(value, (*intertr).second.c_str());
		//LOGINFO(_T("Get section: %s, key: %s, value: %s\n"), section, key, value);

		return true;
	}
	else
	{
		//LOGINFO(_T("Can't find: section: %s, key: %s"), section, key);
		return false;
	}
	//LOGFUNEND;
}

/*******************************************************************************
 *  desc: set a string value by key
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   const TCHAR* value  -- key value
 *------------------------------------------------------------------------------
 * return: true  -- key value success to write in buffer
*******************************************************************************/
bool CIniFile::SetString(const TCHAR* section, const TCHAR* key, const TCHAR* value)
{
	//LOGINFO(_T("Set section: %s, key: %s, value: %s\n"), section, key, value);
	m_mapData[section][key] = value;
	m_pChanged = true;
	return true;
}

/*******************************************************************************
 *  desc: get a interger value by key
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   int default_value  -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
int CIniFile::GetInteger(const TCHAR* section, const TCHAR* key, int default_value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(section, key, tszBuffer))
	{
		return (int)(_ttoi(tszBuffer));
	}
	return default_value;
}

/*******************************************************************************
 *  desc: set a interger value
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   const int  value   -- key value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetInteger(const TCHAR* section, const TCHAR* key, const int value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(tszBuffer, 0, sizeof(tszBuffer));
	_stprintf(tszBuffer, _T("%d"), value);
	return SetString(section, key, tszBuffer);
}

/*******************************************************************************
 *  desc: get a long value by key
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   long   default_value  -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
long CIniFile::GetLong(const TCHAR* section, const TCHAR* key, long default_value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(section, key, tszBuffer))
	{
		return (int)(_ttol(tszBuffer));
	}
	return default_value;
}

/*******************************************************************************
 *  desc: set a long value
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   const long  value   -- key value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetLong(const TCHAR* section, const TCHAR* key, const long value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(tszBuffer, 0, sizeof(tszBuffer));
	_stprintf(tszBuffer, _T("%d"), value);
	return SetString(section, key, tszBuffer);
}

/*******************************************************************************
 *  desc: get a double value by key
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   double  default_value  -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
double CIniFile::GetDouble(const TCHAR* section, const TCHAR* key, double default_value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(section, key, tszBuffer))
	{
		return (int)(_ttof(tszBuffer));
	}
	return default_value;
}

/*******************************************************************************
 *  desc: set a double value
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   const double value   -- key value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetDouble(const TCHAR* section, const TCHAR* key, const double value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(tszBuffer, 0, sizeof(tszBuffer));
	_stprintf(tszBuffer, _T("%g"), value);
	return SetString(section, key, tszBuffer);
}

/*******************************************************************************
 *  desc: get a bool value by key
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   bool   b_default  -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
bool CIniFile::GetBool(const TCHAR* section, const TCHAR* key, bool default_value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(section, key, tszBuffer))
	{
		if(_tcscmp(tszBuffer, _T("y")) == 0 ||		
			_tcscmp(tszBuffer, _T("yes")) == 0 ||
			_tcscmp(tszBuffer, _T("true")) == 0 )
			return true;
		if(_tcscmp(tszBuffer, _T("n")) == 0 ||
			_tcscmp(tszBuffer, _T("no")) == 0  ||
			_tcscmp(tszBuffer, _T("false")) == 0 )
			return false;
	}
	return default_value;
}



/*******************************************************************************
 *  desc: set a bool value
 *------------------------------------------------------------------------------
 * param: const TCHAR* section -- section name
 *   const TCHAR* key   -- key name
 *   const bool value   -- key value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetBool(const TCHAR* section, const TCHAR* key, const bool value)
{
	TCHAR tszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(tszBuffer, 0, sizeof(tszBuffer));
	if(value)
		_stprintf(tszBuffer, _T("%s"), _T("true"));
	else
		_stprintf(tszBuffer, _T("%s"), _T("false"));
	return SetString(section, key, tszBuffer);
}