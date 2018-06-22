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

#include <stdio.h>
#include "array30.h"
#include "resource.h"
#include "CINFile.h"
#include "BOXFile.h"
#include "keyhook.h"
#include "listfile.h"

const TCHAR *szAppName = _T("ArrayWndClass");

HINSTANCE g_hInstance;
HMENU hmenu;
HFONT hFont;
HWND g_hwnd;
HWND hIMMode;
HWND hHalfStatus;
HWND hInputText;
HWND hChoiceText;

NOTIFYICONDATA data;
int inputMode = 2; //Send output method setting
int hanConvert = 1; //Traditional<->Simplified Chinese Output setting
bool autoSwitchMode = true;
long int lastPointX = 1;
long int lastPointY = 1;
bool showUnicodeSetting = true;

UINT g_message = WM_USER; // for keyboard Hook
BOOL isInARMode = TRUE;
BOOL isInFullMode = FALSE;
int  curSize = 0;

HANDLE Mutex;

CINFile mainTable;
CINFile shortCode;
BOXFile boxTable;
LISTFILE listclass;
LISTFILE listtitle;

int HotKeyID1 = 1;
int HotKeyID2 = 2;
int HotKeyID3 = 3;
int HotKeyID4 = 4;
int HotKeyID5 = 5;
int HotKeyID6 = 6;
int HotKeyID7 = 7;

std::wstring gdkClipboard;
bool b_isExtBFontExit = true;

char HalfWordArray[] =
{
	' ', '!', '\"', '#', '$', '%', '&', '\'',
	'(', ')', '*', '+', ',', '-', '.', '/',
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', ':', ';', '<', '=', '>', '?',
	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
	'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z', '{', '|', '}', '~'
};

char HalfDvorakArray[] =
{
	' ', '!', '_', '#', '$', '%', '&', '-',
	'(', ')', '*', '}', 'w', '{', 'v', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'S', 's', 'W', ']', 'V', 'Z',
	'@', 'A', 'X', 'J', 'E', '>', 'U', 'I',
	'D', 'C', 'H', 'T', 'N', 'M', 'B', 'R',
	'L', '\"', 'P', 'O', 'Y', 'G', 'K', '<',
	'Q', 'F', ':', '/', '\\', '=', '^', '[',
	'`', 'a', 'x', 'j', 'e', '.', 'u', 'i',
	'd', 'c', 'h', 't', 'n', 'm', 'b', 'r',
	'l', '\'', 'p', 'o', 'y', 'g', 'k', ',',
	'q', 'f', ';', '?', '|', '+', '~'
};

WCHAR FullWordArray[] =
{
	// ¡@      ¡I       ¡¨      ¡­     ¢C      ¢H      ¡®       ¡¦
	0x3000, 0xFF01, 0x201D, 0xFF03, 0xFF04, 0xFF05, 0xFF06, 0x2019, 
	// ¡]       ¡^     ¡¯      ¡Ï      ¡A      ¡Ð      ¡D      ¡þ
	0xFF08, 0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF0F, 
	// ¢¯      ¢°      ¢±     ¢²      ¢³      ¢´      ¢µ      ¢¶
	0xFF10, 0xFF11, 0xFF12, 0xFF13, 0xFF14, 0xFF15, 0xFF16, 0xFF17, 
	// ¢·      ¢¸      ¡G     ¡F      ¡Õ      ¡×      ¡Ö      ¡H
	0xFF18, 0xFF19, 0xFF1A, 0xFF1B, 0xFF1C, 0xFF1D, 0xFF1E, 0xFF1F, 
	// ¢I      ¢Ï      ¢Ð     ¢Ñ      ¢Ò      ¢Ó      ¢Ô      ¢Õ
	0xFF20, 0xFF21, 0xFF22, 0xFF23, 0xFF24, 0xFF25, 0xFF26, 0xFF27, 
	// ¢Ö      ¢×     ¢Ø      ¢Ù      ¢Ú      ¢Û      ¢Ü      ¢Ý
	0xFF28, 0xFF29, 0xFF2A, 0xFF2B, 0xFF2C, 0xFF2D, 0xFF2E, 0xFF2F, 
	// ¢Þ      ¢ß     ¢à      ¢á      ¢â      ¢ã      ¢ä      ¢å
	0xFF30, 0xFF31, 0xFF32, 0xFF33, 0xFF34, 0xFF35, 0xFF36, 0xFF37, 
	// ¢æ      ¢ç     ¢è      ¡e      ¢@      ¡f      ¡s      ¡Å
	0xFF38, 0xFF39, 0xFF3A, 0x3014, 0xFF3C, 0x3015, 0xFF3E, 0xFF3F, 
	// ¡¥       ¢é      ¢ê      ¢ë      ¢ì      ¢í      ¢î      ¢ï
	0x2018, 0xFF41, 0xFF42, 0xFF43, 0xFF44, 0xFF45, 0xFF46, 0xFF47, 
	// ¢ð      ¢ñ      ¢ò      ¢ó      ¢ô      ¢õ      ¢ö      ¢÷
	0xFF48, 0xFF49, 0xFF4A, 0xFF4B, 0xFF4C, 0xFF4D, 0xFF4E, 0xFF4F, 
	// ¢ø      ¢ù      ¢ú      ¢û      ¢ü      ¢ý      ¢þ      £@
	0xFF50, 0xFF51, 0xFF52, 0xFF53, 0xFF54, 0xFF55, 0xFF56, 0xFF57, 
	// £A      £B      £C      ¡a      ¡U      ¡b      ¡ã 
	0xFF58, 0xFF59, 0xFF5A, 0xFF5B, 0xFF5C, 0xFF5D, 0xFF5E
};


