#include "CINFile.h"
#include <ctype.h>

CINFile::CINFile()
{
	m_filename = "";
}

CINFile::~CINFile()
{
}

bool CINFile::readTable()
{
	std::ifstream file(m_filename.c_str());
	char buffer[256];
	bool readCINFile = false;

	if(!file)
	{
		return false;
	}

	while(!file.eof())
	{
		file.getline(buffer, 256);

		if(buffer[0]=='\n' || strlen(buffer) < 1)
			continue;

		if(buffer[0]=='#')
			continue;

		if(buffer[0]=='%')
		{
			if(!strcmp(buffer, "%chardef begin"))
			{
				readCINFile = true;
				continue;
			}
			else if(!strcmp(buffer, "%chardef end"))
			{
				readCINFile = false;
				break;
			}
			else
			{
				continue;
			}
		}

		if(readCINFile)
		{
			char value1[10];
			char value2[10];
			int number = 0;
			memset(value1, '\0', 10);
			memset(value2, '\0', 10);

			int length = strlen(buffer);
			for(int count = 0; count < length; count++)
			{
				if(buffer[count]==' ' || buffer[count]=='\t')
				{
					char c;
					int i = 0;
					while (buffer[i])
					{
						c = buffer[i];
						buffer[i] = toupper(c);
						i++;
					}

					strncpy(value1, buffer, count);

					number = count;
					break;
				}
			}

			for(int count = number; count < length; count++)
			{
				if(buffer[count]!=' ' && buffer[count]!='\t')
				{
					strncpy(value2, buffer + count, strlen(buffer) - count + 1);
					number = count;
					break;
				}
			}

			DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, value2, -1, NULL, 0);

			wchar_t *pwText; 
			pwText = new wchar_t[dwNum]; 
			if(!pwText) 
			{ 
				delete []pwText; 
			}

			MultiByteToWideChar (CP_UTF8, 0, value2, -1, pwText, dwNum);
			m_table.insert(std::make_pair(std::string(value1), std::wstring(pwText)));
			delete []pwText;
		}
	}

	return true;
}

void CINFile::setFilename(std::string filename)
{
	m_table.clear(); //Clear origin table
	m_filename = filename;
}

