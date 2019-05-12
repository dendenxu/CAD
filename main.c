#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

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
#define TEXTFRAME 4
#define DONTDRAW 5
#define PI 3.141592653589793
#define INITSIZE 20

#define SHADEOFGRAY 0.85//Percentage of grayc of the text. (One means white and zero means black.)(Using gray to imitate the terminal environment.)
#define FONTNAME "Bell MT"//Fontname of the displayed text. (Terminal font doesn't change with this.)(Currently only monospaced font are guaranteed to be properly displayed. If you try to use non-monospace font, the program may work properly most of the time. But for some letter like 'f', stain may occur when editing the text.)
#define LAMBDA 0.7//Lambda stands for the size of the text, line of the text and size of the cursor. 40 point are used as default(The lambda coefficient should be 1.0 in this case).

#define TEXTPENSIZE (LAMBDA*2)//Cursor weight(Modification not recommended)
#define TEXTPOINTSIZE (LAMBDA*40)
#define CURSORRIGHT (LAMBDA*0.015)//Cursor correction constant
#define CURSORDOWN (LAMBDA*0.12)
#define CURSORLEN (LAMBDA*0.55)
#define POINT_COR (LAMBDA*2.5)//Point correction constant
#define Y_COR (LAMBDA*0.15)//Y coordinate correction constant
#define LINEHEIGHT (LAMBDA*0.7)//Row height(determining line spacing)

//This is the toggle rectangle frame of an element.
typedef struct _toggle_point
{
	int select;
	double x;
	double y;
} toggle_point;

typedef struct _toggle_rect
{
	toggle_point points[9]; //There should be 8 toggle points.
	int isDisplay;			//Whether the toggle frame should be displayed.
} toggle_rect;

typedef struct _line
{
	double x;
	double y;
	double dx;
	double dy;
	toggle_rect frame;
} line;

typedef struct _rectangle
{
	double x;
	double y;
	double dx;
	double dy;
	toggle_rect frame;
} rectangle;

typedef struct _ellipse
{
	double x;
	double y;
	double rx;
	double ry;
	toggle_rect frame;
} ellipse;

typedef struct _element
{
	int index;
	int id;
	void *pointer;
} element;

typedef struct _elementp
{
	element *array;
	int size;
	int index;
} elementp;

typedef struct _textFrame
{
	rectangle *prect;
	char text[2000];
	int theX;
	double cx, cy, chx, chy;
	int isDone;
}textFrame;

extern HDC osdc;
extern int pixelWidth;
extern int pixelHeight;
extern HWND graphicsWindow;
int POINTSIZE = 10;
int PENSIZE = 3;
char *PENCOLOR = "Blue";
int isDrag;
int isDraw;
int isSelect;
int isntNew;
int isMove;
int isToggle;
int isFirstMove;
int isFrameDrawn;
int theX;
double omx, omy;
double tx, ty;

int reverse[9] = { 8,0,6,0,0,0,2,0,0 };
char text[2000];
char null[2000];
int textLen;
int textIndex = -1;
int newLine;
bool isNotDisplay;
double cx, cy; //Text display start point. Down below there's a text width limitation(restrict the text inside the graphics window. Can be changed according to the DesiredWidth in graphics.c).
double chx, chy;

//Core of this CAD program. Represents the element to be moved, toggled, deleted, or kind of element to be drawn.
element elemt = { 0, DONTDRAW, NULL };
elementp allelements = { NULL, 0, 0 };
//toggle_rect frame;
toggle_rect drawnFrame; //This is completely something used to beautify the code in DrawEverything()
int Toolbox;

void DisplayArc(double xc, double yc, double rx, double ry, double start, double sweep);
void DisplayLine(double x, double y, double dx, double dy);
void DrawTextString(string text);
void DisplayText(double x, double y, string text);
void startTimer(int id, int timeinterval);
void cancelTimer(int id);
int ScaleX(double x);
int ScaleY(double y);
double InchesY(int y);
double InchesX(int x);
int ScaleX(double x);
int ScaleY(double y);

void KeyboardEventProcess(int key, int event);
void MouseEventProcess(int x, int y, int key, int event);
void CharEventProcess(char ch);
void TimerEventProcess(int timerID);
int CheckArea(double mx, double my);   //Return whether there's an elemtect in this area. Select this elemtect if there is. Change isSelect and isMove status.
int CheckToggle(double mx, double my); //Return whether toggle point is touched. Select this toggle point if it is. Change isToggle status.

void Move(double mx, double my); //Move element around.
void MoveLine(line *pmoved, double mx, double my);
void MoveRectangle(rectangle *pmoved, double mx, double my);
void MoveEllipse(ellipse *pmoved, double mx, double my);

void Toggle(double mx, double my); //Toggle the size of selected element.
void ToggleLine(line *ptoggled, double mx, double my);
void ToggleRectangle(rectangle *ptoggled, double mx, double my);
void ToggleEllipse(ellipse *ptoggled, double mx, double my);

