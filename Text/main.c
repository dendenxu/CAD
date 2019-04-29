/*
4-29-2019 update:
- fixed problem when trying to add a letter after shifting the position of the cursor
- tring Consolas as an output font
README:

This program displays the text you input(imitating a terminal).
It uses line buffer like many terminals.
It interprets TAB as 4 spaces.
It supports ENTER key, interpreting it as line shifter.
LEFTARROW and RIGHTARROW can be used for moving the cursor.
BACKSPACE and DELETE are supported.
When you want to quit, hit the ESC key and your input will be displayed in a real terminal.
After that hit CTRL+C or ALT+F4 to close the program.

Modifications are supported.
- font
	- weight
	- size
- text
	- position
	- line spacing
- cursor
	- weight
	- length
- color
	- font color
	- background(black or white)


Currently monospaced fonts are completely supported.
Particular letters may be displayed incorrectly when using proportional font.
*/

#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <stdio.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#define SHADEOFGRAY 0.85//Percentage of grayc of the text. (One means white and zero means black.)(Using gray to imitate the terminal environment.)
#define FONTNAME "Consolas"//Fontname of the displayed text. (Terminal font doesn't change with this.)(Currently only monospaced font are guaranteed to be properly displayed. If you try to use non-monospace font, the program may work properly most of the time. But for some letter like 'f', stain may occur when editing the text.)
#define LAMBDA 1//Lambda stands for the size of the text, line of the text and size of the cursor. 40 point are used as default(The lambda coefficient should be 1 in this case).

#define PENSIZE (LAMBDA*2)//Cursor weight(Modification not recommended)
#define POINTSIZE (LAMBDA*40)
#define CURSORRIGHT (LAMBDA*0.015)//Cursor correction constant
#define CURSORDOWN (LAMBDA*0.12)
#define CURSORLEN (LAMBDA*0.6)
#define POINT_COR (LAMBDA*2.5)//Point correction constant
#define Y_COR (LAMBDA*0.12)//Y coordinate correction constant
#define LINEHEIGHT (LAMBDA*0.8)//Row height(determining line spacing)

void KeyboardEventProcess(int key, int event);
void CharEventProcess(int ch);
void TimerEventProcess(int timerID);
void UpdateMyTimer();//Currently a 500ms timer is used.
void DeleteOperation();//Used for VK_DELETE and VK_BACK
void MoveRight();//Cursor movement
void MoveLeft();
void CursorErase();


//These functions were static funtion in "graphics.c". Currently used as global functions.
//Accordingly the "graphics.c" file is slightly modified.(Function body unchanged.)
void DisplayText(double x, double y, char *text);
void startTimer(int id, int timeinterval);
void cancelTimer(int id, int timeinterval);
int ScaleX(double x);
int ScaleY(double y);


char text[200];
int textLen;
int textIndex = -1;
int newLine;
bool isNotDisplay;

//These variables were static variables in "graphics.c". Currently used as global variables.
//Accordingly the "graphics.c" file is slightly modified.
extern int pixelWidth, pixelHeight;
extern HDC gdc, osdc;
extern HWND graphicsWindow;
double cx = 1, cy = 13;//Text display start point. Down below there's a text width limitation(restrict the text inside the graphics window. Can be changed according to the DesiredWidth in graphics.c).

void Main()
{
	RECT bounds;

	InitGraphics();

	GetClientRect(graphicsWindow, &bounds);

	BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, BLACKNESS);//If a white background is desired, this line can be deleted.

	registerKeyboardEvent(KeyboardEventProcess);
	registerCharEvent(CharEventProcess);
	registerTimerEvent(TimerEventProcess);

	DefineColor("Gray", SHADEOFGRAY, SHADEOFGRAY, SHADEOFGRAY);
	SetPenColor("Gray");
	SetPenSize(PENSIZE);
	SetPointSize(POINTSIZE);
	SetFont(FONTNAME);

	UpdateMyTimer();
}

void UpdateMyTimer()
{
	cancelTimer(1, 500);
	isNotDisplay = 0;
	SetEraseMode(isNotDisplay);
	MovePen(cx + CURSORRIGHT, cy - CURSORDOWN);
	DrawLine(0, CURSORLEN);
	isNotDisplay = !isNotDisplay;
	startTimer(1, 500);
}

