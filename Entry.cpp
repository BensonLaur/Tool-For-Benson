/******************************************************************
	文件名：Entry.cpp

	文件描述：整个程序的入口，以及程序主窗口的消息处理函数

*********************************************************************/
#include <windows.h>  
#include <stdio.h>
#include "MyDefine.h"
#include "GlobleManager.h"

//设置启用命令行界面
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"") 

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK SubWndProc(HWND, UINT, WPARAM, LPARAM);


/*********************************************************************
*		主函数入口
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
*	              主窗口用的消息处理函数
******************************************************************/

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	 int cxClient,cyClient,i;
	 
     switch (message)
     {
     case WM_CREATE:
			//初始化自定义的句柄映射队列
			InitializeMapList();

			//创建【主模块管理器】 ModuleManager 以及其句柄 hMM
			
			moduleProc[0].ManagerProc = ModuleManagerProc;
			moduleProc[0].ButtonWindowProc = ModuleButtonWindowProc;
			moduleProc[0].ContentWindowProc = ModuleContentWindowProc;

			// ModuleManager 里面有 其相应的 hStaticBoard; hButtonBoard; hContentBoard;
			CreateStaticModuleManergerBoard(hMM,ModuleManeger,moduleProc[0],
				         hwnd,(HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));

			for(i=0; i <COUNT_OF_MODULE ; i++ )
			   AddNewModule(hMM,moduleProc[0],ModuleName[i]);


			//为第二个模块创建【第二个模块管理器】 ModuleManager2 以及其句柄 hMM2
			
			moduleProc[1].ManagerProc = ModuleManagerProc2;
			moduleProc[1].ButtonWindowProc = ModuleButtonWindowProc2;
			moduleProc[1].ContentWindowProc = ModuleContentWindowProc2;
			
			CreateStaticModuleManergerBoard(hMM2,ModuleManeger2,moduleProc[1],
				         ModuleManeger.pToolModule[1].hContentBoard,(HMENU)1, //新的模块管理器的父窗口为第二个模块的子窗口
						 (HINSTANCE)GetWindowLong(ModuleManeger.pToolModule[1].hContentBoard,GWL_HINSTANCE));
			
			for(i=0; i <COUNT_OF_SUB_MODULE ; i++ )
			   AddNewModule(hMM2,moduleProc[1],CollectionModuleName[i]);

			/**注意:第二个模块管理器的布局 交由【主模块管理器】静态ContentBoard 的消息处理函数 来绘制  */
			/**    而第二个模块管理器里的各个模块的布局 则由【第二个模块管理器】ModuleContentWindowProc2负责*/

          return 0 ;

	 case WM_SIZE:

			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			
			//设置ModuleManeger中底层hStaticBoard的位置
			MoveWindow(ModuleManeger.hStaticBoard,5,5,cxClient-10,cyClient-10,TRUE);

			//而hStaticBoard的子窗口hButtonBoard 和hContentBoard的位置 在hStaticBoard 的消息处理函数中设置

		 return 0;

     case WM_DESTROY:
		  //释放句柄的内存
		  FreeMapList();
			
		  //在关闭文件之前，先将数据数据重新写入文件
		  //调用第一各模块管理器的 “底层”静态static board 的消息处理函数来实现数据写入
		  SendMessage(ModuleManeger.hStaticBoard,WM_USER+1,0,0);

          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}


// 当前错误提示，用到20
