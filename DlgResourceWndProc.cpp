/******************************************************************
	文件名：DlgResourceWndProc.cpp

	文件描述：实现整个程序的 的 对话框资源的消息处理函数

*********************************************************************/
#include <windows.h>
#include "MyDefine.h"


//对话框资源 所用到的 对话框消息处理函数，此处理函数用于所有只确认的对话框
BOOL  CALLBACK ConformDlgProc (HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)      
{
           switch (message)
			{
        
           case   WM_INITDIALOG :
        
                  return TRUE ;
        
           case   WM_COMMAND :
        
                  switch (LOWORD (wParam))
                  {
        
                  case   IDOK :
                                EndDialog (hDlg, 0) ;
						 return TRUE ;
                  }
        
                  break ;
           }
    return FALSE ;
}
