#ifndef __CINFILE__
#define __CINFILE__

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>


#include <iostream>
#include <fstream>
#include <string>
#include <map>

class CINFile
{
	public:	
		CINFile();
		virtual ~CINFile();
		bool readTable();
		void setFilename(std::string filename);

		std::string m_filename;
		std::multimap<std::string, std::wstring> m_table;
};
#endif

