#include "listfile.h"

LISTFILE::LISTFILE()
{
	m_filename = "";
}

LISTFILE::~LISTFILE()
{
}

bool LISTFILE::readTable()
{
	std::ifstream ifile(m_filename.c_str());
	char buffer[256];
	
	if(!ifile)
		return false;

	while(!ifile.eof())
	{
		ifile.getline(buffer, 256);

		if(buffer[0]=='\n' || strlen(buffer) < 1)
			continue;		
			
		char value1[250];
		char value2[2];
		memset(value1, '\0', 250);
		memset(value2, '\0', 2);

		int length = strlen(buffer);
		for(int count = 0; count < length; count++)
		{
			if(buffer[count]==',')
			{
				strncpy(value1, buffer, count);
				strncpy(value2, buffer + count + 1, strlen(buffer) - count - 1);
			}
		}	
		
		DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, value1, -1, NULL, 0);

		wchar_t *pwText; 
		pwText = new wchar_t[dwNum]; 
		if(!pwText) 
		{ 
			delete []pwText; 
		}

		MultiByteToWideChar (CP_UTF8, 0, value1, -1, pwText, dwNum);		
		int mode = atoi(value2);
		
		table.insert(std::make_pair(std::wstring(pwText), mode));
	}
	
	ifile.close();

	return true;
}


bool LISTFILE::writeTable()
{
	std::ofstream ofile(m_filename.c_str());
	char buffer[256];
	
	if(!ofile)
		return false;
		
	table.insert(std::make_pair(std::wstring(L"Opera"), 3));	
	
	std::map<std::wstring, int>::iterator pos;
	for(pos = table.begin(); pos != table.end(); ++pos)
	{	
		memset(buffer, '\0', 256);
		char *lpszStr;
		DWORD dwMinSize; 
		dwMinSize = WideCharToMultiByte(CP_UTF8, 0, pos->first.c_str(), -1, NULL, 0, NULL,FALSE); 	
		
		lpszStr = (char *)malloc(sizeof(char) * (dwMinSize + 1));
		
		WideCharToMultiByte(CP_UTF8, 0, pos->first.c_str(), -1, lpszStr, dwMinSize, NULL, FALSE);
		sprintf(buffer, "%s,%d", lpszStr, pos->second);	
		
		free(lpszStr);

		ofile.write(buffer, strlen(buffer));
		ofile << std::endl;					
	}	
	
	ofile.close();
	
	return true;
}

void LISTFILE::setFilename(std::string filename)
{
	m_filename = filename;
}