void Add(double mx, double my);
void AddLine(double mx, double my);
void AddRectangle(double mx, double my);
void AddEllipse(double mx, double my);
double Distans(double mx, double my, double ix, double iy);
void Delete(int index); //Delete selected element.
void DrawFrame();
void DrawTextframe(textFrame *ptextframe);
void DrawEverything();

void UpdateMyTimer();//Currently a 500ms timer is used.
void DeleteOperation();//Used for VK_DELETE and VK_BACK
void MoveRight();//Cursor movement
void MoveLeft();
void CursorErase();

void Main()
{
	InitGraphics();

	SetPenColor(PENCOLOR);
	SetPenSize(PENSIZE);
	SetPointSize(POINTSIZE);
	SetFont(FONTNAME);

	registerKeyboardEvent(KeyboardEventProcess);
	registerMouseEvent(MouseEventProcess);
	registerCharEvent(CharEventProcess);
	registerTimerEvent(TimerEventProcess);

	allelements.array = (element *)malloc(sizeof(element) * INITSIZE);
	allelements.size = INITSIZE;
}

void DrawFrame()
{
	if (!drawnFrame.isDisplay) //Don't draw if not asked to.
		return;

	//char str[2] = { 0,0 };
	double x, y;
	int array[9] = { 0, 1, 2, 5, 8, 7, 6, 3, 0 }; //defines the route of the frame drawn
	SetPenColor("Black");
	SetPenSize(1);
	for (int i = 0; i < 8; i++)
	{
		x = drawnFrame.points[array[i]].x;
		y = drawnFrame.points[array[i]].y;

		//This section draws the small square
		if (elemt.id == TEXTFRAME)
		{
			if (i == 0 || i == 2 || i == 4 || i == 6)
			{
				MovePen(x - 0.1, y - 0.1);
				DrawLine(0.2, 0);
				DrawLine(0, 0.2);
				DrawLine(-0.2, 0);
				DrawLine(0, -0.2);
				MovePen(x + 0.2, y + 0.2);
			}
		}
		else
		{
			MovePen(x - 0.05, y - 0.05);
			DrawLine(0.1, 0);
			DrawLine(0, 0.1);
			DrawLine(-0.1, 0);
			DrawLine(0, -0.1);
			MovePen(x + 0.1, y + 0.1);
		}
		//str[0] = array[i]+'0';
		//DrawTextString(str);
		MovePen(x, y);
		DrawLine(drawnFrame.points[array[i + 1]].x - x, drawnFrame.points[array[i + 1]].y - y);
	}
	SetPenColor(PENCOLOR);
	SetPenSize(PENSIZE);
}

void DrawEverything()
{
	if (allelements.index <= 0)
	{
		BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
		return;
	}

	//Draw every element in the element array
	for (int i = 0; i < allelements.index; i++)
	{
		switch (allelements.array[i].id)
		{
		case LINE:
		{
			line *pline = (line *)allelements.array[i].pointer;
			DisplayLine(pline->x, pline->y, pline->dx, pline->dy);
			break;
		}
		case RECTANGLE:
		{
			rectangle *pline = (rectangle *)allelements.array[i].pointer;
			DisplayLine(pline->x, pline->y, pline->dx, 0);
			DisplayLine(pline->x + pline->dx, pline->y, 0, pline->dy);
			DisplayLine(pline->x + pline->dx, pline->y + pline->dy, -pline->dx, 0);
			DisplayLine(pline->x, pline->y + pline->dy, 0, -pline->dy);
			break;
		}
		case ELLIPSE:
		{
			ellipse *pelli = (ellipse *)allelements.array[i].pointer;
			DisplayArc(pelli->x, pelli->y, pelli->rx, pelli->ry, 0, 360);
			break;
		}
		case TEXTFRAME:
		{
			DrawTextframe(((textFrame *)allelements.array[i].pointer));
			rectangle *pline = ((textFrame *)allelements.array[i].pointer)->prect;
			DisplayLine(pline->x, pline->y, pline->dx, 0);
			DisplayLine(pline->x + pline->dx, pline->y, 0, pline->dy);
			DisplayLine(pline->x + pline->dx, pline->y + pline->dy, -pline->dx, 0);
			DisplayLine(pline->x, pline->y + pline->dy, 0, -pline->dy);
			break;
		}
		}
	}

	//draw the toggle frame of the selected element
	if(isSelect)
	switch (elemt.id)
	{
	case LINE:
		memcpy(&drawnFrame, &((line *)elemt.pointer)->frame, sizeof(toggle_rect));
		break;
	case RECTANGLE:
		memcpy(&drawnFrame, &((rectangle *)elemt.pointer)->frame, sizeof(toggle_rect));
		break;
	case ELLIPSE:
		memcpy(&drawnFrame, &((ellipse *)elemt.pointer)->frame, sizeof(toggle_rect));
		break;
	case TEXTFRAME:
		memcpy(&drawnFrame, &((textFrame *)elemt.pointer)->prect->frame, sizeof(toggle_rect));
		break;
	default:
		return;
	}
	else
		drawnFrame.isDisplay = 0;
	DrawFrame();
}

