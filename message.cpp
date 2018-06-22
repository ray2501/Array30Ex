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
 *****************************************************************************/ 

/******************************************************************************
 * Handle Windows message
 *****************************************************************************/ 
#include "array30.h"
#include "resource.h"
#include "CINFile.h"
#include "BOXFile.h"
#include "keyhook.h"
#include "listfile.h"

extern HINSTANCE g_hInstance;
extern HMENU hmenu;
extern HFONT hFont;
extern HWND hIMMode;
extern HWND hHalfStatus;
extern HWND hInputText;
extern HWND hChoiceText;
extern NOTIFYICONDATA data; //for system tray

extern UINT g_message;
extern BOOL isInARMode;
extern BOOL isInFullMode;
extern int  curSize;
extern HANDLE Mutex;

extern CINFile mainTable;
extern CINFile shortCode;
extern BOXFile boxTable;
extern LISTFILE listclass;
extern LISTFILE listtitle;

extern int inputMode;
extern int hanConvert;
extern bool autoSwitchMode;
extern long int lastPointX;
extern long int lastPointY;
extern bool showUnicodeSetting;

//Function pointer
extern HINSTANCE hFuncInst;
extern SetLayeredWindowAttributesFunc SetLayeredWindowAttributesImp;

//Hot Key ID
extern int HotKeyID1;
extern int HotKeyID2;
extern int HotKeyID3;
extern int HotKeyID4;
extern int HotKeyID5;
extern int HotKeyID6;
extern int HotKeyID7;

extern bool moreInputMode;
extern bool drawPrev;
extern bool drawNext;


void SwitchToArray()
{
	TCHAR arModeMsg[16];
	LoadStringW(g_hInstance, IDS_arModeMsg, arModeMsg, sizeof(arModeMsg)/sizeof(TCHAR));

	isInARMode = TRUE;
	SetWindowText(hIMMode, arModeMsg);

	ClearWindow();
	
	SendMessage(hIMMode, WM_KILLFOCUS, 0, 0);
}


void SwitchToEnglish()
{
	TCHAR enModeMsg[16];
	LoadStringW(g_hInstance, IDS_enModeMsg, enModeMsg, sizeof(enModeMsg)/sizeof(TCHAR));

	isInARMode = FALSE;
	SetWindowText(hIMMode, enModeMsg);

	ClearWindow();
	
	SendMessage(hIMMode, WM_KILLFOCUS, 0, 0);
}


void SwitchToFull()
{
	TCHAR fullMsg[16];
	LoadStringW(g_hInstance, IDS_fullMsg, fullMsg, sizeof(fullMsg)/sizeof(TCHAR));

	isInFullMode = TRUE;
	SetWindowText(hHalfStatus, fullMsg);

	SetWindowText(hInputText, _T(""));
	SetWindowText(hChoiceText,_T(""));

	SendMessage(hHalfStatus, WM_KILLFOCUS, 0, 0);
}


void SwitchToHalf()
{
	TCHAR halfMsg[16];
	LoadStringW(g_hInstance, IDS_halfMsg, halfMsg, sizeof(halfMsg)/sizeof(TCHAR));

	isInFullMode = FALSE;
	SetWindowText(hHalfStatus, halfMsg);

	SetWindowText(hInputText, _T(""));
	SetWindowText(hChoiceText,_T(""));

	SendMessage(hHalfStatus, WM_KILLFOCUS, 0, 0);
}


/********************************************************************
 * Setting dialog's windows procedure
 ********************************************************************/ 
