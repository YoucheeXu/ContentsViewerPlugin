//************************* begin of -- include ******************************
#include <cstdlib>
#include <iostream>
#include "IniFile.h"
//************************* **end of -- include ******************************

using namespace std;

CIniFile::CIniFile() {
	m_pFile = nullptr;
	m_pChanged = false;
	m_bDebug = false;
}

CIniFile::CIniFile(const wchar_t* wszFile)
{
	m_pFile = nullptr;
	m_pChanged = false;

	m_wszfile = const_cast<wchar_t*>(wszFile);
	switch(OpenFile(m_wszfile))
	{
		case 0:
			break;
		case 1:
			//LOGINFO(L"fail to open %s file"), tszfile);
			break;
		case 2:
			//LOGINFO(L"fail to read %s file to buffer"), tszfile);
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
 * return: 0  -- file successfully opened
 *	-1  -- fail to open ini file
 *	-2  -- fail to read file to buffer
 *	-3	-- fail to close last file
*******************************************************************************/
int CIniFile::OpenFile(const wchar_t* m_wszfile)
{
	int ret = 0;
	
	if(m_pFile != nullptr) {
		
		ret = CloseFile();
		
		if(ret != 0) return -3;
	}

	// open file
	//m_pFile = _tfopen(tszfile, L"r+"));
	// _tfopen_s(&m_pFile, tszfile, L"a+"));
	// _tfopen_s(&m_pFile, tszfile, L"r"));
	m_pFile = _wfopen(m_wszfile, L"a+, ccs=UNICODE");
	assert(m_pFile);

	wchar_t wszLine[256];

	wchar_t *wszKey, *wszValue;
	
	wstring wSection;
	//std::basic_string<wchar_t> tSection;

	//while(!fin.eof())
	while(fgetws(wszLine, 256, m_pFile))
	{
		//LOGINFO(L"Line£º%s"), tszLine);

		// _tcscpy(tszLine, _tcstrim(tszLine));	//trim bank in head and tail
		wcscpy(wszLine, _wcstrimAll(wszLine));	//trim bank in head and tail, middle
		//if(_tcscmp(tszLine[0], L'[')) == 0)
		//PrintDebugString(L"Line = %s\n", wszLine);

		if(wszLine[0] == L'[')
		{
			//tszLine = Mid(tszLine, 1, _tcslen(tszLine)-2);
			wSection = wszLine;
			//tSection = tSection.substr(1, _tcslen(tszLine) - 3);
			wSection = wSection.substr(1, wcslen(wszLine) - 2);
			//map<wstring, wstring> mapSection;
			//m_mapData[tSection] = mapSection;
			//LOGINFO(L"wszSection: %s"), tSection.c_str());
		}
		//else if((_tcscmp(tszLine[0], L'#')) != 0) || (_tcscmp(tszLine[0], L';')) != 0))
		else if (wszLine[0] != L'#' && wszLine[0] != L';' && wszLine[0] != L'\n')
		{
			wszKey = _wcstok(wszLine, L"=");
			wszValue = _wcstok(NULL, L"=");

			if (NULL == wszValue) wszValue = L"";
			if (NULL == wszKey) continue;
			// _tcscpy(tszKey, _tcstrim(tszKey));
			wcscpy(wszKey, _wcstrimAll(wszKey));
			// _tcscpy(tszValue, _tcstrim(tszValue));
			wcscpy(wszValue, _wcstrimAll(wszValue));
			m_mapData[wSection][wszKey] = wszValue;
			PrintDebugString(L"wszKey: %s,\t wszValue: %s\n", wszKey, wszValue);
		}
	}
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
	if(m_pFile != nullptr) {

		// save file if buffer changed
		if (m_pChanged) {
			// close file
			fclose(m_pFile);
			m_pFile = nullptr;
			m_pFile = _wfopen(m_wszfile, L"w+");

			//LOGINFO(L"\nCloseFile -- wrtie file"));
			//_tfwrite(m_tszFilebuffer, m_lFilesize, 1, m_pFile);
			// map<wstring, map<wstring, wstring>>::iterator multitr;
			// map<wstring, wstring>::iterator intertr;
			for (auto multitr = m_mapData.begin(); multitr != m_mapData.end(); multitr++)
			{
				//std::wcout << L"[") << multitr->first << L"]") <<endl;
				fputws(L"[", m_pFile);
				fputws(multitr->first.c_str(), m_pFile);
				fputws(L"]\n", m_pFile);
				for (auto intertr = multitr->second.begin(); intertr != multitr->second.end(); intertr++)
				{
					//std::wcout << intertr->first << " = " << intertr->second << endl;
					fputws(intertr->first.c_str(), m_pFile);
					fputws(L" = ", m_pFile);
					fputws(intertr->second.c_str(), m_pFile);
					fputws(L"\n", m_pFile);
				}
					
			}
		}
		
		// close file
		if(fclose(m_pFile) != -1)
		{
			m_pFile = nullptr;
		}
		else {
			return -1;
		}
		
		//clear map
		// map<wstring, map<wstring, wstring>>::iterator multitr;
		//map<wstring, wstring>::iterator intertr;
		for (auto multitr = m_mapData.begin(); multitr != m_mapData.end(); multitr++) {
			multitr->second.clear();
		}
		
		m_mapData.clear();		
	}

	return 0;
}

/*******************************************************************************
 *  desc: get a string value by  key and section
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   char *  value  -- key value
 *------------------------------------------------------------------------------
 * return: true  -- key value found
 *   false -- key value not found
*******************************************************************************/
bool CIniFile::GetString(const wchar_t* wszSection, const wchar_t* wszKey, wchar_t* wszValue)
{
	// LOGFUNBGN;
	//keymap map = m_mapData[wszSection];
	//_tcscpy(value, map[wszKey].c_str());
	//m_mapData[wszSection].find(wszKey)->second
	//_tcscpy(value, map->find(wszKey)->second.c_str());
	//wstring* v = &m_mapData[wszSection][wszKey];
	;
	//wstring v = static_cast<keymap>(m_mapData[wszSection])[wszKey];
	/*map<wstring, wstring> * keymap = &m_mapData[wszSection];
	intertr = m_mapData[wszSection].find(wszKey);
	wstring v;
	//v = m_mapData[wszSection].at(wszKey);
	v = keymap->at(wszKey);
	//_tcscpy(value, m_mapData[wszSection][wszKey].c_str());*/
	//map<wstring, map<wstring, wstring>>::iterator multitr;

	// map<wstring, wstring>::iterator intertr;
	/*for (multitr = m_mapData.begin(); multitr != m_mapData.end(); multitr++)
	{
	std::wcout << "[" << multitr->first << "]" <<endl;
	for (intertr = multitr->second.begin(); intertr != multitr->second.end(); intertr++)
	std::wcout << intertr->first << " = " << intertr->second << endl;
	}*/
	auto intertr = m_mapData[wszSection].find(wszKey);
	//std::wcout << L"start to Get wszSection: ") << (*intertr).first << " = " << (*intertr).second << endl;
	if (intertr != m_mapData[wszSection].end())
	{
		wcscpy(wszValue, (*intertr).second.c_str());

		return true;
	}
	else
	{
		return false;
	}
}

/*******************************************************************************
 *  desc: set a string value by key and section
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   const wchar_t* value  -- key value
 *------------------------------------------------------------------------------
 * return: true  -- wszKey value success to write in buffer
*******************************************************************************/
bool CIniFile::SetString(const wchar_t* wszSection, const wchar_t* wszKey, const wchar_t* wszValue)
{
	//LOGINFO(L"Set wszSection: %s, wszKey: %s, value: %s\n"), wszSection, wszKey, value);
	m_mapData[wszSection][wszKey] = wszValue;
	m_pChanged = true;
	return true;
}

/*******************************************************************************
 *  desc: get a integer value by  key and section
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   int default_value  -- default value
 *------------------------------------------------------------------------------
 * return: wszKey value or default value
*******************************************************************************/
int CIniFile::GetInteger(const wchar_t* wszSection, const wchar_t* wszKey, int default_value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(wszSection, wszKey, wszBuffer))
	{
		return (int)(_wtoi(wszBuffer));
	}
	return default_value;
}

/*******************************************************************************
 *  desc: set a integer value
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   const int  value   -- key value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetInteger(const wchar_t* wszSection, const wchar_t* wszKey, const int value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(wszBuffer, 0, sizeof(wszBuffer));
	_stprintf(wszBuffer, L"%d", value);
	return SetString(wszSection, wszKey, wszBuffer);
}

/*******************************************************************************
 *  desc: get a long value by key and section
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   long   default_value  -- default value
 *------------------------------------------------------------------------------
 * return: wszKey value or default value
*******************************************************************************/
long CIniFile::GetLong(const wchar_t* wszSection, const wchar_t* wszKey, long default_value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(wszSection, wszKey, wszBuffer))
	{
		return (int)(_wtol(wszBuffer));
	}
	return default_value;
}

