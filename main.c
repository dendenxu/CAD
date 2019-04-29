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

//id number
#define LINE 1
#define RECTANGLE 2
#define ELLIPSE 3
#define TEXT 4
#define DONTDRAW 5

//This is the toggle rectangle frame of an element.
typedef struct _toggle_point
{
	double x;
	double y;
	int select;
}toggle_point;
typedef struct _toggle_rect
{
	toggle_point* points[8];//There should be 8 toggle points.
	int isDisplay;//Whether the toggle frame should be displayed.
}toggle_rect;


//These element should be rendered as polygon for selection.
typedef struct _point
{
	double x;
	double y;
}point;

typedef struct _line
{
	int id;
	double x;
	double y;
	double dx;
	double dy;
	point* p;
	toggle_rect frame;
}line;

typedef struct _rectangle
{
	int id;
	double x;
	double y;
	double l;
	double h;
	point* p;
	toggle_rect frame;
}rectangle;

typedef struct _ellipse
{
	int id;
	double rx;
	double ry;
	double start;
	double sweep;
	point* p;
	toggle_rect frame;
}ellipse;

typedef struct _element
{
	int id;
	void* pointer;
}element;

int POINTSIZE = 40;
int PENSIZE = 3;
int isDrag;
int isDraw;
int isSelect;
int isMove;
int isToggle;

//Core of this CAD program. Represents the element to be moved, toggled, deleted, or kind of element to be drawn.
element elemt = {DONTDRAW, NULL};



void KeyboardEventProcess(int key, int event);
void MouseEventProcess(int x, int y, int key, int event);
void CharEventProcess(int ch);
int CheckArea();//Return whether there's an elemtect in this area. Select this elemtect if there is. Change isSelect and isMove status.
int CheckToggle();//Return whether toggle point is touched. Select this toggle point if it is. Change isToggle status.

void Move(element elemt, double mx, double my);//Move element around.
void MoveLine(line moved, double mx, double my);
void MoveRectangle(rectangle moved, double mx, double my);
void MoveEllipse(ellipse moved, double mx, double my);

void Toggle(element elemt, double mx, double my);//Toggle the size of selected element.
void ToggleLine(line toggled, double mx, double my);
void ToggleRectangle(rectangle toggled, double mx, double my);
void ToggleEllipse(ellipse toggled, double mx, double my);

void Add(element elemt, double mx, double my);//When adding an element, consider using Toggle() functions.
void AddLine(line added, double mx, double my);
void AddRectangle(rectangle added, double mx, double my);
void AddEllipse(ellipse added, double mx, double my);

void Delete(element elemt);//Delete selected element.
void DeleteLine(line deleted);
void DeleteRectangle(rectangle deleted);
void DeleteEllipse(ellipse deleted);

void DrawEverything();

void Main()
{
	InitGraphics();

	SetPenColor("Gray");
	SetPenSize(PENSIZE);
	SetPointSize(POINTSIZE);

	registerKeyboardEvent(KeyboardEventProcess);
	registerMouseEvent(MouseEventProcess);
	registerCharEvent(CharEventProcess);
}

void Move(element elemt, double mx, double my)
{
	switch (elemt.id)
	{
	case LINE:
		MoveLine(*(line*)elemt.pointer, mx, my);
		break;
	case RECTANGLE:
		MoveRectangle(*(rectangle*)elemt.pointer, mx, my);
		break;
	case ELLIPSE:
		MoveEllipse(*(ellipse*)elemt.pointer, mx, my);
		break;
	}
}

void Toggle(element elemt, double mx, double my)
{
	switch (elemt.id)
	{
	case LINE:
		ToggleLine(*(line*)elemt.pointer, mx, my);
		break;
	case RECTANGLE:
		ToggleRectangle(*(rectangle*)elemt.pointer, mx, my);
		break;
	case ELLIPSE:
		ToggleEllipse(*(ellipse*)elemt.pointer, mx, my);
		break;
	}
}

void Add(element elemt, double mx, double my)
{
	switch (elemt.id)
	{
	case LINE:
		AddLine(*(line*)elemt.pointer, mx, my);
		break;
	case RECTANGLE:
		AddRectangle(*(rectangle*)elemt.pointer, mx, my);
		break;
	case ELLIPSE:
		AddEllipse(*(ellipse*)elemt.pointer, mx, my);
		break;
	}
}

void Delete(element elemt)
{
	switch (elemt.id)
	{
	case LINE:
		DeleteLine(*(line*)elemt.pointer);
		break;
	case RECTANGLE:
		DeleteRectangle(*(rectangle*)elemt.pointer);
		break;
	case ELLIPSE:
		DeleteEllipse(*(ellipse*)elemt.pointer);
		break;
	}
}

void KeyboardEventProcess(int key, int event)
{
	switch (event)
	{
	case KEY_DOWN:
		switch (key)
		{
		case VK_F1:
			elemt.id= LINE;
		case VK_F2:
			elemt.id = RECTANGLE;
		case VK_F3:
			elemt.id = ELLIPSE;
		case VK_F4:
			elemt.id = TEXT;
		case VK_F5:
			elemt.id = DONTDRAW;
		case DELETE:
			Delete(elemt);
		//More keyborad function to be added.
		}
	}
}

void MouseEventProcess(int x, int y, int key, int event)
{
	double mx = ScaleXInches(x);
	double my = ScaleYInches(y);
	switch (key)
	{
	case LEFT_BUTTON:
		switch (event)
		{
		case BUTTON_DOWN:
			isDrag = 1;
			switch (elemt.id)
			{
			case DONTDRAW:
				isDraw = 0;
				if (CheckArea())
					break;
				if (isSelect)
					if (CheckToggle())
						break;
				break;
			case LINE:
			case RECTANGLE:
			case ELLIPSE:
				isDraw = 1;
				break;
			}

		case MOUSEMOVE:
			if (isDrag)
				if (isDraw)
					Add(elemt, mx, my);
				else if (isMove)
					Move(elemt, mx, my);
				else if (isToggle)
					Toggle(elemt, mx, my);
			DrawEverything();
			break;

		case BUTTON_UP:
			isDrag = 0;
			isDraw = 0;
			break;

			//Consider adding hand(mouse)-drawing function.
		}
	case RIGHT_BUTTON:
		//Consider changing pensize.
	}
}

void CharEventProcess(int ch)
{
	//Consider using already written Text program(in homework4).
}