BOOL CALLBACK SettingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hRadio1, hRadio2, hRadio3, hRadio4, hRadio5, hRadio6;
	static HWND hCheck1;

	switch(message)
	{
		case WM_INITDIALOG:
			hRadio1 = GetDlgItem(hDlg, IDC_RADIO1);
			hRadio2 = GetDlgItem(hDlg, IDC_RADIO2);
			hRadio3 = GetDlgItem(hDlg, IDC_RADIO3);
			hRadio4 = GetDlgItem(hDlg, IDC_RADIO4);
			hRadio5 = GetDlgItem(hDlg, IDC_RADIO5);
			hRadio6 = GetDlgItem(hDlg, IDC_RADIO6);
			hCheck1 = GetDlgItem(hDlg, IDC_CHECK1); 

			//Change: follow current setting, no need read again
			if(inputMode==1)
				Button_SetCheck(hRadio1, BST_CHECKED);
			else if(inputMode==2)
				Button_SetCheck(hRadio2, BST_CHECKED);
			else if(inputMode==3)
				Button_SetCheck(hRadio3, BST_CHECKED);

			//Set auto switch option
			if(autoSwitchMode==true)
				Button_SetCheck(hCheck1, BST_CHECKED);

			//Set han convert option
			if(hanConvert==1)
				Button_SetCheck(hRadio4, BST_CHECKED);
			else if(hanConvert==2)
				Button_SetCheck(hRadio5, BST_CHECKED);
			else if(hanConvert==3)
				Button_SetCheck(hRadio6, BST_CHECKED);

			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					//Final get the value
					if(Button_GetCheck(hRadio1)==BST_CHECKED)
						inputMode = 1;
					else if(Button_GetCheck(hRadio2)==BST_CHECKED)
						inputMode = 2;
					else if(Button_GetCheck(hRadio3)==BST_CHECKED)
						inputMode = 3;

					if(Button_GetCheck(hCheck1)==BST_CHECKED)
						autoSwitchMode = true;
					else
						autoSwitchMode = false;

					if(Button_GetCheck(hRadio4)==BST_CHECKED)
						hanConvert = 1;
					else if(Button_GetCheck(hRadio5)==BST_CHECKED)
						hanConvert = 2;
					else if(Button_GetCheck(hRadio6)==BST_CHECKED)
						hanConvert = 3;

					WCHAR buffer[16];

					memset(buffer, '\0', sizeof(WCHAR) * 16);
					wsprintf(buffer, _T("%d"), inputMode);
					WritePrivateProfileString(_T("MODE"), _T("METHOD"), buffer, _T("./ar30ex.ini"));

					memset(buffer, '\0', sizeof(WCHAR) * 16);
					wsprintf(buffer, _T("%d"), autoSwitchMode==true?1:2);
					WritePrivateProfileString(_T("MODE"), _T("AUTOSWITCH"), buffer, _T("./ar30ex.ini"));

					memset(buffer, '\0', sizeof(WCHAR) * 16);
					wsprintf(buffer, _T("%d"), hanConvert);
					WritePrivateProfileString(_T("MODE"), _T("HANCONVERT"), buffer, _T("./ar30ex.ini"));

					EndDialog(hDlg, IDOK);
					return TRUE;
			}

			return FALSE;
	}

	return FALSE;
}


