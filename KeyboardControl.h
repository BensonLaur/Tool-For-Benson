/******************************************************************
	文件名：KeyBoardControl.h

	文件描述：处理实现整个程序所有的 键盘切换 功能

*********************************************************************/
#include <windows.h>
#include <stdio.h>

extern MODULEMANAGER ModuleManager;
extern WNDPROC oldModuleButtonWndProc[MAX_COUNT_OF_MODULE];
extern WNDPROC oldSignInControlWndProc[COUNT_OF_CONTROL_IN_SIGN_IN];
extern WNDPROC oldFavoriteButtonWndProc[3];
extern HWND SignInControlHWND[COUNT_OF_CONTROL_IN_SIGN_IN];
extern int currentKeyFocusModule;
extern int currentKeyFocusSignIn;

LRESULT CALLBACK ModuleButtonProc (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	int id = GetWindowLong (hwnd, GWL_ID) ;
	switch (message)
    {
           case WM_KEYDOWN :
        
                  if (wParam == VK_RIGHT||wParam == VK_TAB)
				  {
					  SetFocus(ModuleManager.pToolModule[++id%ModuleManager.countOfModule].hButton);
					  //由于父窗口Button window 只是获取 按钮的wParam 中的句柄，所以只需要保证wParam 为句柄即可
					  SendMessage(ModuleManager.hButtonBoard,WM_COMMAND,
						  0,(LONG)ModuleManager.pToolModule[id%ModuleManager.countOfModule].hButton);
					  currentKeyFocusModule = id%ModuleManager.countOfModule;
				  }
				  else if(wParam == VK_LEFT)
				  {
				      SetFocus(ModuleManager.pToolModule[--id%ModuleManager.countOfModule].hButton);
					  //由于父窗口Button window 只是获取 按钮的wParam 中的句柄，所以只需要保证wParam 为句柄即可
					  SendMessage(ModuleManager.hButtonBoard,WM_COMMAND,
						  0,(LONG)ModuleManager.pToolModule[id%ModuleManager.countOfModule].hButton);
					  currentKeyFocusModule = id%ModuleManager.countOfModule;
				  }
				  else if(wParam == VK_DOWN)
				  {
					  //如果是第一个模块
					  if(id==0)
					  {
						  SetFocus(SignInControlHWND[0]);
						  currentKeyFocusSignIn = 0;
					  }
				  }
        
                  break ;
     }
     return CallWindowProc (oldModuleButtonWndProc[id], hwnd, message, wParam,lParam) ;
        
}


LRESULT CALLBACK NormalButtonProc (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	int buttonID ;
	buttonID = GetWindowLong(hwnd,GWL_ID);
	switch(buttonID)
	{
	case 2: // hAddItemButton 对应的currentKeyFocusSignIn = 1
		currentKeyFocusSignIn = 1;
		break;
	case 3: // hDeleteItemButton 对应的currentKeyFocusSignIn = 2
		currentKeyFocusSignIn = 2;
		break;
	case 6: // hSignInButton 对应的currentKeyFocusSignIn = 3
		currentKeyFocusSignIn = 3;
		break;
	case 9: // hBaseDataButton 对应的currentKeyFocusSignIn = 4
		currentKeyFocusSignIn = 4;
		break;
	case 10: // hHistogramButton 对应的currentKeyFocusSignIn = 5
		currentKeyFocusSignIn = 5;
		break;
	case 11: // hAdvanceButton 对应的currentKeyFocusSignIn = 6
		currentKeyFocusSignIn = 6;
		break;
	}

	switch (message)
    {
           case WM_KEYDOWN :
        
                  if (wParam == VK_TAB )
				  {
					  if(currentKeyFocusModule == 0)
					  {
						  SetFocus(SignInControlHWND[++currentKeyFocusSignIn%COUNT_OF_CONTROL_IN_SIGN_IN]);
						  currentKeyFocusSignIn = currentKeyFocusSignIn%COUNT_OF_CONTROL_IN_SIGN_IN;
					  }
				  }
				  else if(wParam == VK_UP)
				  {
						SetFocus(ModuleManager.pToolModule[currentKeyFocusModule].hButton);
				  }
				  else if(wParam == VK_RETURN)
				  {
					  printf("Return press! currentKeyFocusModule=%d buttonId=%d\n",currentKeyFocusModule,buttonID);
					  //第一个模块的对按钮发送的消息只处理其LOWORD (wParam)里面代表的按钮的窗口ID，
						SendMessage(ModuleManager.pToolModule[currentKeyFocusModule].hContentBoard,WM_COMMAND,
						  (LONG)buttonID,0);
				  }

                  break ;
     }

	if(currentKeyFocusModule == 0)
	{
		return CallWindowProc (oldSignInControlWndProc[currentKeyFocusSignIn], hwnd, message, wParam,lParam) ;
	}
}


LRESULT CALLBACK ListBoxProc (HWND hwnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	switch (message)
    {
           case WM_KEYDOWN :
        
                  if (wParam == VK_TAB)
				  {
					  if(currentKeyFocusModule == 0)
					  {
						  SetFocus(SignInControlHWND[++currentKeyFocusSignIn%COUNT_OF_CONTROL_IN_SIGN_IN]);
						  currentKeyFocusSignIn = currentKeyFocusSignIn%COUNT_OF_CONTROL_IN_SIGN_IN;
					  }
				  }
                  break ;
     }

	if(currentKeyFocusModule == 0)
		return CallWindowProc (oldSignInControlWndProc[0], hwnd, message, wParam,lParam) ;
	else 
		return 0;
}