void DrawTextframe(textFrame *ptextframe)
{
	//if (!ptextframe->isDone)
	//	return;
	int preTool = Toolbox;
	SetPenColor(PENCOLOR);
	SetPenSize(TEXTPENSIZE);
	SetPointSize(TEXTPOINTSIZE);
	SetFont(FONTNAME);

	double chx = ptextframe->chx;
	double chy = ptextframe->chy;

	char newtext[2000];
	int prevLine = 0;
	int newLine = 0;
	for (int i = 0;; i++)
	{
		if (ptextframe->text[i] == '\n')
		{
			memcpy(newtext, ptextframe->text, sizeof(text));
			prevLine = newLine?newLine+1:newLine;
			newLine = i;
			newtext[newLine] = 0;
			DisplayText(chx, chy, &newtext[prevLine]);
			chy -= LINEHEIGHT;
		}

		if(ptextframe->text[i] == 0)
		{
			memcpy(newtext, ptextframe->text, sizeof(text));
			prevLine = newLine;
			DisplayText(chx, chy, &newtext[prevLine]);
			chy -= LINEHEIGHT;

			break;
		}
	}

	SetPenColor(PENCOLOR);
	SetPenSize(PENSIZE);
	SetPointSize(POINTSIZE);
	Toolbox = preTool;
}

int CheckArea(double mx, double my)
{
	//Unselect the last selected object
	switch (elemt.id)
	{
	case LINE:
		((line *)elemt.pointer)->frame.isDisplay = 0;
		break;
	case RECTANGLE:
		((rectangle *)elemt.pointer)->frame.isDisplay = 0;
		break;
	case ELLIPSE:
		((ellipse *)elemt.pointer)->frame.isDisplay = 0;
		break;
	}

	for (int i = 0; i < allelements.index; i++)
	{
		switch (allelements.array[i].id)
		{
		case LINE:
		{
			double x, y, dx, dy;
			line *pline = (line *)allelements.array[i].pointer;
			x = pline->x;
			y = pline->y;
			dx = pline->dx;
			dy = pline->dy;

			double dt;
			dt = (fabs(dx) > fabs(dy) ? InchesX(1) : InchesY(1)) / (fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy));
			double ix, iy;
			for (double t = 0.0; t <= 1; t += dt)
			{
				ix = x + dx * t;
				iy = y + dy * t;
				double d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isSelect = 1;
					isMove = 1;
					elemt.id = LINE;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					isFirstMove = 1;
					return 1;
				}
			}
		}
		case RECTANGLE:
		{
			double x, y, dx, dy;
			rectangle *pline = (rectangle *)allelements.array[i].pointer;
			x = pline->x;
			y = pline->y;
			dx = pline->dx;
			dy = pline->dy;

			double dtx, dty;
			dtx = InchesX(1) / fabs(dx);
			dty = InchesY(1) / fabs(dy);
			double ix, iy;
			for (double t = 0.0; t <= 1; t += dtx)
			{
				ix = x + t * dx;
				iy = y;
				double d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isSelect = 1;
					isMove = 1;
					elemt.id = RECTANGLE;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					isFirstMove = 1;
					return 1;
				}
				ix = x + t * dx;
				iy = y + dy;
				d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isSelect = 1;
					isMove = 1;
					elemt.id = RECTANGLE;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					isFirstMove = 1;
					return 1;
				}
			}
			for (double t = 0.0; t <= 1; t += dty)
			{
				ix = x;
				iy = y + t * dy;
				double d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isSelect = 1;
					isMove = 1;
					elemt.id = RECTANGLE;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					isFirstMove = 1;
					return 1;
				}
				ix = x + dx;
				iy = y + t * dy;
				d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isSelect = 1;
					isMove = 1;
					elemt.id = RECTANGLE;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					isFirstMove = 1;
					return 1;
				}
			}
		}
		case ELLIPSE:
		{
			double x, y, rx, ry;
			ellipse *pelli = (ellipse *)allelements.array[i].pointer;
			x = pelli->x;
			y = pelli->y;
			rx = pelli->rx;
			ry = pelli->ry;

			double dt;
			double ix, iy;
			dt = atan2(InchesY(1), rx > ry ? rx : ry);
			for (double t = 0.0; t <= 2 * PI; t += dt)
			{
				ix = x + rx * cos(t);
				iy = y + ry * sin(t);
				double d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isSelect = 1;
					isMove = 1;
					elemt.id = ELLIPSE;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pelli->frame.isDisplay = 1;
					isFirstMove = 1;
					return 1;
				}
			}
			break; 
		}
		case TEXTFRAME:
		{
			double x, y, dx, dy;
			rectangle *pline = ((textFrame *)allelements.array[i].pointer)->prect;
			x = pline->x;
			y = pline->y;
			dx = pline->dx;
			dy = pline->dy;

			double dtx, dty;
			dtx = InchesX(1) / fabs(dx);
			dty = InchesY(1) / fabs(dy);
			double ix, iy;
			for (double t = 0.0; t <= 1; t += dtx)
			{
				ix = x + t * dx;
				iy = y;
				double d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isMove = 1;
					elemt.id = TEXTFRAME;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					return 1;
				}
				ix = x + t * dx;
				iy = y + dy;
				d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isMove = 1;
					elemt.id = TEXTFRAME;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					return 1;
				}
			}
			for (double t = 0.0; t <= 1; t += dty)
			{
				ix = x;
				iy = y + t * dy;
				double d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isMove = 1;
					elemt.id = TEXTFRAME;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					return 1;
				}
				ix = x + dx;
				iy = y + t * dy;
				d = Distans(mx, my, ix, iy);
				if (d <= 0.1)
				{
					isMove = 1;
					elemt.id = TEXTFRAME;
					elemt.index = i;
					elemt.pointer = allelements.array[i].pointer;
					pline->frame.isDisplay = 1;
					return 1;
				}
			}
		}
		}
	}
	return 0;
}

