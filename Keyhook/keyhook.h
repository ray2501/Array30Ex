#ifndef _KEYHOOK_WRAPPER__
#define _KEYHOOK_WRAPPER__
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#endif


#ifdef BUILD_DLL
	#if defined(__cplusplus)
		#define _KEYHOOKEXPORT extern "C" __declspec(dllexport)		
	#else
		#define _KEYHOOKEXPORT __declspec(dllexport)		
	#endif
#else
	#if defined(__cplusplus)
		#define _KEYHOOKEXPORT extern "C" __declspec(dllimport)
	#else
		#define _KEYHOOKEXPORT __declspec(dllimport)
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

_KEYHOOKEXPORT BOOL WINAPI installKeyHook (HWND hwnd, UINT message);
_KEYHOOKEXPORT BOOL WINAPI removeKeyHook ();

#ifdef __cplusplus
}
#endif /* __cplusplus */

