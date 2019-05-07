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

//This is the toggle rectangle frame of an element.
typedef struct _toggle_point
{
	int select;
	double x;
	double y;
}toggle_point;

typedef struct _toggle_rect
{
	toggle_point points[9];//There should be 8 toggle points.
	int isDisplay;//Whether the toggle frame should be displayed.
}toggle_rect;


//These element should be rendered as polygon for selection.
//typedef struct _point
//{
//	double x;
//	double y;
//}point;
//
//typedef struct _pointp
//{
//	point *array;
//	int index;
//	int size;
//}pointp;

typedef struct _line
{
	double x;
	double y;
	double dx;
	double dy;
	//pointp p;
	toggle_rect frame;
}line;

typedef struct _rectangle
{
	double x;
	double y;
	double l;
	double h;
	//pointp p;
	toggle_rect frame;
}rectangle;

typedef struct _ellipse
{
	double x;
	double y;
	double rx;
	double ry;
	//pointp p;
	toggle_rect frame;
}ellipse;

typedef struct _element
{
	int index;
	int id;
	void* pointer;
}element;

typedef struct _elementp
{
	element *array;
	int size;
	int index;
}elementp;

extern HDC osdc;
extern int pixelWidth;
extern int pixelHeight;
extern HWND graphicsWindow;
int POINTSIZE = 10;
int PENSIZE = 5;
char *PENCOLOR = "Gray";
int isErase;
int isDrag;
int isDraw;
int isSelect;
int isntNew;
int isMove;
int isToggle;
static int isFirstMove;
double omx, omy;
double tx, ty;

//Core of this CAD program. Represents the element to be moved, toggled, deleted, or kind of element to be drawn.
element elemt = { 0, DONTDRAW, NULL };
element slect = { 0, DONTDRAW, NULL };
elementp allelements = { NULL, 0, 0 };
toggle_rect frame;

void StartPolygon(void);
void AddSegment(int x0, int y0, int x1, int y1);
void DisplayPolygon(void);
void AddPolygonPoint(int x, int y);
double InchesY(int y);
int ScaleX(double x);
int ScaleY(double y);

void KeyboardEventProcess(int key, int event);
void MouseEventProcess(int x, int y, int key, int event);
void CharEventProcess(char ch);
int CheckArea(double mx, double my);//Return whether there's an elemtect in this area. Select this elemtect if there is. Change isSelect and isMove status.
int CheckToggle(double mx, double my);//Return whether toggle point is touched. Select this toggle point if it is. Change isToggle status.

void Move(double mx, double my);//Move element around.
void MoveLine(line *pmoved, double mx, double my);
void MoveRectangle(rectangle *pmoved, double mx, double my);
void MoveEllipse(ellipse *pmoved, double mx, double my);

void Toggle(double mx, double my);//Toggle the size of selected element.
void ToggleLine(line *ptoggled, double mx, double my);
void ToggleRectangle(rectangle *ptoggled, double mx, double my);
void ToggleEllipse(ellipse *ptoggled, double mx, double my);

void Add(double mx, double my);
//void AddPoint(pointp *pp, double xi, double yi);
void AddLine(double mx, double my);
void AddRectangle(double mx, double my);
void AddEllipse(double mx, double my);
//void RenderEllipse(ellipse *pelli);
double DistanceOfP(double mx, double my, double ix, double iy);
void Delete();//Delete selected element.
void DrawFrame();
void DrawEverything();

void DisplayArc(double xc, double yc, double rx, double ry, double start, double sweep);
void DrawTextString(string text);

void Main()
{
	InitGraphics();

	SetPenColor(PENCOLOR);
	SetPenSize(PENSIZE);
	SetPointSize(POINTSIZE);
	SetFont("Consolas");

	registerKeyboardEvent(KeyboardEventProcess);
	registerMouseEvent(MouseEventProcess);
	registerCharEvent(CharEventProcess);

	allelements.array = (element*)malloc(sizeof(element) * INITSIZE);
	allelements.size = INITSIZE;
}