double Distans(double mx, double my, double ix, double iy)
{
	double re;
	double dx, dy;
	dx = mx - ix;
	dy = my - iy;
	re = sqrt(dx * dx + dy * dy);
	return re;
}

int CheckToggle(double mx, double my)
{
	
	//This is simply for cleanness of the code
	switch (elemt.id)
	{
	case LINE:
		//Unselect previous toggle point
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				((line *)elemt.pointer)->frame.points[3 * i + j].select = 0;
		memcpy(&drawnFrame, &((line *)elemt.pointer)->frame, sizeof(toggle_rect));
		break;
	case RECTANGLE:
		//Unselect previous toggle point
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				((rectangle *)elemt.pointer)->frame.points[3 * i + j].select = 0;
		memcpy(&drawnFrame, &((rectangle *)elemt.pointer)->frame, sizeof(toggle_rect));
		break;
	case ELLIPSE:
		//Unselect previous toggle point
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				((ellipse *)elemt.pointer)->frame.points[3 * i + j].select = 0;
		memcpy(&drawnFrame, &((ellipse *)elemt.pointer)->frame, sizeof(toggle_rect));
		break;
	}

	double x, y;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			x = drawnFrame.points[3 * i + j].x;
			y = drawnFrame.points[3 * i + j].y;
			if (3 * i + j != 4 && Distans(mx, my, x, y) <= 0.1)
			{
				isToggle = 1;
				switch (elemt.id)
				{
				case LINE:
					((line *)elemt.pointer)->frame.points[3 * i + j].select = 1;
					break;
				case RECTANGLE:
					((rectangle *)elemt.pointer)->frame.points[3 * i + j].select = 1;
					break;
				case ELLIPSE:
					((ellipse *)elemt.pointer)->frame.points[3 * i + j].select = 1;
					break;
				}
				return 1;
			}
		}

	return 0;
}

void MoveLine(line *pline, double mx, double my)
{
	double newx, newy;
	tx = pline->frame.points[0].x + (mx - omx);
	ty = pline->frame.points[0].y + (my - omy);
	newx = pline->frame.points[8].x + (mx - omx);
	newy = pline->frame.points[8].y + (my - omy);
	if (isFirstMove)
	{
		Delete(elemt.index);
		AddLine(newx, newy);
		isFirstMove = 0;
		if (isMove)
			((line *)elemt.pointer)->frame.isDisplay = 1;
		return;
	}

	AddLine(newx, newy);
	Delete(elemt.index - 1);
	elemt.index--;
	if (isMove)
		((line *)elemt.pointer)->frame.isDisplay = 1;
}
void MoveRectangle(rectangle *pline, double mx, double my)
{
	MoveLine((line *)elemt.pointer, mx, my);
	elemt.id = RECTANGLE;
	allelements.array[allelements.index - 1].id = RECTANGLE;
}

void MoveTextframe(textFrame *ptext, double mx, double my)
{
	ptext->chx += (mx - omx);
	ptext->chy += (my - omy);
	ptext->cx += (mx - omx);
	ptext->cy += (my - omy);
	ptext->prect->x += (mx - omx);
	ptext->prect->y += (my - omy);
	for (int i = 0; i < 9; i++)
	{
		ptext->prect->frame.points[i].x += (mx - omx);
		ptext->prect->frame.points[i].y += (my - omy);
	}
}

void Move(double mx, double my)
{
	switch (elemt.id)
	{
	case LINE:
		MoveLine((line *)elemt.pointer, mx, my);
		break;
	case RECTANGLE:
		MoveRectangle((rectangle *)elemt.pointer, mx, my);
		break;
	case ELLIPSE:
		MoveEllipse((ellipse *)elemt.pointer, mx, my);
		break;
	case TEXTFRAME:
		MoveTextframe((textFrame *)elemt.pointer, mx, my);
		break;
	}
}