/********************************************************************
 * Main procecure
 ********************************************************************/ 
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int);

/* MinGW doesn't support Wide char version startup function wWinMain, so we
 * provide a stub to keep it quiet */
#if !defined(_MSC_VER) && defined(_UNICODE) /* Only for MinGW and defined _UNICODE */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hNull, LPSTR lpCmdLine, int nCmdShow)
{
	return _tWinMain(hInst, NULL, GetCommandLine(), nCmdShow);
}
#endif


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	GetConfig(); //Read config setting

	//µù¥Uµøµ¡Ãþ§O
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, _T("AR30"));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject (COLOR_APPWORKSPACE);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;
	wc.hIconSm = LoadIcon(hInstance, _T("AR30"));

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, _T("Register Window Fail!"), _T("Error!"),
				MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
			WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
			szAppName,
			_T("Array30Ex"),
			WS_POPUP | WS_BORDER,
			lastPointX, lastPointY, 665, 30,
			NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, _T("Create Window Fail!"), _T("Error!"),
				MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	g_hwnd = hwnd;
	g_hInstance = hInstance;
	
	installKeyHook(hwnd, g_message);
	b_isExtBFontExit = isExtBFontExit();

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);	

	//Initial our status
	isInARMode = TRUE;
	isInFullMode = FALSE;

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