BOOL OnCreate(HWND hwnd, LPCREATESTRUCT pCreateStruct)
{
	Mutex = CreateMutex(NULL,FALSE, _T("OnlyOne"));
	if (GetLastError()==ERROR_ALREADY_EXISTS)
	{
		::MessageBox(hwnd, _T("This program is only run on instance!"), _T("ERROR"), MB_OK);
		ReleaseMutex(Mutex);

		return FALSE;
	}

	//Initial our array30 CIN table
	mainTable.setFilename(std::string("array30.cin"));
	if(mainTable.readTable()==false)
	{
		::MessageBox(hwnd, _T("Read table array30.cin fail."), _T("ERROR"), MB_OK);
		return FALSE;
	}

	shortCode.setFilename(std::string("ArrayShortCode.cin"));
	if(shortCode.readTable()==false)
	{
		::MessageBox(hwnd, _T("Read table ArrayShortCode.cin fail."), _T("ERROR"), MB_OK);
		return FALSE;
	}

	//Initial arbox.txt
	boxTable.setFilename(std::string("arbox.txt"));
	if(boxTable.readTable()==false)
	{
		::MessageBox(hwnd, _T("Read table arbox.txt fail."), _T("ERROR"), MB_OK);
		return FALSE;
	}
	
	//Initial our SEND METHOD Windows TITLE LIST
	listtitle.setFilename(std::string("listtitle.txt"));
	if(listtitle.readTable()==false)
	{
		::MessageBox(hwnd, _T("Read table listtitle.txt fail."), _T("ERROR"), MB_OK);
		return FALSE;
	}
	
	//Initial our SEND METHOD Windows CLASS LIST
	listclass.setFilename(std::string("listclass.txt"));
	if(listclass.readTable()==false)
	{
		::MessageBox(hwnd, _T("Read table listclass.txt fail."), _T("ERROR"), MB_OK);
		return FALSE;
	}	

	hmenu = LoadMenu (pCreateStruct-> hInstance, _T("POPMENU"));
	hmenu = GetSubMenu(hmenu, 0);

	EnableMenuItem(hmenu, MENU_HIDE, MF_ENABLED);	
	EnableMenuItem(hmenu, MENU_SHOW, MF_GRAYED | MF_DISABLED);
	
	if(showUnicodeSetting)
	{
		CheckMenuItem(hmenu, MENU_SHOWNO, MF_UNCHECKED);
		CheckMenuItem(hmenu, MENU_SHOWUNI, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hmenu, MENU_SHOWNO, MF_CHECKED);
		CheckMenuItem(hmenu, MENU_SHOWUNI, MF_UNCHECKED);
	}

	TCHAR fontName[16];
	LoadStringW(g_hInstance, IDS_fontName, fontName, sizeof(fontName)/sizeof(TCHAR));

	hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, CHINESEBIG5_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, fontName);

	TCHAR arModeMsg[16];
	LoadStringW(g_hInstance, IDS_arModeMsg, arModeMsg, sizeof(arModeMsg)/sizeof(TCHAR));

	hIMMode = CreateWindowEx(0, _T("button"), arModeMsg,
			WS_CHILD | WS_VISIBLE,
			1, 1, 52, 26, hwnd, NULL, pCreateStruct-> hInstance, NULL);

	TCHAR halfMsg[16];
	LoadStringW(g_hInstance, IDS_halfMsg, halfMsg, sizeof(halfMsg)/sizeof(TCHAR));

	hHalfStatus = CreateWindowEx(0, _T("button"), halfMsg,
			WS_CHILD | WS_VISIBLE,
			53, 1, 26, 26, hwnd, NULL, pCreateStruct-> hInstance, NULL);

	hInputText = CreateWindowEx(SS_LEFT, _T("static"), _T(""),
			WS_CHILD|WS_VISIBLE|WS_BORDER,
			79, 1, 130, 26, hwnd, NULL, pCreateStruct-> hInstance, NULL);

	SendMessage(hInputText, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	hChoiceText = CreateWindowEx(SS_LEFT, _T("static"), _T(""),
			WS_CHILD|WS_VISIBLE|WS_BORDER,
			209, 1, 400, 26, hwnd, NULL, pCreateStruct-> hInstance, NULL);

	SendMessage(hChoiceText, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	//Hotkey
	RegisterHotKey(hwnd, HotKeyID1, MOD_SHIFT, VK_F8);
	RegisterHotKey(hwnd, HotKeyID2, MOD_SHIFT, VK_F9);
	RegisterHotKey(hwnd, HotKeyID3, MOD_CONTROL, VK_SPACE);
	RegisterHotKey(hwnd, HotKeyID4, MOD_SHIFT, VK_SPACE);
	RegisterHotKey(hwnd, HotKeyID5, 0, VK_BACK);
	RegisterHotKey(hwnd, HotKeyID6, 0, VK_ESCAPE);
	RegisterHotKey(hwnd, HotKeyID7, MOD_SHIFT, 0);

	return TRUE;
}

void OnCommand(HWND hwnd, UINT id, HWND hwndCtl, UINT codeNotify)
{
	TCHAR szInfo[64] = _T("Array30Ex");
	BYTE opacity;

	switch(id) {
		case MENU_HIDE:
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

			break;
		case MENU_SHOW:
			Shell_NotifyIcon(NIM_DELETE, &data);
			ShowWindow(hwnd, SW_SHOWNORMAL);

			hmenu = LoadMenu (g_hInstance, _T("POPMENU"));
			hmenu = GetSubMenu(hmenu, 0);

			EnableMenuItem(hmenu, MENU_HIDE, MF_ENABLED);	
			EnableMenuItem(hmenu, MENU_SHOW, MF_GRAYED | MF_DISABLED);

			break;
		case MENU_OPA10:
			SetWsExLayered(hwnd, FALSE);
			break;

		case MENU_OPA09:
			opacity = (BYTE) 255 * 90 / 100;
			goto setopacity;

		case MENU_OPA08:
			opacity = (BYTE) 255 * 80 / 100;
			goto setopacity;

		case MENU_OPA07:
			opacity = (BYTE) 255 * 70 / 100;
			goto setopacity;

		case MENU_OPA06:
			opacity = (BYTE) 255 * 60 / 100;
			goto setopacity;

		case MENU_OPA05:
			opacity = (BYTE) 255 * 50 / 100;
			goto setopacity;

		case MENU_OPA04:
			opacity = (BYTE) 255 * 40 / 100;
			goto setopacity;

		case MENU_OPA03:
			opacity = (BYTE) 255 * 30 / 100;
			goto setopacity;

		case MENU_OPA02:
			opacity = (BYTE) 255 * 20 / 100;
			goto setopacity;

		case MENU_OPA01:
			opacity = (BYTE) 255 * 10 / 100;
setopacity:
			SetWindowOpacity(hwnd, opacity);
			break;

		case MENU_SETTING:
			DialogBox(g_hInstance, _T("SETDIALOG"), hwnd, SettingDlgProc);

			RedrawWindow(hwnd, NULL, NULL, 
				RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN|RDW_ERASENOW);
				
			break;
			
		case MENU_SHOWNO:
		case MENU_SHOWUNI:
			if(id==MENU_SHOWNO)
			{
				CheckMenuItem(hmenu, MENU_SHOWNO, MF_CHECKED);
				CheckMenuItem(hmenu, MENU_SHOWUNI, MF_UNCHECKED);
				showUnicodeSetting = false;
			}
			else if(id==MENU_SHOWUNI)
			{
				CheckMenuItem(hmenu, MENU_SHOWNO, MF_UNCHECKED);
				CheckMenuItem(hmenu, MENU_SHOWUNI, MF_CHECKED);
				showUnicodeSetting = true;
			}
			
			WCHAR buffer[16];
			memset(buffer, '\0', sizeof(WCHAR) * 16);
			wsprintf(buffer, _T("%d"), showUnicodeSetting==true?1:2);
			WritePrivateProfileString(_T("MODE"), _T("SHOWUNICODE"), buffer, _T("./ar30ex.ini"));
			break;
			
		case MENU_EXIT:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;
	}

	if(hwndCtl==hIMMode)
	{
		if(isInARMode)
			SwitchToEnglish();			
		else
			SwitchToArray();
	}
	else if(hwndCtl==hHalfStatus)
	{
		if(isInFullMode)
			SwitchToHalf();
		else
			SwitchToFull();
	}
}


void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	
	HDC dc = BeginPaint(hwnd, &ps);
	HDC hdc_mem = CreateCompatibleDC(dc);

	RECT rect;
	rect.left = 610;
	rect.top = 1;
	rect.right = 660;
	rect.bottom = 28;		
	FillRect(dc, &rect, (HBRUSH) GetStockObject (COLOR_APPWORKSPACE));
		
	if(moreInputMode)
	{
		BITMAP bmp_info, bmp_info2;

		HBITMAP hBMPprev;
		if(drawPrev)
			hBMPprev = LoadBitmap(g_hInstance, _T("PAGEUP"));
		else
			hBMPprev = LoadBitmap(g_hInstance, _T("NPAGEUP"));

		HBITMAP hBMPnext;
		if(drawNext)
			hBMPnext = LoadBitmap(g_hInstance, _T("PAGEDN"));
		else
			hBMPnext = LoadBitmap(g_hInstance, _T("NPAGEDN"));

		GetObject(hBMPprev, (int)sizeof(BITMAP), &bmp_info);
		GetObject(hBMPnext, (int)sizeof(BITMAP), &bmp_info2);
		SelectObject(hdc_mem, hBMPprev);
		BitBlt(dc, 611, 2, 24, 24, hdc_mem, 0, 0, SRCCOPY);

		SelectObject(hdc_mem, hBMPnext);
		BitBlt(dc, 636, 2, 24, 24, hdc_mem, 0, 0, SRCCOPY);

		DeleteDC(hdc_mem);
		DeleteObject(hBMPprev);
		DeleteObject(hBMPnext);		
	}	
	
	EndPaint(hwnd, &ps);
	
	//Update child window
	UpdateWindow(hIMMode);
	UpdateWindow(hHalfStatus);
	UpdateWindow(hInputText);
	UpdateWindow(hChoiceText);	
}



