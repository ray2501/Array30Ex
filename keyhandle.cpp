/******************************************************************************
 * Copyright (c) <2008> <Danilo Raynor>
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * UTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#include "array30.h"
#include "CINFile.h"
#include "listfile.h"

extern HINSTANCE g_hInstance;
extern HWND g_hwnd;
extern HMENU hmenu;
extern HWND hIMMode;
extern HWND hHalfStatus;
extern HWND hInputText;
extern HWND hChoiceText;

extern NOTIFYICONDATA data; //for system tray

extern BOOL isInARMode;
extern BOOL isInFullMode;
extern int  curSize;

extern char HalfWordArray[];
extern WCHAR FullWordArray[];

extern LISTFILE listclass;
extern LISTFILE listtitle;

extern int inputMode;
extern int hanConvert;
extern bool autoSwitchMode;

extern std::wstring gdkClipboard;
extern std::wstring showUnicode;

void ClearCllpboard()
{
    HWND hwnd = GetForegroundWindow();

	if(OpenClipboard(hwnd))
	{
		EmptyClipboard();
		CloseClipboard();
	}       
}

void CopyToClipboard(HWND hwnd, std::wstring& mystring)
{
	HGLOBAL handle;
	WCHAR *ucsbuf;

	if(OpenClipboard(hwnd))
	{
		EmptyClipboard();
		int length = mystring.length();

		handle = GlobalAlloc(GMEM_MOVEABLE, (length + 1) * sizeof(WCHAR));

		if (!handle)
		{
				CloseClipboard();
				return;
		}

		ucsbuf = (WCHAR *)malloc(sizeof(WCHAR) * (length + 1));
		ucsbuf = (WCHAR *)GlobalLock(handle);

		memcpy(ucsbuf, mystring.c_str(), sizeof(WCHAR) * (length + 1));
		GlobalUnlock(handle);

		SetClipboardData(CF_UNICODETEXT, handle);
		CloseClipboard();
	}
}


/********************************************************************
 * Use SendInput() to simulate keyboard event
 ********************************************************************/ 
void SendInputKeys(int number, WORD *keys)
{
        INPUT Input;

        for(int i = 0; i < number; i++)
        {
                Input.type = INPUT_KEYBOARD;
                Input.ki.wVk = keys[i];
                Input.ki.wScan = 0;
                Input.ki.time = 0;
                Input.ki.dwExtraInfo = GetMessageExtraInfo();
                Input.ki.dwFlags = 0;
                SendInput(1, &Input, sizeof(Input));
        }

        Sleep(1);

        for(int i = number - 1; i >= 0; i--)
        {
                Input.type = INPUT_KEYBOARD;
                Input.ki.wVk = keys[i];
                Input.ki.wScan = 0;
                Input.ki.time = 0;
                Input.ki.dwExtraInfo = GetMessageExtraInfo();
                Input.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &Input, sizeof(Input));
        }

        Sleep(1);
}


/********************************************************************
 * Send unicode word function
 ********************************************************************/ 
