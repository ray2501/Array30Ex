#ifndef __ARRAY30__
#define __ARRAY30__

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include <string>

#define NOTIFY_MESSAGE (WM_APP+1)

typedef BOOL (WINAPI *SetLayeredWindowAttributesFunc)(HWND,COLORREF,BYTE,DWORD);

#ifdef __cplusplus
extern "C" {
void GetConfig();
void setPointConfig(long int x, long int y);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Check Unicode ExtB font
bool isExtBFontExit();

//Set Opacity function
void SetWsExLayered(HWND hWnd, BOOL flag);
void SetWindowOpacity(HWND hWnd, int opacity);

//Message handler
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT pCreateStruct);
void OnCommand(HWND hwnd, UINT id, HWND hwndCtl, UINT codeNotify);
void OnPaint(HWND hwnd);
void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
BOOL OnWindowPosChanging(HWND hwnd, WINDOWPOS *pwp);
void OnClose(HWND hwnd);
void OnDestroy(HWND hwnd);

//Setting Dialog procedure
BOOL CALLBACK SettingDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void SwitchToArray();
void SwitchToEnglish();
void SwitchToFull();
void SwitchToHalf();

//Keyboard Hook and Key Handle function
void ClearCllpboard();
void CopyToClipboard(HWND hwnd, std::wstring& string);
void SendInputKeys(int number, WORD *keys);
void SendUnicodeWord(std::wstring string, bool isWord);
void HalfEnglish(WPARAM wParam, LPARAM lParam);
void FullEnglish(WPARAM wParam, LPARAM lParam);
void HandleArray(WPARAM wParam, LPARAM lParam);
void keyHandle(HWND hwnd, WPARAM wParam, LPARAM lParam);

void ClearWindow();
void ShowPrevNext(bool prev, bool next);
void setChoiceText(bool boxFile, WCHAR *listString, int length);
void getShortCode(WPARAM wParam);
void getMainCode(WPARAM wParam);
void getBoxWord(); //array box support

//Traditional<->Simplified Chinese convert function
int HCCompare(const void *a, const void *b);
unsigned short HCFind(unsigned key, unsigned short *table, size_t size);
unsigned short UCS2TradToSimpChinese(unsigned short c);
unsigned short UCS2SimpToTradChinese(unsigned short c);
}
#endif
#endif
