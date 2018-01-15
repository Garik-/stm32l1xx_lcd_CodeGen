#include <windows.h>
#include <tchar.h>
#include <stdint.h>
#include <Strsafe.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "resource.h"

#pragma pack(push, 1)

#define __dead
#define TEXT_LIMIT 10

typedef struct
{
	uint32_t CR;        /*!< LCD control register,              Address offset: 0x00 */
	uint32_t FCR;       /*!< LCD frame control register,        Address offset: 0x04 */
	uint32_t SR;        /*!< LCD status register,               Address offset: 0x08 */
	uint32_t CLR;       /*!< LCD clear register,                Address offset: 0x0C */
	uint32_t RESERVED;       /*!< Reserved,                          Address offset: 0x10 */
	uint32_t RAM[16];   /*!< LCD display memory,           Address offset: 0x14-0x50 */
} LCD_TypeDef;

LCD_TypeDef LCD_BASE;
#define LCD ((LCD_TypeDef *) &LCD_BASE)

#define LCD_RAMRegister_0               ((uint32_t)0x00000000) /*!< LCD RAM Register 0  */
#define LCD_RAMRegister_1               ((uint32_t)0x00000001) /*!< LCD RAM Register 1  */
#define LCD_RAMRegister_2               ((uint32_t)0x00000002) /*!< LCD RAM Register 2  */
#define LCD_RAMRegister_3               ((uint32_t)0x00000003) /*!< LCD RAM Register 3  */
#define LCD_RAMRegister_4               ((uint32_t)0x00000004) /*!< LCD RAM Register 4  */
#define LCD_RAMRegister_5               ((uint32_t)0x00000005) /*!< LCD RAM Register 5  */
#define LCD_RAMRegister_6               ((uint32_t)0x00000006) /*!< LCD RAM Register 6  */
#define LCD_RAMRegister_7               ((uint32_t)0x00000007) /*!< LCD RAM Register 7  */
#define LCD_RAMRegister_8               ((uint32_t)0x00000008) /*!< LCD RAM Register 8  */
#define LCD_RAMRegister_9               ((uint32_t)0x00000009) /*!< LCD RAM Register 9  */
#define LCD_RAMRegister_10              ((uint32_t)0x0000000A) /*!< LCD RAM Register 10 */
#define LCD_RAMRegister_11              ((uint32_t)0x0000000B) /*!< LCD RAM Register 11 */
#define LCD_RAMRegister_12              ((uint32_t)0x0000000C) /*!< LCD RAM Register 12 */
#define LCD_RAMRegister_13              ((uint32_t)0x0000000D) /*!< LCD RAM Register 13 */
#define LCD_RAMRegister_14              ((uint32_t)0x0000000E) /*!< LCD RAM Register 14 */
#define LCD_RAMRegister_15              ((uint32_t)0x0000000F) /*!< LCD RAM Register 15 */

/* code for 'µ' character */
#define C_UMAP 0x6084

/* code for 'm' character */
#define C_mMap 0xb210

/* code for 'n' character */
#define C_nMap 0x2210

/* constant code for '*' character */
#define star 0xA0DD

/* constant code for '-' character */
#define C_minus 0xA000

/* constant code for '/' */
#define C_slatch  0x00c0

/* constant code for ° */
#define C_percent_1 0xec00

/* constant code  for small o */
#define C_percent_2 0xb300

#define C_full 0xffdd

/* Constant table for cap characters 'A' --> 'Z' */
const uint16_t CapLetterMap[26]=
{
	/* A      B      C      D      E      F      G      H      I  */
	0xFE00,0x6714,0x1d00,0x4714,0x9d00,0x9c00,0x3f00,0xfa00,0x0014,
	/* J      K      L      M      N      O      P      Q      R  */
	0x5300,0x9841,0x1900,0x5a48,0x5a09,0x5f00,0xFC00,0x5F01,0xFC01,
	/* S      T      U      V      W      X      Y      Z  */
	0xAF00,0x0414,0x5b00,0x18c0,0x5a81,0x00c9,0x0058,0x05c0
};

/* Constant table for number '0' --> '9' */
const uint16_t NumberMap[10]=
{
	/* 0      1      2      3      4      5      6      7      8      9  */
	0x5F00,0x4200,0xF500,0x6700,0xEa00,0xAF00,0xBF00,0x04600,0xFF00,0xEF00
};

#pragma pack(pop)

