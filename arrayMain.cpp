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

#include <vector>
#include "array30.h"
#include "CINFile.h"
#include "BOXFile.h"
#include "OVWildcard.h"

extern HINSTANCE g_hInstance;
extern HWND g_hwnd;
extern HWND hIMMode;
extern HWND hHalfStatus;
extern HWND hInputText;
extern HWND hChoiceText;

extern BOOL isInARMode;
extern BOOL isInFullMode;
extern int  curSize;

extern long int lastPointX;
extern long int lastPointY;
extern bool showUnicodeSetting;

extern bool b_isExtBFontExit;

int  keylistCount;
int  keylistNumber;
bool moreInputMode = false;

std::string key;  //record press key
std::vector<std::wstring> listBuffer; //record code list
std::vector<std::wstring> wordBuffer; //record arbox.txt words
WCHAR showInput[128]; //show key list for hInputText
WCHAR showList[1024]; //show code list for hChioceText
std::wstring showUnicode;

std::string word; //User input key and array30 key name mapping

//Declare table, main table, short code and words
extern CINFile mainTable;
extern CINFile shortCode;
extern BOXFile boxTable;

bool   isInputEnd = false;
bool   isNextRecord = true;
bool   isBoxInput = false;
bool   isWildCard = false;

bool   drawPrev = false;
bool   drawNext = false;

// For add United States - Dvorak keyboard layout support
extern char HalfWordArray[];
extern char HalfDvorakArray[];


/*****************************************************************************
 * Clear Window Text and reset our variable
 *****************************************************************************/ 
void ClearWindow()
{
	//Clear prev and next bimpat (exists or not)
	HDC dc = GetDC(g_hwnd);

	RECT rect;
	rect.left = 610;
	rect.top = 1;
	rect.right = 660;
	rect.bottom = 28;		
	FillRect(dc, &rect, (HBRUSH) GetStockObject(COLOR_APPWORKSPACE));

	ReleaseDC(g_hwnd, dc);
	
	if(showUnicodeSetting && isInARMode==TRUE && isBoxInput==false
			&& listBuffer.empty() != true && showUnicode.empty() != true)
	{
		memset(showInput, '\0', sizeof(WCHAR)*128);
		SetWindowText(hInputText, _T(""));
		
		if(showUnicode.length()==1)
		{
			WCHAR buffer[16];
			wsprintf(buffer, _T("U+%4X"), showUnicode[0]);
			memset(showList, '\0', sizeof(WCHAR)*1024);
			SetWindowText(hChoiceText, buffer);
		}
		else if(showUnicode.length()==2)
		{
			unsigned int u32 = 0x10000 + ((showUnicode[0] - 0xD800) << 10)
						+ (showUnicode[1] - 0xDC00);
			
			WCHAR buffer[16];
			wsprintf(buffer, _T("U+%4X"), u32);
			memset(showList, '\0', sizeof(WCHAR)*1024);
			SetWindowText(hChoiceText, buffer);
		}			
	}
	else
	{
		//Now reset our status
		memset(showInput, '\0', sizeof(WCHAR)*128);
		SetWindowText(hInputText, _T(""));

		memset(showList, '\0', sizeof(WCHAR)*1024);
		SetWindowText(hChoiceText, _T(""));
	}	
	
	showUnicode.clear();
	
	curSize = 0;
	key.clear();
	listBuffer.clear();
	wordBuffer.clear();

	isInputEnd = false;
	moreInputMode = false;
	keylistNumber = 0;	
	isNextRecord = true;
	isBoxInput = false;
	isWildCard = false;

	drawPrev = false;
	drawNext = false;
}


/*****************************************************************************
 * Draw the prev and next Bitmap, use flag to decide draw neg (false) or not
 *****************************************************************************/ 
void ShowPrevNext(bool drawPrev, bool drawNext)
{
	HDC dc = GetDC(g_hwnd);	
	HDC hdc_mem = CreateCompatibleDC(dc);

	RECT rect;
	rect.left = 610;
	rect.top = 1;
	rect.right = 660;
	rect.bottom = 28;		
	FillRect(dc, &rect, (HBRUSH) GetStockObject(COLOR_APPWORKSPACE));

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
	ReleaseDC(g_hwnd, dc);
}


/*****************************************************************************
 * Setup window Text ( for candidate word list)
 *****************************************************************************/ 