void SendUnicodeWord(std::wstring mystring, bool isWord)
{
	GUITHREADINFO gi;
	DWORD Pid;	
	HWND hwnd = GetForegroundWindow();
	int currentMode = inputMode;
	bool isConsole = false;
	
	WCHAR wszClassBuf[MAX_PATH];
	GetClassName(hwnd, wszClassBuf, MAX_PATH);	
	
	if(!wcscmp(wszClassBuf, _T("ConsoleWindowClass")))
	{
		isConsole = true;
		inputMode = 1;
	}
	else
	{		
		memset(&gi, 0, sizeof(GUITHREADINFO));
		gi.cbSize = sizeof(GUITHREADINFO);
		GetGUIThreadInfo(GetWindowThreadProcessId(hwnd, &Pid), &gi);
	}  
	
	//Traditional<->Simplified Chinese convert
	if(isWord)
	{
		if(hanConvert==2)
		{
			if(mystring.length()==1)  //UTF-16 0x0000-0xffff
			{
					WCHAR word;
					word = mystring[0];

					WCHAR newword = UCS2TradToSimpChinese(word);
					if(newword != 0)
							mystring[0] = newword;
			}
		}
		else if(hanConvert==3)  
		{
			if(mystring.length()==1) //UTF-16 0x0000-0xffff
			{
					WCHAR word;
					word = mystring[0];

					WCHAR newword = UCS2SimpToTradChinese(word);
					if(newword != 0)
							mystring[0] = newword;
			}
		}
		
		showUnicode.clear();
		showUnicode = mystring;
	}

	//If user setup auto switch setting, detect current state
	if(autoSwitchMode==true && isConsole==false)
	{
		WCHAR wszBuf[MAX_PATH], wszBuf2[MAX_PATH];
		GetClassName(gi.hwndFocus, wszBuf, MAX_PATH);
		GetWindowText(hwnd, wszBuf2, MAX_PATH);
		
		if(gi.hwndCaret && (!wcscmp(wszBuf, _T("Edit")) || (!wcsncmp(wszBuf, _T("RICHEDIT"), 8))))
		{
				inputMode = 4;
		}
		
		std::map<std::wstring, int>::iterator pos;
		for(pos = listclass.table.begin(); pos != listclass.table.end(); ++pos)
		{
			if(wcsstr(wszBuf, pos->first.c_str())!= NULL)
			{
				inputMode = pos->second;
				break;
			}
		}
		
		for(pos = listtitle.table.begin(); pos != listtitle.table.end(); ++pos)
		{
			if(wcsstr(wszBuf2, pos->first.c_str()) != NULL)
			{
				inputMode = pos->second;
				break;
			}
		}		
	}		

	if(inputMode==1)
	{
		std::wstring::iterator pos;
		
		if(isConsole)
		{
			for(pos = mystring.begin();pos != mystring.end(); ++pos)
			{               
				WCHAR c = *pos;
				SendMessage(hwnd, WM_IME_CHAR, (WPARAM) c, 0);
			}
		}
		else
		{		
			for(pos = mystring.begin();pos != mystring.end(); ++pos)
			{               
				WCHAR c = *pos;
				SendMessage(gi.hwndFocus, WM_IME_CHAR, (WPARAM) c, 0);
			}
		}		
	}
	else if(inputMode==4)
	{
		SetFocus (gi.hwndFocus);
		SendMessage (gi.hwndFocus, EM_REPLACESEL, 0, (LPARAM) mystring.c_str());
	}
	else
	{
		if(inputMode==2 || inputMode==3)
		{
				CopyToClipboard(hwnd, mystring);
		}

		//We use clipboard to send our Unicode word
		//Send key event - SHIFT +INS or CTRL + V
		WORD mykeys[2];

		if(inputMode==2)
		{
				mykeys[0] = VK_SHIFT;
				mykeys[1] = VK_INSERT;
		}
		else if(inputMode==3)
		{
				mykeys[0] = VK_CONTROL;
				mykeys[1] = 'V';
		}
		else
		{                       
				gdkClipboard.clear();
				gdkClipboard = mystring;

				OpenClipboard(g_hwnd);
				EmptyClipboard();
				SetClipboardData(CF_UNICODETEXT, NULL);
				CloseClipboard();

				mykeys[0] = VK_CONTROL;
				mykeys[1] = 'V';
		}

		SendInputKeys(2, mykeys); 

		//Clear clipboard, for user request
		if(inputMode==2 || inputMode==3)
		{
				Sleep(1);
				ClearCllpboard();
		}
	}
	
	inputMode = currentMode;
}