static void LCD_Conv_Char_Seg(uint8_t* c,bool point,bool column,uint8_t* digit);
static void LCD_GLASS_WriteChar(uint8_t* ch, bool point, bool column, uint8_t position);

/**
* @brief  Converts an ascii char to the a LCD digit.
* @param  c: a char to display.
* @param  point: a point to add in front of char
*         This parameter can be: POINT_OFF or POINT_ON
* @param  column : flag indicating if a column has to be add in front
*         of displayed character.
*         This parameter can be: COLUMN_OFF or COLUMN_ON.
* @param 	digit array with segment 
* @retval None
*/
static void LCD_Conv_Char_Seg(uint8_t* c,bool point,bool column, uint8_t* digit)
{
	uint16_t ch = 0 ;
	uint8_t i,j;

	switch (*c)
	{
	case ' ' : 
		ch = 0x00;
		break;

	case '*':
		ch = star;
		break;

	case 'µ' :
		ch = C_UMAP;
		break;

	case 'm' :
		ch = C_mMap;
		break;

	case 'n' :
		ch = C_nMap;
		break;					

	case '-' :
		ch = C_minus;
		break;

	case '/' :
		ch = C_slatch;
		break;  

	case '°' :
		ch = C_percent_1;
		break;  
	case '%' :
		ch = C_percent_2; 
		break;
	case 255 :
		ch = C_full;
		break ;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':			
		ch = NumberMap[*c-0x30];		
		break;

	default:
		/* The character c is one letter in upper case*/
		if ( (*c < 0x5b) && (*c > 0x40) )
		{
			ch = CapLetterMap[*c-'A'];
		}
		/* The character c is one letter in lower case*/
		if ( (*c <0x7b) && ( *c> 0x60) )
		{
			ch = CapLetterMap[*c-'a'];
		}
		break;
	}

	/* Set the digital point can be displayed if the point is on */
	if (point)
	{
		ch |= 0x0002;
	}

	/* Set the "COL" segment in the character that can be displayed if the column is on */
	if (column)
	{
		ch |= 0x0020;
	}		

	for (i = 12,j=0 ;j<4; i-=4,j++)
	{
		digit[j] = (ch >> i) & 0x0f; //To isolate the less signifiant dibit
	}
}