/********************************************************************
 * Windows procedure - handle messages
 ********************************************************************/ 
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TCHAR szInfo[64] = _T("Array30Ex");
	UINT fuModifiers; 
	UINT uVirtKey;
	switch(msg)
	{
		POINT point;

		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hwnd, WM_RBUTTONUP, OnRButtonUp);
		HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING, OnWindowPosChanging);
		HANDLE_MSG(hwnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);

		case WM_HOTKEY: //MinGW does not support WM_HOTKEY HANDLE_MSG()
		fuModifiers = (UINT) LOWORD(lParam); 
		uVirtKey = (UINT) HIWORD(lParam);

		if(wParam==1)
		{
			data.hWnd = hwnd;
			data.uID = MENU_HIDE; 
			data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			data.uCallbackMessage = NOTIFY_MESSAGE;
			data.hIcon = LoadIcon(g_hInstance, _T("AR30")); 
			_tcscpy(data.szTip, szInfo);

			ShowWindow(hwnd, SW_HIDE);
			Shell_NotifyIcon(NIM_ADD, &data);

			EnableMenuItem(hmenu, MENU_HIDE, MF_GRAYED | MF_DISABLED);	
			EnableMenuItem(hmenu, MENU_SHOW, MF_ENABLED);
		}
		else if(wParam==2)
		{
			Shell_NotifyIcon(NIM_DELETE, &data);
			ShowWindow(hwnd, SW_SHOWNORMAL);

			hmenu = LoadMenu (g_hInstance, _T("POPMENU"));
			hmenu = GetSubMenu(hmenu, 0);

			EnableMenuItem(hmenu, MENU_HIDE, MF_ENABLED);	
			EnableMenuItem(hmenu, MENU_SHOW, MF_GRAYED | MF_DISABLED);
		}
		else if(wParam==3)
		{
			if(isInARMode) 
				SwitchToEnglish();			
			else
				SwitchToArray();
		}
		else if(wParam==4)
		{
			if(isInFullMode)
				SwitchToHalf();
			else
				SwitchToFull();
		}
		else if(wParam==5)
		{
			UnregisterHotKey(hwnd, HotKeyID5);
			keyHandle(hwnd, uVirtKey, 0);
			RegisterHotKey(hwnd, HotKeyID5, 0, VK_BACK);
		}
		else if(wParam==6)
		{
			UnregisterHotKey(hwnd, HotKeyID6);
			keyHandle(hwnd, uVirtKey, 0);
			RegisterHotKey(hwnd, HotKeyID6, 0, VK_ESCAPE);
		}
		else if(wParam==7)
		{
			if(isInARMode) 
				SwitchToEnglish();		
			else
				SwitchToArray();
		}

		break;

		case WM_USER:				
		keyHandle(hwnd, wParam, lParam);
		break;

		case NOTIFY_MESSAGE:
		// Handle System Tray event
		switch(lParam)
		{
			case WM_RBUTTONDOWN:
				hmenu = LoadMenu (g_hInstance, _T("POPMENU"));
				hmenu = GetSubMenu(hmenu, 0);		 		 	

				EnableMenuItem(hmenu, MENU_HIDE, MF_GRAYED | MF_DISABLED);	
				EnableMenuItem(hmenu, MENU_SHOW, MF_ENABLED);

				GetCursorPos(&point);	// Get the mouse cursor point
				TrackPopupMenu(hmenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
				DestroyMenu(hmenu);

				break;
			case WM_LBUTTONDBLCLK:
				Shell_NotifyIcon(NIM_DELETE, &data);

				EnableMenuItem(hmenu, MENU_HIDE, MF_ENABLED);	
				EnableMenuItem(hmenu, MENU_SHOW, MF_GRAYED | MF_DISABLED);

				ShowWindow(hwnd, SW_SHOWNORMAL);
				SetForegroundWindow(hwnd);

				break;
		}
		
		break;

		//Ok, it is special handle for Gtk+ application
		case WM_RENDERALLFORMATS:
		OpenClipboard(hwnd);
		EmptyClipboard();

		case WM_RENDERFORMAT:
		HGLOBAL handle;
		WCHAR *ucsbuf;

		int length = gdkClipboard.length();

		handle = GlobalAlloc(GMEM_MOVEABLE, (length + 1) * sizeof(WCHAR));

		ucsbuf = (WCHAR *)malloc(sizeof(WCHAR) * (length + 1));
		ucsbuf = (WCHAR *)GlobalLock(handle);

		memcpy(ucsbuf, gdkClipboard.c_str(), sizeof(WCHAR) * (length + 1));
		GlobalUnlock(handle);

		SetClipboardData(CF_UNICODETEXT, handle);

		if(msg==WM_RENDERALLFORMATS)
			CloseClipboard();

		gdkClipboard.clear();

		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/********************************************************************
 * Enum font callback function
 ********************************************************************/ 
static int CALLBACK fontEnumProc(const LOGFONT *aLogFont, const TEXTMETRIC *aMetric,
             DWORD aFontType, LPARAM aClosure)
{
	//Windows Font limit is aobut 65535 word, so Unicode 3.1 need ExtB font
	if(wcsstr(aLogFont->lfFaceName, _T("ExtB"))!=NULL)
	{
		*((int *) aClosure) = 1;
		return 0;
	}
	else
	{
		*((int *) aClosure) = 0;
	}

	return 1;
}


bool isExtBFontExit()
{
  HDC     hdc = ::GetDC(0);
  LOGFONT lf;
  int     isExist = 1;

  memset (&lf, 0, sizeof (lf));
  lf.lfCharSet = DEFAULT_CHARSET;
  lf.lfFaceName[0] = L'\0';

  ::EnumFontFamiliesEx(hdc, &lf, fontEnumProc, (LPARAM) &isExist, 0);
  ::ReleaseDC(0, hdc);

  if (isExist)
	  return true;
  else
	  return false;
}


/*************************************************************************
 * Sets or remove WS_EX_LAYERD attribute for a window.
 *************************************************************************/
void SetWsExLayered(HWND hWnd, BOOL flag)
{
	if (flag)
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	}
	else
	{
		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
		RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}
}


/*************************************************************************
 * Setup window opacity
 *************************************************************************/