void HalfEnglish(WPARAM wParam, LPARAM lParam)
{
	GUITHREADINFO gi;
	DWORD Pid;
	HWND hwnd = GetForegroundWindow();
	bool isConsole = false;
	
	curSize = 0;    
	SetWindowText(hInputText, _T(""));
	SetWindowText(hChoiceText,_T(""));	

	LPARAM lparam = lParam | 1UL;

	WCHAR wszClassBuf[MAX_PATH];
	GetClassName(hwnd, wszClassBuf, MAX_PATH);	
	
	if(!wcscmp(wszClassBuf, _T("ConsoleWindowClass")))
	{
		isConsole = true;
	}
	else
	{		
		memset(&gi, 0, sizeof(GUITHREADINFO));
		gi.cbSize = sizeof(GUITHREADINFO);
		GetGUIThreadInfo(GetWindowThreadProcessId(hwnd, &Pid), &gi);
	}
	
	if(wParam >= 0x20 && wParam <= 0x7e)
	{		
		WCHAR wszBuf[MAX_PATH], wszBuf2[MAX_PATH];
		GetClassName(gi.hwndFocus, wszBuf, MAX_PATH);
		GetWindowText(hwnd, wszBuf2, MAX_PATH);

		if(!wcscmp(wszBuf, _T("OperaWindowClass"))) //Special handle for Opera
		{
			std::wstring mystring;
			mystring += wParam;                     

			CopyToClipboard(hwnd, mystring);

			//We use clipboard to send our Unicode word
			//Send key event - SHIFT +INS or CTRL + V
			WORD mykeys[2];

			mykeys[0] = VK_CONTROL;
			mykeys[1] = 'V';

			SendInputKeys(2, mykeys); 

			//Clear clipboard, for user request
			Sleep(1);
			ClearCllpboard();
		}
		else if(wcsstr(wszBuf2, _T("PCMan")) != NULL) //Special handle for PCMAN
		{						
			SendMessage(gi.hwndFocus, WM_IME_CHAR, wParam, lParam);
		}
		else if(!wcsncmp(wszBuf, _T("gdkWindow"), 9)) //Special handle for Gtk+ application
		{
			gdkClipboard.clear();                   
			gdkClipboard += wParam;

			OpenClipboard(g_hwnd);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, NULL);
			CloseClipboard();

			WORD mykeys[2];
			mykeys[0] = VK_CONTROL;
			mykeys[1] = 'V';
			
			SendInputKeys(2, mykeys); 
		}
		else
		{
			if(isConsole)
				SendMessage(hwnd, WM_CHAR, wParam, lparam);
			else
				SendMessage(gi.hwndFocus, WM_CHAR, wParam, lparam);
		}
	}
	else
	{
		if(wParam==VK_ESCAPE)
		{
			WORD mykeys[1];
			mykeys[0] = VK_ESCAPE;
			SendInputKeys(1, mykeys);
		}
		else if(wParam==VK_BACK)
		{
			WORD mykeys[1];
			mykeys[0] = VK_BACK;
			SendInputKeys(1, mykeys);
		}
	}
}

void FullEnglish(WPARAM wParam, LPARAM lParam)
{
	curSize = 0;
	SetWindowText(hInputText, _T(""));
	SetWindowText(hChoiceText,_T(""));

	if(wParam >= 0x20 && wParam <= 0x7e)
	{
		int i;
		for(i = 0; i < 95; i++)
		{
			if(wParam== (WPARAM) HalfWordArray[i])
			{
					break;
			}       
		}

		std::wstring buffer;
		buffer+=FullWordArray[i];
		SendUnicodeWord(buffer, true);
	}
	else if(wParam==VK_ESCAPE)
	{
		WORD mykeys[1];
		mykeys[0] = VK_ESCAPE;
		SendInputKeys(1, mykeys);
	}
	else if(wParam==VK_BACK)
	{
		WORD mykeys[1];
		mykeys[0] = VK_BACK;
		SendInputKeys(1, mykeys);
	}
}


/********************************************************************
 * Main procecure for key handle
 ********************************************************************/ 
void keyHandle(HWND hwnd, WPARAM wParam, LPARAM lParam)
{		
        if(isInARMode==FALSE && isInFullMode==FALSE)
        {
                HalfEnglish(wParam, lParam);
        }
        else if(isInARMode==FALSE && isInFullMode==TRUE)
        {
                FullEnglish(wParam, lParam);
        }
        else if(isInARMode==TRUE)
        {
                HandleArray(wParam, lParam);
        }
}