void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	SendMessage(hwnd, WM_NCLBUTTONDOWN,  HTCAPTION, MAKELPARAM(x, y));

	POINT point;

	point.x = 0;
	point.y = 0;

	ClientToScreen(hwnd, &point);

	//Write to last point x, y
	lastPointX = point.x;
	lastPointY = point.y;

	setPointConfig(lastPointX, lastPointY);
}


void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	POINT point;

	point.x = x;
	point.y = y;

	ClientToScreen(hwnd, &point);
	TrackPopupMenu(hmenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);
}

BOOL OnWindowPosChanging(HWND hwnd, WINDOWPOS *pwp)
{
	pwp->flags |= SWP_NOSIZE;
	/* Continue with default handling */
	return FORWARD_WM_WINDOWPOSCHANGING(hwnd, pwp, DefWindowProc);
}


void OnClose(HWND hwnd)
{
	UnregisterHotKey(hwnd, HotKeyID1);
	UnregisterHotKey(hwnd, HotKeyID2);
	UnregisterHotKey(hwnd, HotKeyID3);
	UnregisterHotKey(hwnd, HotKeyID4);
	UnregisterHotKey(hwnd, HotKeyID5);
	UnregisterHotKey(hwnd, HotKeyID6);
	UnregisterHotKey(hwnd, HotKeyID7);

	removeKeyHook();
	CloseHandle(Mutex);
	DestroyWindow(hwnd);
}

void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}
