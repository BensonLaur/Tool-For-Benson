/******************************************************************
	文件名：SecondModuleManagerWndProc.cpp

	文件描述：第二个模块管理器的三个消息处理函数

*********************************************************************/
#include <windows.h>
#include "MyDefine.h"

//使用全局变量 ModuleManeger2 (在GlobleManager.h中定义，在Entry.cpp中初始化)
extern MODULEMANAGER ModuleManeger2;	

//用于在 【第二个模块管理器】 中指明当前 按钮选择了那个子模块
static idOfCurrentSubModule;

//【第二个模块】 “收藏”中的 的所有句柄  
static HWND hStaticCurrentFloder,hFloderListBox, hLinkListBox,
	   hAddFloderButton, hDeleteFloderButton,
	   hAddLinkButton,  hDeleteLinkButton;
static HWND hStaticCurrentFloder2,hFloderListBox2, hLinkListBox2,
	   hAddFloderButton2, hDeleteFloderButton2,
	   hAddLinkButton2,  hDeleteLinkButton2;
static HWND hDlgAddNetLink, hDlgAddLocalLink,
       hStaticName, hStaticLink, hNameEdit1, hLinkEdit1, hConformButton1 , hCancelbutton1,
       hStaticName2, hStaticLink2, hNameEdit2, hLinkEdit2, hConformButton2 , hCancelbutton2;

/*****************************************************************
*  第二个模块下的子模块管理器的消息处理函数，包括：
*
*	ModuleManagerProc2			管理器 “底层”静态 static Board 控件的消息处理函数
*   ModuleButtonWindowProc2		管理器 “按钮”静态 static button board 控件的消息处理函数
*   ModuleContentWindowProc2	管理器 “内容”静态 static content boead 控件（们）的消息处理函数
******************************************************************/