void MoveEllipse(ellipse *pelli, double mx, double my)
{
	double newx, newy;
	tx = pelli->frame.points[0].x + (mx - omx);
	ty = pelli->frame.points[0].y + (my - omy);
	newx = pelli->frame.points[8].x + (mx - omx);
	newy = pelli->frame.points[8].y + (my - omy);
	if (isFirstMove)
	{
		Delete(elemt.index);
		AddEllipse(newx, newy);
		isFirstMove = 0;
		if (isMove)
			((line *)elemt.pointer)->frame.isDisplay = 1;
		return;
	}

	AddEllipse(newx, newy);
	Delete(elemt.index - 1);
	elemt.index--;
	if (isMove)
		((ellipse *)elemt.pointer)->frame.isDisplay = 1;
}

void Toggle(double mx, double my)
{
	switch (elemt.id)
	{
	case LINE:
		ToggleLine((line *)elemt.pointer, mx, my);
		break;
	case RECTANGLE:
		ToggleRectangle((line *)elemt.pointer, mx, my);
		break;
	case ELLIPSE:
		ToggleEllipse((ellipse *)elemt.pointer, mx, my);
		break;
	}
}

void ToggleEllipse(ellipse *pelli, double mx, double my)
{
	int selected = -1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (pelli->frame.points[3 * i + j].select)
			{
				selected = 3 * i + j;
				break;
			}
		}
	}

	double newx, newy;
	switch (selected)
	{
		//Toggle upper-left
	case 0:
		tx = pelli->frame.points[8].x;
		ty = pelli->frame.points[8].y;
		newx = mx;
		newy = my;
		break;

		//Toggle left
	case 1:
		tx = pelli->frame.points[8].x;
		ty = pelli->frame.points[8].y;
		newx = mx;
		newy = 2 * (pelli->frame.points[1].y - ty) + ty;
		break;

		//Toggle lower-right
	case 2:
		tx = pelli->frame.points[6].x;
		ty = pelli->frame.points[6].y;
		newx = mx;
		newy = my;
		break;

		//Toggle up
	case 3:
		tx = pelli->frame.points[8].x;
		ty = pelli->frame.points[8].y;
		newx = 2 * (pelli->frame.points[3].x - tx) + tx;
		newy = my;
		break;

		//Do nothing
	case 4:
		break;

		//Toggle down
	case 5:
		tx = pelli->frame.points[0].x;
		ty = pelli->frame.points[0].y;
		newx = 2 * (pelli->frame.points[5].x - tx) + tx;
		newy = my;
		break;

		//Toggle upper-right
	case 6:
		tx = pelli->frame.points[2].x;
		ty = pelli->frame.points[2].y;
		newx = mx;
		newy = my;
		break;

		//Toggle right
	case 7:
		tx = pelli->frame.points[0].x;
		ty = pelli->frame.points[0].y;
		newx = mx;
		newy = 2 * (pelli->frame.points[7].y - ty) + ty;
		break;

		//Toggle lower-right
	case 8:
		tx = pelli->frame.points[0].x;
		ty = pelli->frame.points[0].y;
		newx = mx;
		newy = my;
		break;
	}

	AddEllipse(newx, newy);
	Delete(elemt.index - 1);
	elemt.index--;
	if (isToggle)
	{
		((ellipse *)elemt.pointer)->frame.isDisplay = 1;
		((ellipse *)elemt.pointer)->frame.points[8].select = 0;
		switch (selected)
		{
		case 0:
		case 2:
		case 6:
		case 8:
			((ellipse *)elemt.pointer)->frame.points[8].select = 1;
			break;
		case 1:
		case 7:
			((ellipse *)elemt.pointer)->frame.points[7].select = 1;
			break;
		case 3:
		case 5:
			((ellipse *)elemt.pointer)->frame.points[5].select = 1;
			break;
		}
	}
}