void DeleteOperation()
{
	if (textLen <= newLine || textIndex <= newLine - 1)
		return;

	char str[2] = { 0, 0 };
	str[0] = text[textIndex];
	double charWidth = TextStringWidth(str);
	cx -= charWidth;
	textIndex--;

	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = pixelWidth;
	r.bottom = pixelHeight;
	InvalidateRect(graphicsWindow, &r, FALSE);
	BitBlt(osdc, ScaleX(cx), ScaleY(cy - Y_COR), pixelWidth, -POINTSIZE - POINT_COR, osdc, ScaleX(cx + charWidth), ScaleY(cy - Y_COR), SRCCOPY);

	textLen--;
	memcpy(&text[textIndex + 1], &text[textIndex + 2], textLen - textIndex);

	UpdateMyTimer();
}

void MoveRight()
{
	if (textIndex >= textLen - 1)
		return;

	CursorErase();

	char str[2] = { 0, 0 };
	str[0] = text[textIndex + 1];
	double charWidth = TextStringWidth(str);
	cx += charWidth;
	textIndex++;

	UpdateMyTimer();
}

void MoveLeft()
{
	if (textIndex <= newLine - 1)
		return;

	CursorErase();

	char str[2] = { 0, 0 };
	str[0] = text[textIndex];
	double charWidth = TextStringWidth(str);
	cx -= charWidth;
	textIndex--;

	UpdateMyTimer();
}

void CursorErase()
{
	SetEraseMode(1);
	MovePen(cx + CURSORRIGHT, cy - CURSORDOWN);
	DrawLine(0, CURSORLEN);
	SetEraseMode(0);
}

void KeyboardEventProcess(int key, int event)
{
	switch (event)
	{
	case KEY_DOWN:
		switch (key)
		{
		case VK_BACK:
			DeleteOperation();
			break;

		case VK_DELETE:
			if (textIndex >= textLen - 1)
				return;
			MoveRight();
			DeleteOperation();
			break;

		case VK_LEFT:
			MoveLeft();
			break;

		case VK_RIGHT:
			MoveRight();
			break;

		case VK_RETURN:
			CursorErase();
			cx = 1;
			cy -= LINEHEIGHT;
			break;

		case VK_TAB:
			break;

		case VK_ESCAPE:
			InitConsole();
			text[textLen] = 0;
			printf(text);
			break;
		}
	}
}

void CharEventProcess(char ch)
{
	SetEraseMode(0);
	char str[2] = { 0, 0 };

	if (cx >= 22)//Restrict the text inside the graphics window. Can be changed according to the DesiredWidth in "graphics.c".
		return;
	switch (ch)
	{
	case '\r':
		newLine = textLen + 1;
		textIndex = textLen - 1;
		CharEventProcess('\n');
		break;
	case '\t':
		for (int i = 0; i < 4; i++)
			CharEventProcess(' ');
		break;
	case 27://ESC
		break;
	case '\b':
		break;
	default:
		CursorErase();

		textIndex++;
		memcpy(&text[textIndex + 1], &text[textIndex], textLen - textIndex);
		text[textIndex] = ch;
		textLen++;
		str[0] = ch;

		double charWidth = TextStringWidth(str);
		double textWidth = TextStringWidth(&text[textIndex]) + charWidth;
		RECT r;
		r.left = ScaleX(cx);
		r.top = ScaleY(cy - Y_COR) - (POINTSIZE + POINT_COR);
		r.right = ScaleX(cx + textWidth);
		r.bottom = ScaleY(cy - Y_COR);

		InvalidateRect(graphicsWindow, &r, FALSE);
		BitBlt(osdc, ScaleX(cx + charWidth), ScaleY(cy - Y_COR), ScaleX(textWidth - charWidth), -POINTSIZE - POINT_COR, osdc, ScaleX(cx), ScaleY(cy - Y_COR), SRCCOPY);
		BitBlt(osdc, ScaleX(cx), ScaleY(cy - Y_COR), ScaleX(charWidth), -POINTSIZE - POINT_COR, osdc, 0, 0, BLACKNESS);

		DisplayText(cx, cy, str);

		cx += TextStringWidth(str);

		UpdateMyTimer();
		break;
	}
}

void TimerEventProcess(int timerID)
{
	switch (timerID)
	{
	case 1:
		SetEraseMode(isNotDisplay);
		MovePen(cx + CURSORRIGHT, cy - CURSORDOWN);
		DrawLine(0, CURSORLEN);
		isNotDisplay = !isNotDisplay;
	default:
		break;
	}
}