/*****************************************************************
*  模块管理器 的 底层静态“静态窗口” 使用的消息处理函数  (第二个模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleManagerProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	PMODULEMANAGER pMM = GetPModuleManagerByStaticHWND(hwnd);
	CheckNullErrorAndQuit(pMM,14,TEXT("Can't Get ModuleManager By HWND in ModuleManagerProc2()!"));
//	RECT rect;
	int cxWindow,cyWindow,cyChar,i;		

	switch(message)
	{
		case WM_SIZE:
				//获得系统默认字体的高度
			cyChar = HIWORD (GetDialogBaseUnits ()) ;

			cxWindow = LOWORD(lParam);
			cyWindow = HIWORD(lParam);
			//设置hStaticBoard(第二个模块)的子窗口hButtonBoard 和hContentBoard的位置
			MoveWindow(ModuleManeger2.hButtonBoard,3,3,cxWindow-2*3,cyChar*2,TRUE);

			for(i=0 ; i<ModuleManeger2.countOfModule ; i++)
				MoveWindow(ModuleManeger2.pToolModule[i].hContentBoard,3,3+cyChar*2+3,cxWindow-2*3,cyWindow-3*2-3-cyChar*2,TRUE);
			
			//显示当前所选定的“内容窗口”
			for(i=0; i<ModuleManeger2.countOfModule ;i++){
				
				if(i==idOfCurrentSubModule){
					ShowWindow(ModuleManeger2.pToolModule[i].hContentBoard,SW_SHOW);
				}
				else 
					ShowWindow(ModuleManeger2.pToolModule[i].hContentBoard,SW_HIDE);
			}

			break;

	}

	return  CallWindowProc (pMM->OldModuleManager, hwnd, message, wParam,lParam) ;
}


/*****************************************************************
*  模块管理器 的 静态“按钮窗口”使用的消息处理函数 (第二个模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleButtonWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	HDC         hdc ;
    PAINTSTRUCT ps ;
	RECT        rect;
	PMODULEMANAGER pMM = GetPModuleManagerByButtonHWND(hwnd);
	CheckNullErrorAndQuit(pMM,15,TEXT("Can't Get ModuleManager By HWND in ModuleButtonWindowProc2()!"));
	static int i , totalWidth,idGot; 
	static int cxWindow,cyWindow,cxChar;
	static int* intList=NULL;
	int lengthToLeft,buttonShift;
	static int blockLeft,blockRight,newLeft,newRight,oldLeft,oldRight; //用于绘制高亮区域
	static int firstTimeFlag = 1;
	static bool toRight = true;
	static int currentId = 0;

	//"按钮窗口"的两个左右方向按钮
	static HWND hLeftButton,hRightButton;
	//记录“按钮窗口”中按钮的开始与结束位置
	static int xButtonBegin,xButtonEnd;
	
	switch(message)
	{
	case WM_CREATE:
			/** 注意：在子窗口类别化时，次新的回调函数的WM_CREATE 不会被调用，进入该函数的该消息是在
			CreateStaticModuleManergerBoard()中使用 SendMessage(pMM.hButtonBoard,WM_CREATE,0,0); 发送的	*/

		//设置定时器，用于绘画可以移动的高亮区域
		SetTimer (hwnd, 2, 10, NULL) ;
		blockLeft = blockRight = 0;
		xButtonBegin = 5;

		hLeftButton = CreateWindow(TEXT("button"),TEXT("<<"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // 使用 MAX_COUNT_OF_MODULE+1 作为向左移动按钮的ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+1), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		hRightButton = CreateWindow(TEXT("button"),TEXT(">>"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // 使用 MAX_COUNT_OF_MODULE+2 作为向右移动按钮的ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+2), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

		break;

	case WM_SIZE:
		//获得“按钮窗口”的大小消息，和设备字体宽度
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cxChar = LOWORD (GetDialogBaseUnits ()) ;

		//仅调用一次
		if(firstTimeFlag)
		{
			//将每个按钮的字体宽度信息保存在列表中
			intList = (int*)malloc(sizeof(int)*ModuleManeger2.countOfModule);
			CheckNullErrorAndQuit(intList,19,TEXT("Memory for [intList] alloc failed in ModuleButtonWindowProc2()!"));

			for(i=0 ; i< ModuleManeger2.countOfModule ; i++)
			{
				intList[i] = GetWindowTextLength (ModuleManeger2.pToolModule[i].hButton) ;
			}
	
			//第一次调用时将高亮的区域指定为第一个按钮
			newLeft = xButtonBegin;
			newRight = xButtonBegin + intList[0]*cxChar*2+30;
			
			firstTimeFlag = 0;
		}
		//根据xButtonBegin和字体宽度等设置按钮的 位置和大小
		lengthToLeft =0;
		for(i=0 ; i< ModuleManeger2.countOfModule ; i++)
		{
			MoveWindow(ModuleManeger2.pToolModule[i].hButton,
				xButtonBegin+lengthToLeft*cxChar*2+i*30,10, intList[i]*cxChar*2+30,cyWindow-10-2,TRUE);
			lengthToLeft += intList[i];
		}

		//根据xButtonBegin 和 按钮总宽度获得 xButtonEnd
		totalWidth = lengthToLeft*cxChar*2 + i*30;
		xButtonEnd = xButtonBegin + totalWidth;

		//由 xButtonBegin 、xButtonEnd 与 windows size 的关系决定“左右移动”按钮是否出现
		if(xButtonBegin >= 5 && xButtonEnd <= cxWindow-5)
		{
			ShowWindow(hLeftButton,SW_HIDE);
			ShowWindow(hRightButton,SW_HIDE);
		}
		else
		{
			MoveWindow(hLeftButton,cxWindow-5-60,1,30,9,TRUE);
			MoveWindow(hRightButton,cxWindow-5-30,1, 30,9,TRUE);
			ShowWindow(hLeftButton,SW_SHOW);
			ShowWindow(hRightButton,SW_SHOW);
		}

		//改变大小变宽时，若左侧部分隐藏，右侧又有空余，则全部按钮拉着右边框跟着移动
		if(xButtonBegin < 5 && xButtonEnd < cxWindow-5)
		{
			xButtonBegin += cxWindow- 5 - xButtonEnd;
			newLeft += cxWindow- 5 - xButtonEnd;
			newRight += cxWindow - 5-xButtonEnd;
			toRight = true;
		}


		break;

	case WM_COMMAND:
		//由按钮的句柄获得其Id 即得到所在模块pToolModle[]的下标
		idGot = GetWindowLong ((HWND)lParam, GWL_ID) ;
		idOfCurrentSubModule = idGot < MAX_COUNT_OF_MODULE ? idGot : idOfCurrentSubModule;

		if(idGot < MAX_COUNT_OF_MODULE) //为模块按钮的消息
		{
			for(i=0; i<pMM->countOfModule ;i++){
				if(i==idOfCurrentSubModule)
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_SHOW);
				else 
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_HIDE);
			}

			//计算点击按钮的位置，储存在newLeft 和 newRight中
			lengthToLeft =0;
			for(i=0 ; i< ModuleManeger2.countOfModule ; i++)
			{
				if(i==idOfCurrentSubModule){
					newLeft= xButtonBegin+lengthToLeft*cxChar*2+i*30;
					newRight= newLeft +  intList[i]*cxChar*2+30;
					break;
				}
			
				lengthToLeft += intList[i];
			}
		
			//确定高亮移动方向
			if(idOfCurrentSubModule > currentId) toRight =true;
			else 
				if(idOfCurrentSubModule < currentId) toRight = false;
			currentId = idOfCurrentSubModule;
		}
		else //为“左右移动”按钮的消息
		{
			if(idGot == MAX_COUNT_OF_MODULE + 1) //为“向左”按钮
			{
				if(xButtonEnd > cxWindow-5){
					buttonShift=xButtonEnd;
					//改变Button位置
					xButtonEnd = max( cxWindow-5, xButtonEnd-30);
					xButtonBegin = xButtonEnd - totalWidth;
					
					//改变高亮位置
					buttonShift = xButtonEnd -buttonShift;
					newLeft += buttonShift;
					newRight += buttonShift;
					toRight = false;
				}
			}
			else  //为“向右”按钮
			{
				if(xButtonBegin < 5){
					buttonShift=xButtonBegin;
					//改变Button位置
					xButtonBegin = min( 5, xButtonBegin + 30);
					xButtonEnd = xButtonBegin + totalWidth;

					//改变高亮位置
					buttonShift = xButtonBegin - buttonShift;
					newLeft += buttonShift;
					newRight += buttonShift;
					toRight = true;
				}
			}
			
			SendMessage(hwnd,WM_SIZE,0, (cyWindow<<16)|cxWindow);
			InvalidateRect(hwnd,NULL,TRUE);
		}

		break;

	case WM_TIMER:

		//更新绘画高亮区域的范围
		if(blockLeft != newLeft || blockRight != newRight)
		{
			oldLeft = blockLeft;
			oldRight = blockRight;

			blockLeft = blockLeft + (toRight ? 45:60)*(toRight ? 1:-1);
			blockRight = blockRight + (toRight ? 60:45)*(toRight ? 1:-1);
			
			blockLeft = toRight? min(blockLeft,newLeft):max(blockLeft,newLeft);
			blockRight =  toRight? min(blockRight,newRight):max(blockRight,newRight);

			oldLeft = toRight? oldLeft:blockRight;
			oldRight = toRight? blockLeft:oldRight;


			InvalidateRect(hwnd,NULL,TRUE);
		}

		break;
		
	case WM_PAINT:
        hdc = BeginPaint (hwnd, &ps) ;
		
		//FillRect(hdc,&ps.rcPaint,(HBRUSH)GetStockObject(WHITE_BRUSH));
		//绘制高亮区域
		//擦除旧的区域
		SetRect(&rect,oldLeft,1,oldRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		//绘制性的区域
		SetRect(&rect,blockLeft,1,blockRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));

         EndPaint (hwnd, &ps) ;
		break;

	}


	return CallWindowProc (pMM->OldModuleButton, hwnd, message, wParam,lParam);
}