/*******************************************************************************
 *  desc: set a long value
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   const long  value   -- key value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetLong(const wchar_t* wszSection, const wchar_t* wszKey, const long value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(wszBuffer, 0, sizeof(wszBuffer));
	_swprintf(wszBuffer, L"%d", value);
	return SetString(wszSection, wszKey, wszBuffer);
}

/*******************************************************************************
 *  desc: get a double value by key and section
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   double  default_value  -- default value
 *------------------------------------------------------------------------------
 * return: wszKey value or default value
*******************************************************************************/
double CIniFile::GetDouble(const wchar_t* wszSection, const wchar_t* wszKey, double default_value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(wszSection, wszKey, wszBuffer))
	{
		return (int)(_wtof(wszBuffer));
	}
	return default_value;
}

/*******************************************************************************
 *  desc: set a double value
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- wszSection name
 *   const wchar_t* wszKey   -- wszKey name
 *   const double value   -- wszKey value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetDouble(const wchar_t* wszSection, const wchar_t* wszKey, const double value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(wszBuffer, 0, sizeof(wszBuffer));
	_swprintf(wszBuffer, L"%g", value);
	return SetString(wszSection, wszKey, wszBuffer);
}

/*******************************************************************************
 *  desc: get a boolean value by key and section
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   bool   b_default  -- default value
 *------------------------------------------------------------------------------
 * return: key value or default value
*******************************************************************************/
bool CIniFile::GetBool(const wchar_t* wszSection, const wchar_t* wszKey, bool default_value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	if(GetString(wszSection, wszKey, wszBuffer))
	{
		if(wcscmp(wszBuffer, L"y") == 0 ||
			wcscmp(wszBuffer, L"yes") == 0 ||
			wcscmp(wszBuffer, L"true") == 0 )
			return true;
		if(wcscmp(wszBuffer, L"n") == 0 ||
			wcscmp(wszBuffer, L"no") == 0  ||
			wcscmp(wszBuffer, L"false") == 0 )
			return false;
	}
	return default_value;
}

/*******************************************************************************
 *  desc: set a boolean value
 *------------------------------------------------------------------------------
 * param: const wchar_t* wszSection -- section name
 *   const wchar_t* wszKey   -- key name
 *   const bool value   -- key value
 *------------------------------------------------------------------------------
 * return: true
*******************************************************************************/
bool CIniFile::SetBool(const wchar_t* wszSection, const wchar_t* wszKey, const bool value)
{
	wchar_t wszBuffer[M_MAX_INTVAL_BUFFER_SIZE];

	memset(wszBuffer, 0, sizeof(wszBuffer));
	if(value)
		_swprintf(wszBuffer, L"%s", L"true");
	else
		_swprintf(wszBuffer, L"%s", L"false");
	return SetString(wszSection, wszKey, wszBuffer);
}