void DrawFrame()
{
	double x, y;
	int array[9] = { 0,1,2,5,8,7,6,3,0 };
	//char str[2] = { 0,0 };
	SetPenColor("Black");
	SetPenSize(1);
	for (int i = 0; i < 8; i++)
	{
		x = frame.points[array[i]].x;
		y = frame.points[array[i]].y;
		MovePen(x - 0.05, y - 0.05);
		DrawLine(0.1, 0);
		DrawLine(0, 0.1);
		DrawLine(-0.1, 0);
		DrawLine(0, -0.1);
		MovePen(x + 0.1, y + 0.1);
		//str[0] = i + '0';
		//DrawTextString(str);

		MovePen(x, y);
		DrawLine(frame.points[array[i + 1]].x - x, frame.points[array[i + 1]].y - y);
	}
	SetPenColor(PENCOLOR);
	SetPenSize(PENSIZE);
}

void DrawEverything()
{
	//RECT r;
	//r.left = 0;
	//r.top = 0;
	//r.right = pixelWidth;
	//r.bottom = pixelHeight;
	//InvalidateRect(graphicsWindow, &r, FALSE);
	//if (isErase)
	//	SetPenColor("White");
	//else
	//	SetPenColor(PENCOLOR);
	for (int i = 0; i < allelements.index; i++)
	{
		//StartPolygon();
		switch (allelements.array[i].id)
		{
		case LINE:
		case RECTANGLE:
		case ELLIPSE:
		{
			double x, y, rx, ry;
			ellipse *pelli = ((ellipse*)allelements.array[i].pointer);
			x = pelli->x;
			y = pelli->y;
			rx = pelli->rx;
			ry = pelli->ry;
			//MovePen(x + rx, y);
			DisplayArc(x, y, rx, ry, 0, 360);
			break;
		}

		/*	for (int j = 0; j < ((ellipse*)allelements.array[i].pointer)->p.index; j++)
			{
				int scalex, scaley;
				scalex = ScaleX(((ellipse*)allelements.array[i].pointer)->p.array[j].x);
				scaley = ScaleY(((ellipse*)allelements.array[i].pointer)->p.array[j].y);
				AddPolygonPoint(scalex, scaley);
			}*/
		}
		//DisplayPolygon();
	}
	if (frame.isDisplay)
		DrawFrame();
}

int CheckArea(double mx, double my)
{
	for (int i = 0; i < allelements.index; i++)
	{
		switch (allelements.array[i].id)
		{
		case LINE:
		case RECTANGLE:
		case ELLIPSE:
			((ellipse *)allelements.array[i].pointer)->frame.isDisplay = 0;
			break;
		}
	}

	for (int i = 0; i < allelements.index; i++)
	{
		switch (allelements.array[i].id)
		{
		case LINE:
		case RECTANGLE:
		case ELLIPSE:
		{
			double x, y, rx, ry;
			ellipse *pelli;
			pelli = ((ellipse*)allelements.array[i].pointer);
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
				double d = DistanceOfP(mx, my, ix, iy);
				if (d <= 0.2)
				{
					isSelect = 1;
					isMove = 1;
					elemt.id = ELLIPSE;
					elemt.index = i + 1;
					elemt.pointer = allelements.array[i].pointer;
					slect.id = ELLIPSE;
					slect.index = i;
					slect.pointer = allelements.array[i].pointer;
					((ellipse *)elemt.pointer)->frame.isDisplay = 1;
					isFirstMove = 1;
					memcpy(&frame, &pelli->frame, sizeof(toggle_rect));

					return 1;
				}
			}
		}
		break;
		}
	}
	return 0;
}

double DistanceOfP(double mx, double my, double ix, double iy)
{
	double re;
	double dx, dy;
	dx = mx - ix;
	dy = my - iy;
	re = sqrt(dx*dx + dy * dy);
	return re;
}