/*****************************************************************
*  每个新模块的 静态“内容窗口”使用的消息处理函数  (第二个模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleContentWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	//得到原来的消息函数
	WNDPROC oldModuleContent = GetOldContentWndProcByContentHWND(hwnd);
	CheckNullErrorAndQuit(oldModuleContent,16,TEXT("Can't Get oldModuleContent By HWND in ModuleButtonWindowProc()!"));
	
	//得到模块的ID
	int id = GetIdInModuleManagerByContentHWND(hwnd);
	if(id == -1){
		MessageBox(hwnd,TEXT("ErrorCode: 0X 0017\nCant't Get [idModule] in ModuleContentWindowProc2()"),TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
	//得到模块管理器指针
	PMODULEMANAGER pMM = GetPModuleManagerByContentHWND(hwnd);
	CheckNullErrorAndQuit(pMM,18,TEXT("Can't Get ModuleManager By ContentHWND in ModuleContentWindowProc2()!"));


	static int cxWindow,cyWindow,cxChar,cyChar,i;

	switch(message)
	{
		
	case WM_CREATE:
		/** 注意：在子窗口类别化时，次新的回调函数的WM_CREATE 不会被调用，进入该函数的该消息是在
			AddNewModule()中使用 SendMessage(hContentBoard,WM_CREATE,0,0); 发送的	*/

		//初始化第一个子模块的控件
		if(id==0)
		{
			//显示当前文件夹的静态标签
			hStaticCurrentFloder = CreateWindow(TEXT("static"),TEXT("当前文件夹"),WS_CHILD|WS_VISIBLE|SS_CENTER,
									0,0,0,0,
									hwnd, (HMENU)1, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//两个显示当前文件夹和链接的ListBox
			hFloderListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)2,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hLinkListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)3,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//四个添加或删除按钮
			hAddFloderButton = CreateWindow(TEXT("button"),TEXT("新增文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)4, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteFloderButton = CreateWindow(TEXT("button"),TEXT("删除文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)5, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
			
			hAddLinkButton = CreateWindow(TEXT("button"),TEXT("添加链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)6, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteLinkButton = CreateWindow(TEXT("button"),TEXT("删除链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		}
		else if(id==1)//初始化第二个子模块的控件
		{
						//显示当前文件夹的静态标签
			hStaticCurrentFloder2 = CreateWindow(TEXT("static"),TEXT("当前文件夹"),WS_CHILD|WS_VISIBLE|SS_CENTER,
									0,0,0,0,
									hwnd, (HMENU)8, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//两个显示当前文件夹和链接的ListBox
			hFloderListBox2 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)9,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hLinkListBox2 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)10,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//四个添加或删除按钮
			hAddFloderButton2 = CreateWindow(TEXT("button"),TEXT("新增文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)11, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteFloderButton2 = CreateWindow(TEXT("button"),TEXT("删除文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)12, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
			
			hAddLinkButton2 = CreateWindow(TEXT("button"),TEXT("添加链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)13, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteLinkButton2 = CreateWindow(TEXT("button"),TEXT("删除链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)14, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		}
		else
		{

		}

		break;

	case WM_SIZE:
		
		//获得“内容窗口”的大小消息，和设备字体宽度
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cyChar = LOWORD (GetDialogBaseUnits ()) ;

		//调整第一个子模块的控件的位置和大小
		if(id==0)
		{
			MoveWindow(hStaticCurrentFloder,30,10,240,2*cyChar,TRUE);
			MoveWindow(hFloderListBox,30,10+3*cyChar,240,cyWindow-10*cyChar-10,TRUE);

			MoveWindow(hAddFloderButton,30,cyWindow-6*cyChar,110,4*cyChar,TRUE);
			MoveWindow(hDeleteFloderButton,30+130,cyWindow-6*cyChar,110,4*cyChar,TRUE);

			MoveWindow(hLinkListBox,30+300,15,300,cyWindow-10*cyChar-10+2*cyChar,TRUE);
			MoveWindow(hAddLinkButton,30+300,cyWindow-6*cyChar,110,4*cyChar,TRUE);
			MoveWindow(hDeleteLinkButton,30+300+190,cyWindow-6*cyChar,110,4*cyChar,TRUE);

		}
		else if(id==1)//调整第二个子模块的控件的位置和大小
		{
			MoveWindow(hStaticCurrentFloder2,30,10,240,2*cyChar,TRUE);
			MoveWindow(hFloderListBox2,30,10+3*cyChar,240,cyWindow-10*cyChar-10,TRUE);

			MoveWindow(hAddFloderButton2,30,cyWindow-6*cyChar,110,4*cyChar,TRUE);
			MoveWindow(hDeleteFloderButton2,30+130,cyWindow-6*cyChar,110,4*cyChar,TRUE);

			MoveWindow(hLinkListBox2,30+300,15,300,cyWindow-10*cyChar-10+2*cyChar,TRUE);
			MoveWindow(hAddLinkButton2,30+300,cyWindow-6*cyChar,110,4*cyChar,TRUE);
			MoveWindow(hDeleteLinkButton2,30+300+190,cyWindow-6*cyChar,110,4*cyChar,TRUE);

		}
		else
		{

		}
		break;
	}


	return CallWindowProc (oldModuleContent, hwnd, message, wParam,lParam);
}