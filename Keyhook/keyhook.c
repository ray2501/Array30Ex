#include "keyhook.h"
HINSTANCE g_hinstDll = NULL;

#if (_MSC_VER)
#pragma data_seg(".shared")
HHOOK g_keyhhook = NULL;
HWND g_hwndPost = NULL; // Window to notify of keyboard events
UINT g_kmessage = WM_USER;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.shared,RWS")
#elif (__MINGW__)
HHOOK g_keyhhook __attribute__((section ("Shared"), shared)) = NULL;
HWND g_hwndPost __attribute__((section ("Shared"), shared)) = NULL;
UINT g_kmessage __attribute__((section ("Shared"), shared)) = WM_USER;
#elif (__WATCOM__)
HHOOK __far g_keyhhook;
HWND __far g_hwndPost;
UINT __far g_kmessage;
#endif


static LRESULT WINAPI KeyHook_HookProc (int nCode, WPARAM wParam, LPARAM lParam)
{	
	if (nCode < 0)
	{
		return(CallNextHookEx(g_keyhhook, nCode, wParam, lParam));
	}
	else if (nCode == HC_ACTION) 
	{
		//Check whether key was pressed(not released)
		if((lParam & 0x80000000) == 0x00000000)
		{			
		    if(GetKeyState(VK_CONTROL) >= 0 && GetKeyState(VK_MENU) >= 0)
		    {				
				TCHAR ac[2];
				UINT uScanCodeAndShiftState = HIWORD(lParam) & (0x00FF | KF_UP);
				BYTE bKeyState[256];				

				GetKeyboardState(bKeyState);

				if (ToAscii(wParam, uScanCodeAndShiftState, bKeyState, (PWORD) ac, 0) == 1)
				{	
					if(ac[0] >= 0x20 && ac[0] <= 0x7e)
					{												
						PostMessage(g_hwndPost, g_kmessage, (WPARAM) ac[0], lParam);
						return 1;
					}
				}

				return(CallNextHookEx(g_keyhhook, nCode, wParam, lParam));
		    }
		}

		return(CallNextHookEx(g_keyhhook, nCode, wParam, lParam));
	}

	return(CallNextHookEx(g_keyhhook, nCode, wParam, lParam));
}


_KEYHOOKEXPORT BOOL WINAPI installKeyHook (HWND hwnd, UINT message)
{
	HHOOK hhook;

	if (g_keyhhook != NULL)
		return(FALSE);

	g_hwndPost = hwnd;
	g_kmessage = message;

	Sleep(0); //for thread

	hhook = SetWindowsHookEx(WH_KEYBOARD, KeyHook_HookProc, g_hinstDll, 0);

	InterlockedExchange((PLONG) &g_keyhhook, (LONG) hhook);	

	return(g_keyhhook != NULL);
}

_KEYHOOKEXPORT BOOL WINAPI removeKeyHook()
{
	BOOL fOK = TRUE;

	// Only uninstall the hook if it was successfully installed.
	if (g_keyhhook != NULL) {
		fOK = UnhookWindowsHookEx(g_keyhhook);
		g_keyhhook = NULL;
	}

	return(fOK);
}

BOOL __stdcall DllMain (HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			g_hinstDll = hinstDll;

			break;
	}

	return(TRUE);
}