void setChoiceText(bool boxFile, WCHAR *listString, int length)
{
	memset(listString, '\0', length * sizeof(WCHAR));

	if(keylistCount <= 10) keylistNumber = 0; //set keylistNumber = 0 if number <= 10

	int number1 = keylistCount/10;
	int number2 = keylistCount%10;

	if(isNextRecord==false)
	{
		if(keylistNumber==0 && keylistCount > 10)
		{
			keylistNumber = number1;
		}
		else if(keylistNumber != 0 && keylistCount > 10)
		{
			keylistNumber--;
		}
	}

	int max;
	if(keylistNumber < number1)
		max = 10;
	else
		max = number2; //in the last keylist Number
	
	for(int i = 0; i < max; i++)
	{
		if(i == 0)
		{
			wcscpy(listString, L" 1");
		}
		else if(i >= 1 && i <= 8)
		{
			WCHAR temp[12];
			wsprintf(temp, TEXT(" %d"), i + 1);	
			wcscat(listString, temp);
		}
		else if(i==9) 
		{
			wcscat(listString, L" 0");
		}	

		if(boxFile)
		{
			wcscat(listString, wordBuffer[keylistNumber * 10 + i].c_str());
		}
		else
		{
			WCHAR buffer[2];
			WCHAR *wptr = (WCHAR *) listBuffer[keylistNumber * 10 + i].c_str();
			if(wcslen(wptr)==2)
			{
				if(b_isExtBFontExit==false)
				{
					buffer[0] = 0x3000;
					buffer[1] = L'\0';

					wcscat(listString, buffer);
				}
				else
				{
					wcscat(listString, listBuffer[keylistNumber * 10 + i].c_str());
				}
			}
			else
			{
				// for MS Mingliu Font 5.03 issue
				if(wptr[0] == 0x2394)
				{
					buffer[0] = 0x3000;
					buffer[1] = L'\0';

					wcscat(listString, buffer);
				}
				else
				{
					wcscat(listString, listBuffer[keylistNumber * 10 + i].c_str());
				}
			}
		}
	}

	if(keylistNumber==0 && keylistCount > 10)
	{
		drawPrev = false;
		drawNext = true;
		
		ShowPrevNext(drawPrev, drawNext);	
	}
	else if(keylistNumber==number1 && keylistCount > 10)
	{
		drawPrev = true;
		drawNext = false;

		ShowPrevNext(drawPrev, drawNext);	
	}
	else if(keylistCount > 10)
	{
		drawPrev = true;
		drawNext = true;
		
		ShowPrevNext(drawPrev, drawNext);			
	}	

	if(keylistNumber < number1 && isNextRecord==true && keylistCount > 10)
	{
		keylistNumber++;
	}
	else if(keylistNumber==number1 && isNextRecord==true && keylistCount > 10)
	{
		keylistNumber = 0;
	}
}


/*****************************************************************************
 * key 1 and key 2 short code handle
 *****************************************************************************/
void getShortCode(WPARAM wParam)
{
	if(wParam != VK_SPACE) //So we can do such thing: send VK_SPACE and does not break original key seq
	{
		if(wParam==VK_BACK) //If the key is VK_BACK, do special handle
		{
			int length;
			length = wcslen(showInput);
			showInput[length - 1] = '\0';
			showInput[length - 2] = '\0';
			showInput[length - 3] = '\0';

			SetWindowText(hInputText, showInput);

			length = key.length();
			key.erase(length - 1);
		}
		else
		{
			WCHAR buf[16];
			memset(buf, '\0', sizeof(WCHAR)*16);
			wsprintf(buf, TEXT(" %S"), word.c_str()); 

			if(curSize==0)
			{
				memset(showInput, '\0', sizeof(WCHAR) * 128);
				wcscpy(showInput, buf);
			}
			else
			{
				wcscat(showInput, buf);
			}

			SetWindowText(hInputText, showInput);

			if(curSize==0)
			{
				key.clear();
				key.push_back((char) wParam);
			}
			else
			{
				key.push_back((char) wParam);
			}
		}
	}

	listBuffer.clear();	
	int count = shortCode.m_table.count(key);
	std::multimap<std::string, std::wstring>::iterator pos;
	for(pos = shortCode.m_table.lower_bound(key);
			pos != shortCode.m_table.upper_bound(key); ++pos)
	{
		listBuffer.push_back(pos->second);
	}

	keylistCount = count;

	WCHAR listString[1024];
	setChoiceText(false, listString, 1024);

	SetWindowText(hChoiceText, listString);
}


