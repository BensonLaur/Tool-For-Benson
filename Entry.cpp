/******************************************************************
	�ļ�����Entry.cpp

	�ļ������������������ڣ��Լ����������ڵ���Ϣ������

*********************************************************************/
#include <windows.h>  
#include <stdio.h>
#include "MyDefine.h"
#include "GlobleManager.h"

//�������������н���
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"") 

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK SubWndProc(HWND, UINT, WPARAM, LPARAM);

//ʹ��ȫ�ֱ��� quitFlag (��GlobleManager.h�ж���)
extern bool quitFlag;

//���ڱ��ǩ���������Ƿ����ı�
extern bool isSignInChange;

/*********************************************************************
*		���������
*********************************************************************/

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;

	 //prepare the window class property for App Window
     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }

	 //prepare the window class property for SubWindow (including hDlgBaseData , hDlgHistogram, hDlgAddItem)
     wndclass.lpfnWndProc   = SubWndProc ;
	 wndclass.lpszClassName = szSubWindowClassName;
	      if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }

     hwnd = CreateWindow (szAppName,                  // window class name
                          TEXT ("Tool For Benson"), // window caption
                          WS_OVERLAPPEDWINDOW,        // window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          CW_USEDEFAULT,              // initial x size
                          CW_USEDEFAULT,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

/*****************************************************************
*	              �������õ���Ϣ������
******************************************************************/

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	 int cxClient,cyClient,i,MB_ICON;
	 HMENU hSignInMenu;
	 
     switch (message)
     {
     case WM_CREATE:
			//��ʼ���Զ���ľ��ӳ�����
			InitializeMapList();

			//��ʼ���˵�
			hSignInMenu = LoadMenu (((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE (IDR_MENU_SIGN_IN)) ;
			//Ĭ�ϴ�ʱΪǩ��ģ��Ĳ˵�
			SetMenu(hwnd,hSignInMenu);

			//��������ģ��������� ModuleManager �Լ����� hMM
			
			moduleProc[0].ManagerProc = ModuleManagerProc;
			moduleProc[0].ButtonWindowProc = ModuleButtonWindowProc;
			moduleProc[0].ContentWindowProc = ModuleContentWindowProc;

			// ModuleManager ������ ����Ӧ�� hStaticBoard; hButtonBoard; hContentBoard;
			CreateStaticModuleManergerBoard(hMM,ModuleManager,moduleProc[0],
				         hwnd,(HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));

			for(i=0; i <COUNT_OF_MODULE ; i++ )
			   AddNewModule(hMM,moduleProc[0],ModuleName[i]);


			//Ϊ�ڶ���ģ�鴴�����ڶ���ģ��������� ModuleManager2 �Լ����� hMM2
			
			moduleProc[1].ManagerProc = ModuleManagerProc2;
			moduleProc[1].ButtonWindowProc = ModuleButtonWindowProc2;
			moduleProc[1].ContentWindowProc = ModuleContentWindowProc2;
			
			CreateStaticModuleManergerBoard(hMM2,ModuleManager2,moduleProc[1],
				         ModuleManager.pToolModule[1].hContentBoard,(HMENU)1, //�µ�ģ��������ĸ�����Ϊ�ڶ���ģ����Ӵ���
						 (HINSTANCE)GetWindowLong(ModuleManager.pToolModule[1].hContentBoard,GWL_HINSTANCE));
			
			for(i=0; i <COUNT_OF_SUB_MODULE ; i++ )
			   AddNewModule(hMM2,moduleProc[1],CollectionModuleName[i]);

			/**ע��:�ڶ���ģ��������Ĳ��� ���ɡ���ģ�����������̬ContentBoard ����Ϣ������ ������  */
			/**    ���ڶ���ģ���������ĸ���ģ��Ĳ��� ���ɡ��ڶ���ģ���������ModuleContentWindowProc2����*/

          return 0 ;

	 case WM_SIZE:

			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			
			//����ModuleManager�еײ�hStaticBoard��λ��
			MoveWindow(ModuleManager.hStaticBoard,5,5,cxClient-10,cyClient-10,TRUE);

			//��hStaticBoard���Ӵ���hButtonBoard ��hContentBoard��λ�� ��hStaticBoard ����Ϣ������������

		 return 0;
	 case WM_INITMENUPOPUP:
		
		 if(HIWORD (lParam) == 0)//��ϵͳ�˵�
		 {
			if(LOWORD (lParam) == 0)
			{
				if(isSignInChange)
					EnableMenuItem ((HMENU)wParam, IDM_SIGN_IN_SAVE,  MF_ENABLED);
				else
					EnableMenuItem ((HMENU)wParam, IDM_SIGN_IN_SAVE,  MF_GRAYED) ;
			}

		 }


		 return 0;
	 
	 case WM_COMMAND:
			//������WM_COMMANDֻ�������Բ˵�����Ϣ
			
		 switch(LOWORD(wParam))
		 {
		 case	IDM_SIGN_IN_BACKUP:
			break;
		 case	IDM_SIGN_IN_IMPORT:
			break;
		 case	IDM_SIGN_IN_SAVE:
			 
		  quitFlag = false;
		  //��������������д���ļ�
		  //���õ�һ��ģ��������� ���ײ㡱��̬static board ����Ϣ��������ʵ������д��
		  if(SendMessage(ModuleManager.hStaticBoard,WM_USER+1,0,0)==0)
		  {
			wsprintf(szBuffer,TEXT("���³ɹ���"));
			MB_ICON = MB_ICONINFORMATION;
		  }
		  else
		  {
			wsprintf(szBuffer,TEXT("���³��������⣡"));
			MB_ICON = MB_ICONERROR;
		  }
		  MessageBox(hwnd,szBuffer,TEXT("��ʾ"),MB_ICON);

		  isSignInChange = false;

			break;
		 }


		 return 0;
     case WM_DESTROY:
		  //�ͷž�����ڴ�
		  FreeMapList();
			
		  quitFlag = true;
		  //�ڹر��ļ�֮ǰ���Ƚ�������������д���ļ�
		  //���õ�һ��ģ��������� ���ײ㡱��̬static board ����Ϣ��������ʵ������д��
		  SendMessage(ModuleManager.hStaticBoard,WM_USER+1,0,0);

          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;

}


// ��ǰ������ʾ���õ�20
