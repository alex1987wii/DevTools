#ifndef _WIN_H_
#define _WIN_H_

#include <windows.h>

struct Position{
	struct Position *reference;
	int x;
	int y;
	int nWidth;
	int nHeight;
};

struct win_struct
{
	struct win_struct *parent;
	HWND	hwnd;
	LPCSTR	lpClassName;
	LPCSTR	lpWindowName;
	DWORD	dwExStyle;
	DWORD	dwStyle;
	struct Position position;
	int nchild;
	struct win_struct *childs;
};

int  CreateWndTree(struct win_struct *root);
BOOL DestroyWndTree(struct win_struct *root);
BOOL EnableWndTree(struct win_struct *root, BOOL bEnable);

#define ARRAY_SIZE(arr)		(sizeof(arr)/sizeof((arr)[0]))

#endif