/*****************************************************************************
 * main code table handle and wild card support function
 *
 * If user press SPACE (key1 and key2), or user press key (key 3 and above), 
 * we need search main table to let user review the words.
 *****************************************************************************/
void getMainCode(WPARAM wParam)
{
	// It is not a good idea but works, when user press a key, we search our
	// table and get a list, so when user press VK_SPACE, we set flag first then
	// we only need to handle UI change
	if(moreInputMode==false)
	{		
		if(wParam != VK_SPACE) //Prvent wild card mode isInputEnd == true and send wParam == VK_SPACE
		{
			if(wParam==VK_BACK)  // Key VK_BACK need special handle
			{
				int length;
				length = wcslen(showInput);
				showInput[length - 1] = '\0';
				showInput[length - 2] = '\0';
				showInput[length - 3] = '\0';

				SetWindowText(hInputText, showInput);

				length = key.length();
				key.erase(length - 1);
			}		
			else
			{
				WCHAR buf[16];
				memset(buf, '\0', sizeof(WCHAR)*16);
				wsprintf(buf, TEXT(" %S"), word.c_str()); 

				if(curSize==0)
				{
					memset(showInput, '\0', sizeof(WCHAR) * 128);
					wcscpy(showInput, buf);
				}
				else
				{
					wcscat(showInput, buf);
				}

				SetWindowText(hInputText, showInput);

				if(curSize==0)
				{
					key.clear();
					key.push_back((char) wParam);
				}
				else
				{
					key.push_back((char) wParam);
				}	
			}
		}	

		listBuffer.clear();
		int count = 0;
		
		if(isWildCard==false)
		{	
			count = mainTable.m_table.count(key);
			std::multimap<std::string, std::wstring>::iterator pos;
			for(pos = mainTable.m_table.lower_bound(key);
					pos != mainTable.m_table.upper_bound(key); ++pos)
			{
				listBuffer.push_back(pos->second);
			}
		}
		else
		{
			//Let craete out wild card table
			OpenVanilla::OVWildcard wildcard(key, '?', '*', true);
		
			std::multimap<std::string, std::wstring>::iterator pos;
			for(pos = mainTable.m_table.begin();
					pos != mainTable.m_table.end(); ++pos)
			{
				if (wildcard.match(pos->first)) {
					listBuffer.push_back(pos->second);
					count++;
				}
			}
		}

		keylistCount = count;
		keylistNumber = 0;		
	}

	// Depends on flag to set window text
	if(moreInputMode==false && isInputEnd==false)
	{
		if(isWildCard==true)
		{
			//We can guess user still not press VK_SPACE, still show empty list
			SetWindowText(hChoiceText, _T(""));
		}
		else
		{
			WCHAR listString[1024];
			setChoiceText(false, listString, 1024);

			SetWindowText(hChoiceText, listString);
		}
	}
	else
	{
		WCHAR listString[1024];
		setChoiceText(false, listString, 1024);

		SetWindowText(hChoiceText, listString);
	}
}


/*****************************************************************************
 * If user press ' or " key, we need search box table
 *****************************************************************************/
void getBoxWord()
{
	int count = boxTable.m_table.count(key);
	std::multimap<std::string, std::wstring>::iterator pos;
	for(pos = boxTable.m_table.lower_bound(key);
			pos != boxTable.m_table.upper_bound(key); ++pos)
	{
			wordBuffer.push_back(pos->second);
	}	

	keylistCount = count;

	WCHAR listString[1024];
	setChoiceText(true, listString, 1024);

	SetWindowText(hChoiceText, listString);
}


/*****************************************************************************
 * Array30 input method key handle
 * Main function HandleArray()
 *****************************************************************************/
