/******************************************************************
	�ļ�����DlgResourceWndProc.cpp

	�ļ�������ʵ����������� �� �Ի�����Դ����Ϣ������

*********************************************************************/
#include <windows.h>
#include "MyDefine.h"


//�Ի�����Դ ���õ��� �Ի�����Ϣ���������˴�������������ֻȷ�ϵĶԻ���
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
