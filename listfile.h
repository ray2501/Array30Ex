#ifndef __LISTFILE__
#define __LISTFILE__

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>

class LISTFILE
{
	public:			
		LISTFILE();
		virtual ~LISTFILE();
		bool readTable();
		bool writeTable();		
		void setFilename(std::string filename);

		std::string m_filename;
		std::map<std::wstring, int> table;
};
#endif