void HandleArray(WPARAM wParam, LPARAM lParam)
{
	GUITHREADINFO gi;
	DWORD Pid;
	HWND hWnd = GetForegroundWindow();
	memset(&gi, 0, sizeof(GUITHREADINFO));
	gi.cbSize = sizeof(GUITHREADINFO);
	GetGUIThreadInfo(GetWindowThreadProcessId(hWnd, &Pid), &gi);
	
	//
	// For add United States - Dvorak keyboard layout support
	//		
	HKL hkl = GetKeyboardLayout(GetWindowThreadProcessId(hWnd, &Pid));	
	WORD layout = HIWORD( hkl );	
	layout = layout & 0xF002;	
	if ( layout == 0xF002) 
	{			
	    int count;
		
		for(count = 0; count < 95; count++) {
			if(wParam== (WPARAM) HalfDvorakArray[count]) {
				break;
			}	
		}
		
		//Now re-map to QWERTY keyboard layout
		wParam = HalfWordArray[count];
	}	

	//Re-map the key for our array30 CIN table
	if(wParam >= 'a' && wParam <= 'z')
		wParam = wParam - 0x20;

	//Now prepare the screen display word
	if(wParam=='Q') word = "1^";
	else if(wParam=='A') word = "1-";
	else if(wParam=='Z') word = "1v";
	else if(wParam=='W') word = "2^";
	else if(wParam=='S') word = "2-";
	else if(wParam=='X') word = "2v";
	else if(wParam=='E') word = "3^";
	else if(wParam=='D') word = "3-";
	else if(wParam=='C') word = "3v";
	else if(wParam=='R') word = "4^";
	else if(wParam=='F') word = "4-";
	else if(wParam=='V') word = "4v";
	else if(wParam=='T') word = "5^";
	else if(wParam=='G') word = "5-";
	else if(wParam=='B') word = "5v";
	else if(wParam=='Y') word = "6^";
	else if(wParam=='H') word = "6-";
	else if(wParam=='N') word = "6v";
	else if(wParam=='U') word = "7^";
	else if(wParam=='J') word = "7-";
	else if(wParam=='M') word = "7v";
	else if(wParam=='I') word = "8^";
	else if(wParam=='K') word = "8-";
	else if(wParam==',') word = "8v";
	else if(wParam=='O') word = "9^";
	else if(wParam=='L') word = "9-";
	else if(wParam=='.') word = "9v";
	else if(wParam=='P') word = "0^";
	else if(wParam==';') word = "0-";
	else if(wParam=='/') word = "0v";
	else if(wParam=='1') word = " 1";
	else if(wParam=='2') word = " 2";
	else if(wParam=='3') word = " 3";
	else if(wParam=='4') word = " 4";
	else if(wParam=='5') word = " 5";
	else if(wParam=='6') word = " 6";
	else if(wParam=='7') word = " 7";
	else if(wParam=='8') word = " 8";
	else if(wParam=='9') word = " 9";
	else if(wParam=='0') word = " 0";
	else if(wParam=='?') word = " ?";
	else if(wParam=='*') word = " *";

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	if((wParam >= 'A' && wParam <= 'Z') ||
			wParam==','|| wParam=='.' || wParam==';' || wParam=='/')
	{				
		if(curSize==0) 
		{
			getShortCode(wParam);
			curSize++;
		}
		else if(curSize >= 1 && curSize <= 4) //Key 1
		{
			//User press SPACE then press other key, send word then
			//re-search table again.
			if(isInputEnd==true)
			{				
				SendUnicodeWord(listBuffer[0], true);
				ClearWindow();

				getShortCode(wParam);
				curSize++;
				goto end;
			}

			if(isBoxInput==true)
			{				
				if(wordBuffer[0].empty() != true)
				{
					SendUnicodeWord(wordBuffer[0], false);
				}
				ClearWindow();

				getShortCode(wParam);
				curSize++;
				goto end;
			}

			if(isWildCard==true)
			{
				getMainCode(wParam);
				curSize++;

				goto end;
			}
			
			if(moreInputMode==false)
			{
				if(curSize==1)
				{
					getShortCode(wParam);
					curSize++;
				}
				else
				{
					getMainCode(wParam);
					curSize++;
				}
			}
			
			goto end;
		}
		else
		{
			//Array30 the max key equals 5, it is the six-th key
			//so reset our state
			MessageBeep(0);
			ClearWindow();
			goto end;
		}
	}
	else if(wParam=='?' || wParam=='*')
	{
		if(curSize >= 5)
		{
			//Array30 the max key equals 5, so reset our state
			MessageBeep(0);
			ClearWindow();
			goto end;
		}
			
		if(moreInputMode==false && isInputEnd==false)
		{
			isWildCard = true;
			getMainCode(wParam);
			curSize++;
			goto end;
		}
	}
	else if(wParam==VK_ESCAPE)
	{
		if(curSize==0) 
		{
			WORD mykeys[1];
			mykeys[0] = VK_ESCAPE;
			SendInputKeys(1, mykeys);	
		}
		else
		{
			ClearWindow();
			goto end;
		}
	}
	else if(wParam==VK_BACK)
	{
		if(curSize==0) 
		{
			WORD mykeys[1];
			mykeys[0] = VK_BACK;
			SendInputKeys(1, mykeys);
		}
		else if(curSize >= 1)
		{
			//For wild card mode to handle
			if(isWildCard==true)
			{
				getMainCode(wParam);
				curSize--;
				
				if(key.find('?') != std::string::npos || key.find('*') != std::string::npos)
				{
					//Still has wild card char, so ingore this handle, jump to end
					goto end;
				}
				else
				{				
					isWildCard = false; //No wild card char, set wild card mode to false

					if(curSize==0)
						ClearWindow();
					else if(curSize==1 || curSize==2)
						getShortCode((WPARAM) VK_SPACE);
					else	
						getMainCode((WPARAM) VK_SPACE);
				}				

				goto end;
			}

			// Also set isInputEnd flag to false
			isInputEnd = false;
			isBoxInput = false;
			moreInputMode = false;
			
			if(curSize==1) //Key 1
			{
				ClearWindow();
				goto end;
			}

			if(curSize==2 || curSize==3)
			{
				getShortCode(wParam);
				curSize--;
			}
			else 
			{
				getMainCode(wParam);
				curSize--;
			}
		}
	}		
	else
	{
		if(curSize==0)
		{			
			//Check half/full state, then send word
			if(isInFullMode)
				FullEnglish(wParam, lParam);
			else
				HalfEnglish(wParam, lParam);
		}
		else
		{
			if(wParam >= '0' && wParam <= '9')
			{
				if(isBoxInput==true)
				{
					if(wParam=='0') wParam = wParam + 10;

					if((unsigned int) keylistCount > (wParam - 0x30 - 1))
					{						
						if(wordBuffer[wParam - 0x30 - 1].empty() != true)
							SendUnicodeWord(wordBuffer[wParam - 0x30 - 1], false);
					}
					else
					{
						MessageBeep(0);
					}

					ClearWindow();
					goto end;
				}
				else
				{
					if(curSize >= 1)
					{
						if(curSize==1 && key[0] == 'W') //Special handle for symbol mode
						{
							getMainCode(wParam);
							moreInputMode = true;
							curSize++;
							goto end;
						}

						if(wParam=='0')
						{
							wParam = wParam + 10;
						}

						if(moreInputMode==false)
						{
							if((unsigned int) keylistCount > (wParam - 0x30 - 1))
							{
								WCHAR c = 0x2394;
								std::wstring temp;
								temp += c;
								
								if(listBuffer[wParam - 0x30 - 1] != temp) //If value is empty, skip it.								
									SendUnicodeWord(listBuffer[wParam - 0x30 - 1], true);								
								else
									MessageBeep(0);								
							}
							else
							{
								MessageBeep(0);
							}						
						}
						else //more input mode or more than 10 values handle
						{
							//We we have one more pages
							int number1 = keylistCount/10;
							int number2 = keylistCount%10;

							if(isNextRecord)
							{
								if(keylistNumber==0)
								{
									if((unsigned int) number2 > (wParam - 0x30 - 1))
										SendUnicodeWord(listBuffer[number1 * 10 + wParam - 0x30 - 1], true);
									else
										MessageBeep(0);
								}
								else
								{									
									SendUnicodeWord(listBuffer[(keylistNumber - 1) * 10 + wParam - 0x30 - 1], true);
								}
							}
							else
							{
								if(keylistNumber==0)
								{									
									SendUnicodeWord(listBuffer[wParam - 0x30 - 1], true);
								}
								else
								{
									if(keylistNumber==number1)
									{
										//In last, need check its range
										if((unsigned int) number2 > (wParam - 0x30 - 1))										
											SendUnicodeWord(listBuffer[keylistNumber * 10 + wParam - 0x30 - 1], true);
										else
											MessageBeep(0);
									}
									else
									{										
										SendUnicodeWord(listBuffer[keylistNumber * 10 + wParam - 0x30 - 1], true);
									}
								}
							}
						}

						ClearWindow();
						goto end;
					}
				}
			}
			else if(wParam=='\'' || wParam=='\"')
			{
				if(isWildCard==true) //Sorry, but not support
				{
					MessageBeep(0);
					ClearWindow();
					goto end;
				}
				
				getBoxWord();

				if(keylistCount != 0)
				{
					isBoxInput = true;
				}	
				else
				{
					MessageBeep(0);
					ClearWindow();
					goto end;
				}
			}
			else if((wParam=='<' || wParam=='>'))
			{
				//Support more input mode use SHIFT + < or > to change page
				if(moreInputMode==true)
				{
					if((wParam=='<' || wParam=='>') && GetKeyState(VK_SHIFT) < 0)
					{
						if(wParam=='<')
						{
							//When change true to false, modify to correct value.
							//the keylistNumber is current value + 1...
							if(isNextRecord==true)
							{
								if(keylistNumber > 0)
								{
									keylistNumber--;
								}
								else
								{
									int number1 = keylistCount/10;
									keylistNumber = number1;
								}
							}

							isNextRecord = false;
						}
						else
						{
							if(isNextRecord==false)
							{
								int number1 = keylistCount/10;

								if(keylistNumber < number1)
								{
									keylistNumber++;
								}
								else
								{
									keylistNumber = 0;
								}
							}

							isNextRecord = true;
						}
						
						//Only update UI
						getMainCode(wParam);
						goto end;
					}
				}
			}
			else if(wParam==VK_SPACE)
			{
				if(isBoxInput==true)
				{					
					if(wordBuffer[0].empty() != true) SendUnicodeWord(wordBuffer[0], false);

					ClearWindow();
					goto end;
				}

				//If we are in moreInput mode, SPACE is change to next page	
				if(moreInputMode==true) 
				{					
					//Only update UI
					getMainCode(wParam);					
					goto end;
				}

				//User press SPACE again, so send first word in our list
				if(isInputEnd==true)
				{					
					SendUnicodeWord(listBuffer[0], true);

					ClearWindow();
					goto end;
				}

				//get the main table code, if only 1 key-value pair send directly
				//Array30 first show short code, if user press SPACE key then show main table code
				if(isWildCard==false && (curSize==1 || curSize==2))
				{
					//In Key 1 and key 2, if user press SPACE need search main table
					getMainCode(wParam);
				}				

				if(curSize==5)
				{
					if(key==",,OPT")
					{
						DialogBox(g_hInstance, _T("SETDIALOG"), g_hwnd, SettingDlgProc);
										
						//Reset our status and redraw our window
						ClearWindow();						
						RedrawWindow(g_hwnd, NULL, NULL, 
							RDW_ERASE|RDW_INVALIDATE|RDW_FRAME|RDW_ALLCHILDREN|RDW_ERASENOW);
						
						goto end;
					}
					else if(key==",,OUT")
					{
						SendMessage(g_hwnd, WM_CLOSE, 0, 0);
						goto end;
					}
					else if(key==",,PLC")
					{
						RECT rect;
						GetWindowRect(GetDesktopWindow(), &rect);

						int px = rect.left + 3;
						int py = rect.bottom - 80;

						MoveWindow(g_hwnd, px, py, 725, 30, TRUE);

						//Write to last point x, y
						lastPointX = px;
						lastPointY = py;

						setPointConfig(lastPointX, lastPointY);

						ClearWindow();
						goto end;
					}
					else
					{
						goto skipme;
					}	
				}

skipme:			
				if(keylistCount==1)
				{			    					
					SendUnicodeWord(listBuffer[0], true);

					ClearWindow();
					goto end;
				}
				else if(keylistCount==0)
				{
					MessageBeep(0);

					ClearWindow();
					goto end;
				}
				else
				{
					if(keylistCount > 10)
						moreInputMode = true;												
					else
						isInputEnd = true;
					
					//Let call getMainCode() to show list
					//Wild card case need update UI after press VK_SPACE
					if(isWildCard==true)
					{
						//If we set flag isInputEnd to true, need in getMainCode() skip VK_SPACE case
						getMainCode(wParam);
					}
				}
			}
			else
			{
				if(isInFullMode)
				{
					FullEnglish(wParam, lParam);
				}
				else
				{
					HalfEnglish(wParam, lParam);
				}
			}
		}
	}

end:
	return;	
}