int CheckToggle(double mx, double my)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			frame.points[3 * i + j].select = 0;
			((ellipse *)slect.pointer)->frame.points[3 * i + j].select = 0;
		}
	}

	double x, y;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			x = frame.points[3 * i + j].x;
			y = frame.points[3 * i + j].y;
			if (3 * i + j != 4 && DistanceOfP(mx, my, x, y) <= 0.3)
			{
				isToggle = 1;
				switch (slect.id)
				{
				case LINE:
				case RECTANGLE:
				case ELLIPSE:
					((ellipse *)slect.pointer)->frame.points[3 * i + j].select = 1;
					elemt.id = ELLIPSE;
					memcpy(&frame, &((ellipse *)slect.pointer)->frame, sizeof(toggle_rect));
					break;
				}
				return 1;
			}
		}
	}

	return 0;
}
void AddLine(double mx, double my)
{

}
void AddRectangle(double mx, double my)
{

}
void MoveLine(line *pmoved, double mx, double my)
{

}
void MoveRectangle(rectangle *pmoved, double mx, double my)
{

}
void ToggleLine(line *ptoggled, double mx, double my)
{

}
void ToggleRectangle(rectangle *ptoggled, double mx, double my)
{

}

void Move(double mx, double my)
{
	switch (elemt.id)
	{
	case LINE:
		MoveLine((line*)elemt.pointer, mx, my);
		break;
	case RECTANGLE:
		MoveRectangle((rectangle*)elemt.pointer, mx, my);
		break;
	case ELLIPSE:
		MoveEllipse((ellipse*)elemt.pointer, mx, my);
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
		Delete();
		AddEllipse(newx, newy);
		isFirstMove = 0;
		return;
	}
	AddEllipse(newx, newy);
	if (isMove)
		((ellipse *)elemt.pointer)->frame.isDisplay = 1;
	memcpy(&frame, &((ellipse *)elemt.pointer)->frame, sizeof(toggle_rect));
	Delete();
}