void ToggleLine(line *pline, double mx, double my)
{
	int selected = -1;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (pline->frame.points[3 * i + j].select)
			{
				selected = 3 * i + j;
				break;
			}
		}
	}

	double newx, newy;
	switch (selected)
	{
		//Toggle upper-left
	case 0:
		tx = pline->frame.points[8].x;
		ty = pline->frame.points[8].y;
		newx = mx;
		newy = my;
		break;

		//Toggle left
	case 1:
		tx = pline->frame.points[8].x;
		ty = pline->frame.points[8].y;
		newx = mx;
		newy = 2 * (pline->frame.points[1].y - ty) + ty;
		break;

		//Toggle lower-right
	case 2:
		tx = pline->frame.points[0].x + (mx - omx);
		ty = pline->frame.points[0].y;
		newx = pline->frame.points[8].x;
		newy = pline->frame.points[8].y + (my - omy);
		break;

		//Toggle up
	case 3:
		tx = pline->frame.points[8].x;
		ty = pline->frame.points[8].y;
		newx = 2 * (pline->frame.points[3].x - tx) + tx;
		newy = my;
		break;

		//Do nothing
	case 4:
		break;

		//Toggle down
	case 5:
		tx = pline->frame.points[0].x;
		ty = pline->frame.points[0].y;
		newx = 2 * (pline->frame.points[5].x - tx) + tx;
		newy = my;
		break;

		//Toggle upper-right
	case 6:
		tx = pline->frame.points[0].x;
		ty = pline->frame.points[0].y + (my - omy);
		newx = pline->frame.points[8].x + (mx - omx);
		newy = pline->frame.points[8].y;
		break;

		//Toggle right
	case 7:
		tx = pline->frame.points[0].x;
		ty = pline->frame.points[0].y;
		newx = mx;
		newy = 2 * (pline->frame.points[7].y - ty) + ty;
		break;

		//Toggle lower-right
	case 8:
		tx = pline->frame.points[0].x;
		ty = pline->frame.points[0].y;
		newx = mx;
		newy = my;
		break;
	}

	AddLine(newx, newy);
	Delete(elemt.index - 1);
	elemt.index--;
	if (isToggle)
	{
		((line *)elemt.pointer)->frame.isDisplay = 1;
		((line *)elemt.pointer)->frame.points[8].select = 0;
		switch (selected)
		{
		case 0:
		case 8:
			((line *)elemt.pointer)->frame.points[8].select = 1;
			break;
		case 2:
			((line *)elemt.pointer)->frame.points[2].select = 1;
			break;
		case 6:
			((line *)elemt.pointer)->frame.points[6].select = 1;
			break;
		case 1:
		case 7:
			((line *)elemt.pointer)->frame.points[7].select = 1;
			break;
		case 3:
		case 5:
			((line *)elemt.pointer)->frame.points[5].select = 1;
			break;
		}
	}
}
void ToggleRectangle(rectangle *ptoggled, double mx, double my)
{
	ToggleLine((line *)elemt.pointer, mx, my);
	elemt.id = RECTANGLE;
	allelements.array[allelements.index - 1].id = RECTANGLE;
}

void Add(double mx, double my)
{
	if (isntNew) //If item exists, toggle it.
	{
		Toggle(mx, my);
		return;
	}
	if (allelements.index + 5 >= allelements.size)
	{
		void *p = allelements.array;
		allelements.array = (element *)malloc(sizeof(element) * allelements.size * 2);
		memcpy(allelements.array, p, sizeof(element) * allelements.size);
		allelements.size *= 2;
	}
	switch (elemt.id)
	{
	case LINE:
		AddLine(mx, my);
		break;
	case RECTANGLE:
		AddRectangle(mx, my);
		break;
	case ELLIPSE:
		AddEllipse(mx, my);
		break;
	case TEXTFRAME:
	{
		AddRectangle(mx, my);
		isFrameDrawn = -1;
		break;
	}
	}
}

void AddEllipse(double mx, double my)
{
	isSelect = 1;

	ellipse *pelli; //For the cleanness of code(It's actually unnecessary)
	pelli = (ellipse *)malloc(sizeof(ellipse));
	elemt.id = ELLIPSE;
	elemt.pointer = pelli;
	elemt.index = allelements.index;
	memcpy(&allelements.array[allelements.index], &elemt, sizeof(element));
	allelements.index++;

	pelli->rx = fabs((mx - tx) / 2);
	pelli->ry = fabs((my - ty) / 2);
	pelli->x = tx + (mx - tx) / 2;
	pelli->y = ty + (my - ty) / 2;

	/*
	Counting frames as
	0	3	6
	1	4	7
	2	5	8
	*/
	pelli->frame.isDisplay = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pelli->frame.points[3 * i + j].x = tx + i * (mx - tx) / 2;
			pelli->frame.points[3 * i + j].y = ty + j * (my - ty) / 2;
			pelli->frame.points[3 * i + j].select = 0;
		}
	}
	pelli->frame.points[8].select = 1; //For toggle
}

void AddLine(double mx, double my)
{
	isSelect = 1;

	line *pline;
	pline = (line *)malloc(sizeof(line));
	elemt.id = LINE;
	elemt.pointer = pline;
	elemt.index = allelements.index;
	memcpy(&allelements.array[allelements.index], &elemt, sizeof(element));
	allelements.index++;

	pline->dx = mx - tx;
	pline->dy = my - ty;
	pline->x = tx;
	pline->y = ty;

	pline->frame.isDisplay = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pline->frame.points[3 * i + j].x = tx + i * (mx - tx) / 2;
			pline->frame.points[3 * i + j].y = ty + j * (my - ty) / 2;
			pline->frame.points[3 * i + j].select = 0;
		}
	}
	pline->frame.points[8].select = 1; //For toggle
}
void AddRectangle(double mx, double my)
{
	AddLine(mx, my);
	elemt.id = RECTANGLE;
	allelements.array[allelements.index - 1].id = RECTANGLE;
}

void Delete(int index)
{
	if (elemt.id == TEXTFRAME)
		free(((textFrame*)allelements.array[index].pointer)->prect);
	free(allelements.array[index].pointer);
	memcpy(&allelements.array[index], &allelements.array[index + 1], sizeof(element) * (allelements.index - index - 1));
	allelements.index--;
	for (int i = index; i < allelements.index; i++)
		allelements.array[i].index--;
}

