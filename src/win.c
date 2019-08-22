#include "win.h"

extern HINSTANCE hInst;

int CreateWndTree(struct win_struct *root)
{
	int retval = 0;
	/* empty win_struct*/
	if(root == NULL)
		return 0;
	/* create self */
	root->hwnd = CreateWindowEx(root->dwExStyle,
			root->lpClassName,
			root->lpWindowName,
			root->dwStyle,
			root->position.reference ? root->position.x + root->position.reference->x : root->position.x,
			root->position.reference ? root->position.y + root->position.reference->y : root->position.y,
			root->position.nWidth,
			root->position.nHeight,
			root->parent ? root->parent->hwnd : NULL,
			NULL,
			hInst,
			NULL);
	if(root->hwnd == NULL)
		return GetLastError();
	/* create childs */
	int i;
	for(i = 0; i < nchild; ++i)
	{
		retval = CreateWndTree(root->childs + i);	
		if(retval)
			goto error;
	}
	return 0;
error:
	DestroyWndTree(root);
	return retval;
}

BOOL DestroyWndTree(struct win_struct *root)
{
	if(root == NULL || root->hwnd == NULL)
		return TRUE;
	return DestroyWindow(root->hwnd);
}

BOOL EnableWndTree(struct win_struct *root, BOOL bEnable)
{
	if(root == NULL || root->hwnd == NULL)
		return 1;/*disabled*/
	/* enable self */
	int i;
	for(i = 0; i < root->nchild; ++i)
	{
		EnableWndTree(root->childs + i, bEnable);
	}
	return EnableWindow(root->hwnd, bEnable);
}