void Toggle(double mx, double my)
{
	switch (elemt.id)
	{
	case LINE:
		ToggleLine((line*)elemt.pointer, mx, my);
		break;
	case RECTANGLE:
		ToggleRectangle((rectangle*)elemt.pointer, mx, my);
		break;
	case ELLIPSE:
		ToggleEllipse((ellipse*)elemt.pointer, mx, my);
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
	if (isToggle)
	{
		((ellipse *)elemt.pointer)->frame.isDisplay = 1;
		((ellipse *)elemt.pointer)->frame.points[8].select = 0;
		((ellipse *)elemt.pointer)->frame.points[selected].select = 1;
	}
	memcpy(&frame, &((ellipse *)elemt.pointer)->frame, sizeof(toggle_rect));
	Delete();

	switch (selected)
	{
	case 0:
	case 2:
	case 6:
		memcpy(&slect, &elemt, sizeof(element));
		CheckToggle(mx, my);
	}

	switch (selected)
	{
	case 1:
	case 3:
		memcpy(&slect, &elemt, sizeof(element));
		CheckToggle(mx, my);
	}
}

void Add(double mx, double my)
{
	if (isntNew)//If item exists, toggle it.
	{
		Toggle(mx, my);
		return;
	}
	if (allelements.index + 5 >= allelements.size)
	{
		void *p = allelements.array;
		allelements.array = (element *)malloc(sizeof(element) *allelements.size * 2);
		memcpy(allelements.array, p, sizeof(element)*allelements.size);
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
	}
}

//void AddPoint(pointp *pp, double xi, double yi)
//{
//	if (pp->index+10 >= pp->size)
//	{
//		realloc(pp->array, pp->size * 2);
//		pp->size *= 2;
//	}
//	pp->array[pp->index].x = xi;
//	pp->array[pp->index].y = yi;
//	pp->index++;
//}

void AddEllipse(double mx, double my)
{
	isSelect = 1;

	ellipse *pelli;
	pelli = (ellipse *)malloc(sizeof(ellipse));
	elemt.id = ELLIPSE;
	elemt.pointer = pelli;
	elemt.index = allelements.index;
	allelements.array[allelements.index].id = ELLIPSE;
	allelements.array[allelements.index].pointer = pelli;
	allelements.array[allelements.index].index = allelements.index++;

	pelli->rx = fabs((mx - tx) / 2);
	pelli->ry = fabs((my - ty) / 2);
	pelli->x = tx + (mx - tx) / 2;
	pelli->y = ty + (my - ty) / 2;
	//pelli->p.array = (point*)malloc(sizeof(point) * 200);
	//pelli->p.size = 200;
	//pelli->p.index = 0;
	//RenderEllipse(pelli);

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
	pelli->frame.points[8].select = 1;//For toggle
	memcpy(&frame, &((ellipse *)elemt.pointer)->frame, sizeof(toggle_rect));

}

//void RenderEllipse(ellipse *pelli)
//{
//	double dt, xi = pelli->x, yi = pelli->y;
//
//	dt = atan2(InchesY(5), (pelli->rx >= pelli->ry) ? pelli->rx : pelli->ry);
//	for (double t = 0.0; t < 2 * PI; t += dt)
//	{
//		if (t > 2 * PI - dt / 2)
//			t = 2 * PI;
//		xi = pelli->x + pelli->rx * cos(t);
//		yi = pelli->y + pelli->ry * sin(t);
//		AddPoint(&pelli->p, xi, yi);
//	}
//}

void Delete()
{
	free(allelements.array[elemt.index - 1].pointer);
	memcpy(&allelements.array[elemt.index - 1], &allelements.array[elemt.index], sizeof(element)*(allelements.index - elemt.index));
	allelements.index--;
	for (int i = elemt.index - 1; i < allelements.index; i++)
		allelements.array[i].index--;
	elemt.index--;
}


void KeyboardEventProcess(int key, int event)
{
	switch (event)
	{
	case KEY_DOWN:
		switch (key)
		{
		case VK_F1:
			elemt.id = LINE;
			break;
		case VK_F2:
			elemt.id = RECTANGLE;
			break;
		case VK_F3:
			elemt.id = ELLIPSE;
			break;
		case VK_F4:
			elemt.id = TEXTFRAME;
			break;
		case VK_F5:
			elemt.id = DONTDRAW;
			isDraw = 0;
			break;
		case DELETE:
			Delete();
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
			isDrag = 1;
			switch (elemt.id)
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
				isDraw = 1;
				break;
			}
			tx = omx = mx;
			ty = omy = my;
			break;

		case BUTTON_UP:
			switch (slect.id)
			{
			case LINE:
			case RECTANGLE:
			case ELLIPSE:
				for (int i = 0; i < 3; i++)
					for (int j = 0; j < 3; j++)
					{
						((ellipse *)elemt.pointer)->frame.points[3 * i + j].select = 0;
						frame.points[3 * i + j].select = 0;
					}
				break;
			}
			memcpy(&slect, &elemt, sizeof(element));
			BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);
			DrawEverything();
			isDrag = 0;
			isDraw = 0;
			if (isMove || isToggle)
				elemt.id = DONTDRAW;
			isMove = 0;
			isToggle = 0;
			isntNew = 0;
			break;

			//Consider adding hand(mouse)-drawing function.
		}
		break;

	case RIGHT_BUTTON:
		break;
		//Consider changing pensize.
	case MOUSEMOVE:
		if (isDrag)
		{
			BitBlt(osdc, 0, 0, pixelWidth, pixelHeight, osdc, 0, 0, WHITENESS);//If a white background is desired, this line can be deleted.
			//isErase = 1;
			//DrawEverything();
			//isErase = 0;
			if (isDraw && !isMove && !isToggle)
			{
				Add(mx, my);
				isntNew++;
			}
			else if (isToggle)
			{
				memcpy(&slect, &elemt, sizeof(element));
				Toggle(mx, my);
			}
			else if (isMove)
				Move(mx, my);

			DrawEverything();
		}
		tx = omx = mx;
		ty = omy = my;
		break;
	}
}

void CharEventProcess(char ch)
{
	//Consider using already written TEXTFRAME program(in homework4).
}