void UpdateMyTimer()
{
	cancelTimer(1);
	isNotDisplay = 0;
	SetEraseMode(isNotDisplay);
	MovePen(cx + CURSORRIGHT, cy - CURSORDOWN);
	DrawLine(0, CURSORLEN);
	isNotDisplay = !isNotDisplay;
	startTimer(1, 500);
}

void UpdateAll()
{
	BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
	SetPenColor(PENCOLOR);
	SetPenSize(PENSIZE);
	SetPointSize(POINTSIZE);
	DrawEverything();
	SetPenColor(PENCOLOR);
	SetPenSize(TEXTPENSIZE);
	SetPointSize(TEXTPOINTSIZE);
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

	textLen--;
	memcpy(&text[textIndex + 1], &text[textIndex + 2], textLen - textIndex);

	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = pixelWidth;
	r.bottom = pixelHeight;
	InvalidateRect(graphicsWindow, &r, FALSE);

	memcpy(((textFrame *)elemt.pointer)->text, text, sizeof(text));
	UpdateAll();
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

	memcpy(((textFrame *)elemt.pointer)->text, text, sizeof(text));
	UpdateAll();
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

	memcpy(((textFrame *)elemt.pointer)->text, text, sizeof(text));
	UpdateAll();
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
	if (Toolbox == TEXTFRAME && isFrameDrawn == 1)
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
				if (cy < ((textFrame*)elemt.pointer)->prect->frame.points[reverse[theX]].y + 0.55)
					break;
				CursorErase();
				cx = chx;
				cy -= LINEHEIGHT;
				chy -= LINEHEIGHT;
				textIndex++;
				memcpy(&text[textIndex + 1], &text[textIndex], textLen - textIndex);
				text[textIndex] = '\n';
				textLen++;
				newLine = textLen + 1;
				textIndex = textLen - 1;
				break;

			case VK_TAB:
				break;

			case VK_ESCAPE:
				CursorErase();
				textIndex = -1;
				textLen = 0;
				newLine = 0;
				Toolbox = 0;
				isFrameDrawn = 0;
				SetPenColor(PENCOLOR);
				SetPenSize(PENSIZE);
				SetPointSize(POINTSIZE);
				((textFrame *)elemt.pointer)->isDone = 1;
				memcpy(((textFrame *)elemt.pointer)->text, text, sizeof(text));
				memcpy(text, null, sizeof(text));
				cancelTimer(1);
				BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
				DrawEverything();
				return;
			}
		}
		return;
	}

	switch (event)
	{
	case KEY_DOWN:
		switch (key)
		{
		case VK_F1:
			elemt.id = LINE;
			Toolbox = LINE;
			break;
		case VK_F2:
			elemt.id = RECTANGLE;
			Toolbox = RECTANGLE;
			break;
		case VK_F3:
			elemt.id = ELLIPSE;
			Toolbox = ELLIPSE;
			break;
		case VK_F4:
			elemt.id = TEXTFRAME;
			Toolbox = TEXTFRAME;
			break;
		case VK_F5:
			Toolbox = DONTDRAW;
			isDraw = 0;
			break;
		case VK_DELETE:
			if (allelements.index <= 0 || elemt.index < 0)
				return;
			Delete(elemt.index);
			elemt.index--;
			elemt.id = -1;
			isSelect = 0;
			RECT r;
			r.left = 0;
			r.top = 0;
			r.right = pixelWidth;
			r.bottom = pixelHeight;
			InvalidateRect(graphicsWindow, &r, FALSE);
			BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
			DrawEverything();
			break;
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
			isFrameDrawn = 0;
			isDrag = 1; //This flag is to judge whether the left button is down or not
			switch (Toolbox)
			{
			case DONTDRAW:
				isDraw = 0;
				if (isSelect)
					if (CheckToggle(mx, my))
						break;
				if (CheckArea(mx, my))
					break;
				break;
			case LINE:
			case RECTANGLE:
			case ELLIPSE:
			case TEXTFRAME:
				isDraw = 1;
				isSelect = 0;
				break;
			}
			tx = omx = mx; //Track your mouse's location
			ty = omy = my;
			//In case you just select something
			BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
			DrawEverything();
			break;

		case BUTTON_UP:
			//In case you just select something
			BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
			DrawEverything();

			isDrag = 0;
			isDraw = 0;
			isntNew = 0; //So.. it is new now!
			isMove = 0;
			isToggle = 0;
			if (isFrameDrawn == -1)
			{
				isFrameDrawn = 1;
				SetPenColor(PENCOLOR);
				SetPenSize(TEXTPENSIZE);
				SetPointSize(TEXTPOINTSIZE);
				SetFont(FONTNAME);

				textFrame *ptext = (textFrame *)malloc(sizeof(textFrame));
				ptext->prect = elemt.pointer;


				elemt.id = TEXTFRAME;
				elemt.index = allelements.index - 1;
				elemt.pointer = ptext;
				memcpy(&allelements.array[allelements.index-1], &elemt, sizeof(element));

				theX = 0;
				if (((textFrame*)elemt.pointer)->prect->frame.points[8].x < ((textFrame*)elemt.pointer)->prect->frame.points[reverse[theX]].x)
						theX = 8;
				switch (theX)
				{
				case 0:
					if (((textFrame*)elemt.pointer)->prect->frame.points[2].y > ((textFrame*)elemt.pointer)->prect->frame.points[reverse[theX]].y)
						theX = 2;
					break;
				case 8:
					if (((textFrame*)elemt.pointer)->prect->frame.points[6].y > ((textFrame*)elemt.pointer)->prect->frame.points[reverse[theX]].y)
						theX = 6;
					break;
				}
				chx = cx = ((textFrame*)elemt.pointer)->prect->frame.points[theX].x + 0.15;
				chy = cy = ((textFrame*)elemt.pointer)->prect->frame.points[theX].y - 0.40;
				((textFrame *)elemt.pointer)->theX = theX;
				((textFrame *)elemt.pointer)->cx = cx;
				((textFrame *)elemt.pointer)->cy = cy;
				((textFrame *)elemt.pointer)->chx = chx;
				((textFrame *)elemt.pointer)->chy = chy;
				((textFrame *)elemt.pointer)->isDone = 0;

				UpdateMyTimer();
			}
			else
				isFrameDrawn = 0;

			break;
		}
		//Consider adding hand(mouse)-drawing function.
		break;

	case RIGHT_BUTTON:
		break;
		//Consider changing pensize.
	case MOUSEMOVE:
		if (isDrag)
		{
			BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
			if (isDraw)
			{
				Add(mx, my);
				isntNew++; //Whether to simply call the Toggle() function
			}
			else if (isToggle) //the order of the following two matters
				Toggle(mx, my);
			else if (isMove)
				Move(mx, my);

			DrawEverything();
		}
		tx = omx = mx; //Track your mouse's location(Something annoying I have encountered writing the handwriting program)
		ty = omy = my;
		break;
	}
}