static void LCD_GLASS_WriteChar(uint8_t* ch, bool point, bool column, uint8_t position)
{
	uint8_t digit[4];     /* Digit frame buffer */

	/* To convert displayed character in segment in array digit */
	LCD_Conv_Char_Seg(ch,point,column,digit);


	switch (position)
	{
		/* Position 1 on LCD (Digit1)*/
	case 1:
		LCD->RAM[LCD_RAMRegister_0] &= 0xcffffffc;
		LCD->RAM[LCD_RAMRegister_2] &= 0xcffffffc;
		LCD->RAM[LCD_RAMRegister_4] &= 0xcffffffc;
		LCD->RAM[LCD_RAMRegister_6] &= 0xcffffffc;

		LCD->RAM[LCD_RAMRegister_0] |= ((digit[0]& 0x0c) << 26 ) | (digit[0]& 0x03) ; // 1G 1B 1M 1E	    
		LCD->RAM[LCD_RAMRegister_2] |= ((digit[1]& 0x0c) << 26 ) | (digit[1]& 0x03) ; // 1F 1A 1C 1D 
		LCD->RAM[LCD_RAMRegister_4] |= ((digit[2]& 0x0c) << 26 ) | (digit[2]& 0x03) ; // 1Q 1K 1Col 1P                                                                                                                                    
		LCD->RAM[LCD_RAMRegister_6] |= ((digit[3]& 0x0c) << 26 ) | (digit[3]& 0x03) ; // 1H 1J 1DP 1N

		break;

		/* Position 2 on LCD (Digit2)*/
	case 2:
		LCD->RAM[LCD_RAMRegister_0] &= 0xf3ffff03;
		LCD->RAM[LCD_RAMRegister_2] &= 0xf3ffff03;      
		LCD->RAM[LCD_RAMRegister_4] &= 0xf3ffff03;
		LCD->RAM[LCD_RAMRegister_6] &= 0xf3ffff03;

		LCD->RAM[LCD_RAMRegister_0] |= ((digit[0]& 0x0c) << 24 )|((digit[0]& 0x02) << 6 )|((digit[0]& 0x01) << 2 ) ; // 2G 2B 2M 2E	  
		LCD->RAM[LCD_RAMRegister_2] |= ((digit[1]& 0x0c) << 24 )|((digit[1]& 0x02) << 6 )|((digit[1]& 0x01) << 2 ) ; // 2F 2A 2C 2D
		LCD->RAM[LCD_RAMRegister_4] |= ((digit[2]& 0x0c) << 24 )|((digit[2]& 0x02) << 6 )|((digit[2]& 0x01) << 2 ) ; // 2Q 2K 2Col 2P
		LCD->RAM[LCD_RAMRegister_6] |= ((digit[3]& 0x0c) << 24 )|((digit[3]& 0x02) << 6 )|((digit[3]& 0x01) << 2 ) ; // 2H 2J 2DP 2N

		break;

		/* Position 3 on LCD (Digit3)*/
	case 3:
		LCD->RAM[LCD_RAMRegister_0] &= 0xfcfffcff;
		LCD->RAM[LCD_RAMRegister_2] &= 0xfcfffcff;
		LCD->RAM[LCD_RAMRegister_4] &= 0xfcfffcff;
		LCD->RAM[LCD_RAMRegister_6] &= 0xfcfffcff;

		LCD->RAM[LCD_RAMRegister_0] |= ((digit[0]& 0x0c) << 22 ) | ((digit[0]& 0x03) << 8 ) ; // 3G 3B 3M 3E	
		LCD->RAM[LCD_RAMRegister_2] |= ((digit[1]& 0x0c) << 22 ) | ((digit[1]& 0x03) << 8 ) ; // 3F 3A 3C 3D
		LCD->RAM[LCD_RAMRegister_4] |= ((digit[2]& 0x0c) << 22 ) | ((digit[2]& 0x03) << 8 ) ; // 3Q 3K 3Col 3P
		LCD->RAM[LCD_RAMRegister_6] |= ((digit[3]& 0x0c) << 22 ) | ((digit[3]& 0x03) << 8 ) ; // 3H 3J 3DP 3N

		break;

		/* Position 4 on LCD (Digit4)*/
	case 4:
		LCD->RAM[LCD_RAMRegister_0] &= 0xffcff3ff;
		LCD->RAM[LCD_RAMRegister_2] &= 0xffcff3ff;
		LCD->RAM[LCD_RAMRegister_4] &= 0xffcff3ff;
		LCD->RAM[LCD_RAMRegister_6] &= 0xffcff3ff;

		LCD->RAM[LCD_RAMRegister_0] |= ((digit[0]& 0x0c) << 18 ) | ((digit[0]& 0x03) << 10 ) ; // 4G 4B 4M 4E	
		LCD->RAM[LCD_RAMRegister_2] |= ((digit[1]& 0x0c) << 18 ) | ((digit[1]& 0x03) << 10 ) ; // 4F 4A 4C 4D
		LCD->RAM[LCD_RAMRegister_4] |= ((digit[2]& 0x0c) << 18 ) | ((digit[2]& 0x03) << 10 ) ; // 4Q 4K 4Col 4P
		LCD->RAM[LCD_RAMRegister_6] |= ((digit[3]& 0x0c) << 18 ) | ((digit[3]& 0x03) << 10 ) ; // 4H 4J 4DP 4N

		break;

		/* Position 5 on LCD (Digit5)*/
	case 5:
		LCD->RAM[LCD_RAMRegister_0] &= 0xfff3cfff;
		LCD->RAM[LCD_RAMRegister_2] &= 0xfff3cfff;
		LCD->RAM[LCD_RAMRegister_4] &= 0xfff3efff;
		LCD->RAM[LCD_RAMRegister_6] &= 0xfff3efff;

		LCD->RAM[LCD_RAMRegister_0] |= ((digit[0]& 0x0c) << 16 ) | ((digit[0]& 0x03) << 12 ) ; // 5G 5B 5M 5E	
		LCD->RAM[LCD_RAMRegister_2] |= ((digit[1]& 0x0c) << 16 ) | ((digit[1]& 0x03) << 12 ) ; // 5F 5A 5C 5D
		LCD->RAM[LCD_RAMRegister_4] |= ((digit[2]& 0x0c) << 16 ) | ((digit[2]& 0x01) << 12 ) ; // 5Q 5K   5P 
		LCD->RAM[LCD_RAMRegister_6] |= ((digit[3]& 0x0c) << 16 ) | ((digit[3]& 0x01) << 12 ) ; // 5H 5J   5N

		break;

		/* Position 6 on LCD (Digit6)*/
	case 6:
		LCD->RAM[LCD_RAMRegister_0] &= 0xfffc3fff;
		LCD->RAM[LCD_RAMRegister_2] &= 0xfffc3fff;
		LCD->RAM[LCD_RAMRegister_4] &= 0xfffc3fff;
		LCD->RAM[LCD_RAMRegister_6] &= 0xfffc3fff;

		LCD->RAM[LCD_RAMRegister_0] |= ((digit[0]& 0x04) << 15 ) | ((digit[0]& 0x08) << 13 ) | ((digit[0]& 0x03) << 14 ) ; // 6B 6G 6M 6E	
		LCD->RAM[LCD_RAMRegister_2] |= ((digit[1]& 0x04) << 15 ) | ((digit[1]& 0x08) << 13 ) | ((digit[1]& 0x03) << 14 ) ; // 6A 6F 6C 6D
		LCD->RAM[LCD_RAMRegister_4] |= ((digit[2]& 0x04) << 15 ) | ((digit[2]& 0x08) << 13 ) | ((digit[2]& 0x01) << 14 ) ; // 6K 6Q    6P 
		LCD->RAM[LCD_RAMRegister_6] |= ((digit[3]& 0x04) << 15 ) | ((digit[3]& 0x08) << 13 ) | ((digit[3]& 0x01) << 14 ) ; // 6J 6H   6N

		break;

	default:
		break;
	}

}