void SetWindowOpacity(HWND hWnd, int opacity)
{
	HINSTANCE hFuncInst = LoadLibrary(_T("User32.DLL"));
	if(hFuncInst)
	{
		SetLayeredWindowAttributesFunc SetLayeredWindowAttributesImp = NULL;
		SetLayeredWindowAttributesImp = 
			(SetLayeredWindowAttributesFunc)GetProcAddress(hFuncInst, "SetLayeredWindowAttributes");

		if ( (opacity >= 0) && (opacity < 255) )
		{
			SetWsExLayered(hWnd, TRUE);
			SetLayeredWindowAttributesImp(hWnd, (COLORREF)NULL, (BYTE)opacity, LWA_ALPHA);
		}

		FreeLibrary(hFuncInst);
	}
}

/********************************************************************
 * Gets current config
 ********************************************************************/ 
void GetConfig()
{
	TCHAR CDstr[16];
	char CDstrBuf[32];
	RECT rect;

	memset(CDstr, '\0', sizeof(TCHAR) * 16);
	GetPrivateProfileString (_T("MODE"), _T("METHOD"), _T("2"), CDstr, 16,
			_T("./ar30ex.ini"));

	if(wcscmp(CDstr, _T("1"))==0)
		inputMode = 1;
	else if(wcscmp(CDstr, _T("2"))==0)
		inputMode = 2;
	else if(wcscmp(CDstr, _T("3"))==0)
		inputMode = 3;

	memset(CDstr, '\0', sizeof(TCHAR) * 16);
	GetPrivateProfileString (_T("MODE"), _T("AUTOSWITCH"), _T("1"), CDstr, 16,
			_T("./ar30ex.ini"));

	if(wcscmp(CDstr, _T("1"))==0)
		autoSwitchMode = true;
	else 
		autoSwitchMode = false;
		
		
	memset(CDstr, '\0', sizeof(TCHAR) * 16);
	GetPrivateProfileString (_T("MODE"), _T("SHOWUNICODE"), _T("1"), CDstr, 16,
			_T("./ar30ex.ini"));

	if(wcscmp(CDstr, _T("1"))==0)
		showUnicodeSetting = true;
	else 
		showUnicodeSetting = false;
	

	memset(CDstr, '\0', sizeof(TCHAR) * 16);
	GetPrivateProfileString (_T("MODE"), _T("HANCONVERT"), _T("1"), CDstr, 16,
			_T("./ar30ex.ini"));

	if(wcscmp(CDstr, _T("1"))==0)
		hanConvert = 1;
	else if(wcscmp(CDstr, _T("2"))==0)
		hanConvert = 2;
	else
		hanConvert = 3;

	GetWindowRect(GetDesktopWindow(), &rect);

	WCHAR defaultPOINTX[16];
	int px = rect.left + 3;
	wsprintf(defaultPOINTX, _T("%d"), px);
	memset(CDstr, '\0', sizeof(TCHAR) * 16);
	GetPrivateProfileString (_T("LOCATION"), _T("POINTX"), defaultPOINTX,
			CDstr, 16, _T("./ar30ex.ini"));

	memset(CDstrBuf, '\0', sizeof(CDstrBuf));
	sprintf(CDstrBuf, "%S", CDstr);
	lastPointX = atol(CDstrBuf);

	WCHAR defaultPOINTY[16];
	int py = rect.bottom - 80;
	wsprintf(defaultPOINTY, _T("%d"), py);
	memset(CDstr, '\0', sizeof(TCHAR) * 16);
	GetPrivateProfileString (_T("LOCATION"), _T("POINTY"), defaultPOINTY,
			CDstr, 16, _T("./ar30ex.ini"));

	memset(CDstrBuf, '\0', sizeof(CDstrBuf));
	sprintf(CDstrBuf, "%S", CDstr);
	lastPointY = atol(CDstrBuf);

}


void setPointConfig(long int x, long int y)
{
	WCHAR buffer[16];

	memset(buffer, '\0', sizeof(WCHAR) * 16);
	wsprintf(buffer, _T("%d"), x);
	WritePrivateProfileString(_T("LOCATION"), _T("POINTX"), buffer, _T("./ar30ex.ini"));

	memset(buffer, '\0', sizeof(WCHAR) * 16);
	wsprintf(buffer, _T("%d"), y);
	WritePrivateProfileString(_T("LOCATION"), _T("POINTY"), buffer, _T("./ar30ex.ini"));
}