void CharEventProcess(char ch)
{
	if (!(Toolbox == TEXTFRAME && isFrameDrawn == 1))
		return;
	SetEraseMode(0);
	char str[2] = { 0, 0 };

	if (cx + TextStringWidth(&text[textIndex]) >= ((textFrame*)elemt.pointer)->prect->frame.points[reverse[theX]].x - 0.30)//Restrict the text inside the graphics window. Can be changed according to the DesiredWidth in "graphics.c".
		return;
	switch (ch)
	{
	//case '\r':
	//	CharEventProcess('\n');
	//	break;

	case '\t':
		for (int i = 0; i < 4; i++)
			CharEventProcess(' ');
		break;
	case 27://ESC
		break;
	case '\b':
		break;
	//case '\n':
	//	if (cy < ((textFrame*)elemt.pointer)->prect->frame.points[reverse[theX]].y + 0.55)
	//		break;
	//	newLine = textLen + 1;
	//	textIndex = textLen - 1;
	default:
		CursorErase();

		textIndex++;
		memcpy(&text[textIndex + 1], &text[textIndex], textLen - textIndex);
		text[textIndex] = ch;
		textLen++;
		str[0] = ch;

		double charWidth = TextStringWidth(str);
		//double textWidth = TextStringWidth(&text[textIndex]) + charWidth;
		RECT r;
		r.left = 0;
		r.top = 0;
		r.right = pixelWidth;
		r.bottom = pixelHeight;
		InvalidateRect(graphicsWindow, &r, FALSE);
		cx += TextStringWidth(str);

		//BitBlt(osdc, ScaleX(chx), ScaleY(chy - Y_COR), pixelWidth, -TEXTPOINTSIZE - POINT_COR, osdc, 0, 0, WHITENESS);
		//DisplayText(chx, chy, &text[newLine]);

		//SetPenColor(PENCOLOR);
		//SetPenSize(PENSIZE);
		//SetPointSize(POINTSIZE);
		//SetEraseMode(0);
		//DrawEverything();
		//SetPenColor(PENCOLOR);
		//SetPenSize(TEXTPENSIZE);
		//SetPointSize(TEXTPOINTSIZE);
		memcpy(((textFrame *)elemt.pointer)->text, text, sizeof(text));
		BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
		SetPenColor(PENCOLOR);
		SetPenSize(PENSIZE);
		SetPointSize(POINTSIZE);
		DrawEverything();
		SetPenColor(PENCOLOR);
		SetPenSize(TEXTPENSIZE);
		SetPointSize(TEXTPOINTSIZE);

		UpdateMyTimer();
		break;
	}
}

void TimerEventProcess(int timerID)
{
	if (!(Toolbox == TEXTFRAME && isFrameDrawn))
		return;
	switch (timerID)
	{
	case 1:
		SetEraseMode(isNotDisplay);
		MovePen(cx + CURSORRIGHT, cy - CURSORDOWN);
		DrawLine(0, CURSORLEN);
		isNotDisplay = !isNotDisplay;
		SetEraseMode(0);

		break;
	}
}