/**
* @brief  This function writes a char in the LCD RAM.
* @param  ptr: Pointer to string to display on the LCD Glass.
* @retval None
*/
void LCD_GLASS_DisplayString(uint8_t* ptr)
{
	uint8_t i = 0x01;



	/* Send the string character by character on lCD */
	while ((*ptr != 0) & (i < 8))
	{
		/* Display one character on LCD */
		LCD_GLASS_WriteChar(ptr, FALSE, FALSE, i);

		/* Point on the next character */
		ptr++;

		/* Increment the character counter */
		i++;
	}
}

HINSTANCE g_hInst;
HWND hWnd;

void OnInitDialog() {
	HICON hMainIcon = LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_MAIN));

	if(NULL != hMainIcon)
		SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)hMainIcon);

	SendDlgItemMessage(hWnd,IDC_EDIT1,EM_SETLIMITTEXT,TEXT_LIMIT,NULL);
}

void Generate_code() {
	char szBuf[TEXT_LIMIT + 1];

	if(GetDlgItemTextA(hWnd, IDC_EDIT1, szBuf, TEXT_LIMIT + 1)) {
		LCD_GLASS_DisplayString((uint8_t *)szBuf);

		TCHAR szCode[0x200];
		unsigned int i = 0;

		i += _stprintf(&szCode[i],TEXT("// String \"%S\"\r\n"),szBuf);

		for(uint32_t counter = LCD_RAMRegister_0; counter < LCD_RAMRegister_15; counter++)
		{
			if(LCD->RAM[counter] > 0x00) {
				i += _stprintf(&szCode[i],TEXT("LCD->RAM[%d] = 0x%X;\r\n"),counter,LCD->RAM[counter]);
			}
		}

		SetDlgItemText(hWnd, IDC_EDIT2, szCode);
	}
}

void Copy2Clipboard()
{
	if(OpenClipboard(NULL) && EmptyClipboard()) {

		size_t len = SendDlgItemMessage(hWnd,IDC_EDIT2,WM_GETTEXTLENGTH,NULL,NULL);

		HGLOBAL hStrMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(TCHAR));
		if(NULL != hStrMem) {

			TCHAR * pStrMem = (TCHAR *)GlobalLock(hStrMem);
			GetDlgItemText(hWnd, IDC_EDIT2,pStrMem,len);
			GlobalUnlock(pStrMem);

			if(NULL != SetClipboardData(CF_UNICODETEXT, hStrMem)) 
				MessageBeep(MB_OK);
		}
		CloseClipboard();
	}
}

BOOL CALLBACK DialogProc(HWND hwndDlg,UINT Message, UINT wParam, LONG lParam)
{
	switch (Message) 
	{ 
	case WM_INITDIALOG:
		hWnd = hwndDlg;
		OnInitDialog();
		return TRUE;
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDOK: 
			Generate_code();
			return TRUE;
		case IDOK2:
			Copy2Clipboard();
			return TRUE;
		case IDCANCEL: 
			EndDialog(hwndDlg, wParam); 
			return TRUE; 
		} 
	} 
	return FALSE; 
}

int WINAPI _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PTSTR lpCmdLine,int nShowCmd)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	g_hInst = hInstance;

	return DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),NULL,(DLGPROC)&DialogProc);
}