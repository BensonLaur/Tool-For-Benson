/******************************************************************
	文件名：MainModuleManagerWndProc.cpp

	文件描述：主模块管理器的三个消息处理函数

*********************************************************************/
#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>
#include "MyDefine.h"

//使用全局变量 ModuleManager (主模块管理器，在GlobleManager.h中定义，在Entry.cpp中初始化)
//使用全局变量 ModuleManager2 (第二个模块管理器，在GlobleManager.h中定义，在Entry.cpp中初始化)
extern MODULEMANAGER ModuleManager,ModuleManager2;

//使用全局变量szSubWindowClassName[] (子窗口类别名，在GlobleManager.h中定义，在Entry.cpp中初始化)
extern TCHAR szSubWindowClassName[];	

//使用全局变量 quitFlag (在GlobleManager.h中定义)
extern bool quitFlag;

//用于标记签到的内容是否发生改变
extern bool isSignInChange;

//用于在 【第主模块管理器】 中指明当前 按钮选择了那个子模块
static int idOfCurrentAppModule;


/*********   在此定义各个模块的“内容窗口”的所有子窗口的【句柄】以备使用  ********/

//第一个模块 “签到” 的主要窗口句柄
HWND hSignInListBox;
static HWND hAddItemButton , hDeleteItemButton ,  
	   hStaticCurrent , hStaticCurrentItem , 
	   hSignInButton , hStatisticsGroupBox , hBaseDataButton , hHistogramButton ,hAdvanceButton;
HWND hDlgBaseData , hDlgHistogram , hDlgAdvance;

//第二个模块 “收藏” 的具体的句柄在SecondModuleManagerWndProc.cpp 中定义

//第三个模块 

/*********   在此定义各个模块所用到的【数据】  *************************************/
//第一个模块 “签到”的数据
AllSignIn allSignIn;			 //用来储存签到数据的更改，创建新签到项时，新签到项不写入pSignInListTemp，签到了之后才写入
int currentSignInSel;
static FILE *pSignInFile;
pSignIn pSignInListTemp;  //程序启动后文件读取后关闭，签到数据的更改缓冲于此，软件关闭时才更新写入
int countOfSignIn_All;
static TCHAR filePath[256];

static struct tm *pTime;
static time_t secondTime;

//第三个模块 “”的数据


//在SignInModuleSubWindow.cpp 中 初始化第一个模块的两个子窗口hDlgBaseData 和 hDlgHistogram
void InitializeSubWindow(HWND hwnd);
//在SignInModuleSubWindow.cpp 中 初始化第一个模块的 hDlgAddItem
void InitializeAddItemWindow(HWND hwnd,POINT leftTopPoint,int cyChar);

/*****************************************************************
*	模块管理器 的 底层静态“静态窗口” 使用的消息处理函数 (主模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleManagerProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PMODULEMANAGER pMM = GetPModuleManagerByStaticHWND(hwnd);
	CheckNullErrorAndQuit(pMM,6,TEXT("Can't Get ModuleManager By HWND in ModuleManagerProc()!"));
	//RECT rect;
	
	int cxClient,cyClient,cyChar,i;
	int ret1,ret2;

	switch(message)
	{
	case WM_SIZE:
			//获得系统默认字体的高度
			cyChar = HIWORD (GetDialogBaseUnits ()) ;
			cyChar*=2;

			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);

			//设置hStaticBoard的子窗口hButtonBoard和hContentBoard 的位置
			MoveWindow(ModuleManager.hButtonBoard,3,3,cxClient-2*3,cyChar+5,TRUE);

			for(i=0 ; i<ModuleManager.countOfModule ; i++)
				MoveWindow(ModuleManager.pToolModule[i].hContentBoard,3,3+cyChar+6,cxClient-2*3,cyClient-3-cyChar-6-3,TRUE);
			
			//显示当前所选定的“内容窗口”
			for(i=0; i<ModuleManager.countOfModule ;i++){
				
				if(i==idOfCurrentAppModule){
					ShowWindow(ModuleManager.pToolModule[i].hContentBoard,SW_SHOW);
				}
				else 
					ShowWindow(ModuleManager.pToolModule[i].hContentBoard,SW_HIDE);
			}

		break;

	case WM_USER+1: //用于在程序结束由主窗口的消息处理函数 中 WM_DESTROY 调用 或者 用户想要主动保存时调用
		
		  //将数据数据重新写入文件
		  if((pSignInFile = fopen(filePath,"w+b"))==NULL)
		  {
			  wsprintf(szBuffer,TEXT("签到数据无法更新！\n\n原因：\n无法打开文件:%s \n\n请检查其目录是否缺失！"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("提示"),MB_ICONERROR);
			  return -1;
		  }
		  ret1=fwrite(&countOfSignIn_All,sizeof(int),1,pSignInFile);
		  ret2=fwrite(pSignInListTemp,sizeof(SignIn),countOfSignIn_All,pSignInFile);
		  if(ret1==0 || ret2==0)
		  {
			  wsprintf(szBuffer,TEXT("签到数据无法更新！\n\n原因：\n已经打开文件:%s \n\n但是由于未知原因写入失败！"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("提示"),MB_ICONERROR);
			  return -1;
		  }
		  
		  if(fclose(pSignInFile)==EOF)
		  {
			  wsprintf(szBuffer,TEXT("签到数据更新不稳定！\n\n原因：\n已经打开并修改文件:%s \n\n但是未能正常关闭！"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("提示"),MB_ICONERROR);
			  return -1;
		  }

		  //如果要退出程序了才释放pSignInListTemp
		  if(quitFlag == true)
			free(pSignInListTemp);
			
		  return 0;
		break;
	}

	return  CallWindowProc (pMM->OldModuleManager, hwnd, message, wParam,lParam) ;
}


/*****************************************************************
*	模块管理器 的 静态“按钮窗口”使用的消息处理函数 (主模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleButtonWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
    PAINTSTRUCT ps ;
	RECT        rect;
	PMODULEMANAGER pMM = GetPModuleManagerByButtonHWND(hwnd);
	CheckNullErrorAndQuit(pMM,7,TEXT("Can't Get ModuleManager By HWND in ModuleButtonWindowProc()!"));

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
		//设置定时器，用于绘画可以移动的高亮区域
		SetTimer (hwnd, 1, 10, NULL) ;
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
			intList = (int*)malloc(sizeof(int)*ModuleManager.countOfModule);
			CheckNullErrorAndQuit(intList,13,TEXT("Memory for [intList] alloc failed in ModuleButtonWindowProc()!"));

			for(i=0 ; i< ModuleManager.countOfModule ; i++)
			{
				intList[i] = GetWindowTextLength (ModuleManager.pToolModule[i].hButton) ;
			}
	
			//第一次调用时将高亮的区域指定为第一个按钮
			newLeft = xButtonBegin;
			newRight = xButtonBegin + intList[0]*cxChar*2+30;
			
			firstTimeFlag = 0;
		}

		//根据xButtonBegin和字体宽度等设置按钮的 位置和大小
		lengthToLeft =0;
		for(i=0 ; i< ModuleManager.countOfModule ; i++)
		{
			MoveWindow(ModuleManager.pToolModule[i].hButton,
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
		idOfCurrentAppModule = idGot < MAX_COUNT_OF_MODULE ? idGot : idOfCurrentAppModule;

		if(idGot < MAX_COUNT_OF_MODULE) //为模块按钮的消息
		{
			for(i=0; i<pMM->countOfModule ;i++){
				if(i==idOfCurrentAppModule)
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_SHOW);
				else 
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_HIDE);
			}

			//计算点击按钮的位置，储存在newLeft 和 newRight中
			lengthToLeft =0;
			for(i=0 ; i< ModuleManager.countOfModule ; i++)
			{
				if(i==idOfCurrentAppModule){
					newLeft= xButtonBegin+lengthToLeft*cxChar*2+i*30;
					newRight= newLeft +  intList[i]*cxChar*2+30;
					break;
				}
			
				lengthToLeft += intList[i];
			}
		
			//确定高亮移动方向
			if(idOfCurrentAppModule > currentId) toRight =true;
			else 
				if(idOfCurrentAppModule < currentId) toRight = false;
			currentId = idOfCurrentAppModule;
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
*	每个新模块的 静态“内容窗口”使用的消息处理函数 (主模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleContentWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//得到原来的消息函数
	WNDPROC oldModuleContent = GetOldContentWndProcByContentHWND(hwnd);
	CheckNullErrorAndQuit(oldModuleContent,8,TEXT("Can't Get oldModuleContent By HWND in ModuleButtonWindowProc()!"));
	
	//得到模块的ID
	int id = GetIdInModuleManagerByContentHWND(hwnd);
	if(id == -1){
		MessageBox(hwnd,TEXT("ErrorCode: 0X 0011\nCant't Get [idModule] in ModuleContentWindowProc()"),TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
	//得到模块管理器指针
	PMODULEMANAGER pMM = GetPModuleManagerByContentHWND(hwnd);
	CheckNullErrorAndQuit(pMM,12,TEXT("Can't Get ModuleManager By ContentHWND in ModuleContentWindowProc()!"));

	static int cxWindow,cyWindow,cxChar,cyChar,i,j,cxScreen,cyScreen;
	//第一个模块“签到的变量”
	int dlgAnswer,currentIndexSel;
	static SignIn SignInTemp;
	POINT leftTopPoint;
	RECT rect;
	AllSignIn tempAllSignIn;
	bool updateTemp;


	//可以在此消息处理函数中初始化各个模块的内容
	switch(message)
	{
	case WM_CREATE:
		/** 注意：在子窗口类别化时，次新的回调函数的WM_CREATE 不会被调用，进入该函数的该消息是在
			AddNewModule()中使用 SendMessage(hContentBoard,WM_CREATE,0,0); 发送的	*/
		//初始化第一个模块内容
		if( id == 0 )
		{
				// 签到ListBox，增加、删除签到项按钮
				 hSignInListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									5,5,80,80,
									hwnd, (HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hAddItemButton = CreateWindow(TEXT("button"),TEXT("添加签到项"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									5,90,40,20,
									hwnd, (HMENU)2, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hDeleteItemButton = CreateWindow(TEXT("button"),TEXT("删除签到项"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									100,90,40,20,
									hwnd, (HMENU)3, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				 //两个静态标签，显示当前签到项
				 hStaticCurrent = CreateWindow(TEXT("static"),TEXT("当前签到项："),WS_CHILD|WS_VISIBLE|SS_LEFT,
									200,20,100,30,
									hwnd, (HMENU)4, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hStaticCurrentItem =CreateWindow(TEXT("static"),TEXT("***当前签到项***"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									200,60,200,30,
									hwnd, (HMENU)5, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
					
				 //签到按钮
				hSignInButton = CreateWindow(TEXT("button"),TEXT("今日签到"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									200,100,40,20,
									hwnd, (HMENU)6, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				 //签到统计模块
				hStatisticsGroupBox =CreateWindow(TEXT("button"),TEXT("签到统计"),WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
									250,100,80,100,
									hwnd, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
				hBaseDataButton =CreateWindow(TEXT("button"),TEXT("基本数据展示"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									255,130,70,30,
									hwnd, (HMENU)9, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				hHistogramButton =CreateWindow(TEXT("button"),TEXT("柱状图"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									255,155,70,30,
									hwnd, (HMENU)10, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
				hAdvanceButton =CreateWindow(TEXT("button"),TEXT("高级设置"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)11, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				//在SignInModuleSubWindow.cpp 中 初始化第一个模块的两个子窗口hDlgBaseData 和 hDlgHistogram
				InitializeSubWindow(hwnd);

				/** 业务数据逻辑处理部分  ***/

				GetCurrentDirectory(256,filePath);
				wsprintf(filePath,TEXT("%s\\Data"),filePath);
				printf(TEXT("mkdir:%d"),mkdir(filePath));
				wsprintf(filePath,TEXT("%s\\SignIn.dat"),filePath);


				//初始化具体的ListBox项
				
				//打开签到记录文件
				if((pSignInFile = fopen(filePath,"r+b"))==NULL)
				{
					if((pSignInFile = fopen(filePath,"w+b"))==NULL){
						wsprintf(szBuffer,TEXT("Can't Create File [%s]!"),filePath);
						CheckNullErrorAndQuit(NULL,20,szBuffer);
						exit(0);
					}
					//新建文件之后，初始化数据
					countOfSignIn_All=7;
					fwrite(&countOfSignIn_All,sizeof(int),1,pSignInFile);
					pSignInListTemp = (pSignIn)malloc(sizeof(SignIn)*countOfSignIn_All);
					wsprintf(pSignInListTemp[0].name,TEXT("Routine"));
					pSignInListTemp[0].year=2014;
					pSignInListTemp[0].month = 3;
					pSignInListTemp[0].day = 31;

					wsprintf(pSignInListTemp[1].name,TEXT("Sport"));
					pSignInListTemp[1].year=2016;
					pSignInListTemp[1].month = 4;
					pSignInListTemp[1].day = 6;

					wsprintf(pSignInListTemp[2].name,TEXT("Routine"));
					pSignInListTemp[2].year=2016;
					pSignInListTemp[2].month = 4;
					pSignInListTemp[2].day = 9;
					
					wsprintf(pSignInListTemp[3].name,TEXT("Routine"));
					pSignInListTemp[3].year=2016;
					pSignInListTemp[3].month = 4;
					pSignInListTemp[3].day = 16;
					
					wsprintf(pSignInListTemp[4].name,TEXT("Routine"));
					pSignInListTemp[4].year=2016;
					pSignInListTemp[4].month = 4;
					pSignInListTemp[4].day = 17;
					
					wsprintf(pSignInListTemp[5].name,TEXT("Sport"));
					pSignInListTemp[5].year=2016;
					pSignInListTemp[5].month = 4;
					pSignInListTemp[5].day = 14;
					
					wsprintf(pSignInListTemp[6].name,TEXT("Routine"));
					pSignInListTemp[6].year=2016;
					pSignInListTemp[6].month = 4;
					pSignInListTemp[6].day = 17;
					
					fwrite(pSignInListTemp,sizeof(SignIn),countOfSignIn_All,pSignInFile);
					
					free(pSignInListTemp);
				}
				//从头开始读取文件数据
				fseek(pSignInFile,0,SEEK_SET);
				fread(&countOfSignIn_All,sizeof(int),1,pSignInFile);

				//将文件里的数据读出，按存放于结构体AllSignIn中
				pSignInListTemp = (pSignIn)malloc(sizeof(SignIn)*countOfSignIn_All);
				fread(pSignInListTemp,sizeof(SignIn),countOfSignIn_All,pSignInFile);
				fclose(pSignInFile);

				for(i=0;i<countOfSignIn_All;i++)
				{
					if(i==0){
						
						//初始第一个项目到结构体里
						allSignIn.countOfItem =1;
						allSignIn.countInList = (int*)malloc(sizeof(int)*allSignIn.countOfItem);
						allSignIn.countInList[0]=1;
						allSignIn.SignList = (pSignIn*)malloc(sizeof(pSignIn)*allSignIn.countOfItem);
						allSignIn.SignList[0] = (pSignIn)malloc(sizeof(SignIn)*allSignIn.countInList[0]);
						allSignIn.SignList[0][0]= pSignInListTemp[i];
					}
					else
					{

						//在二维数组里查找签到项是否出现，若出现，则加入到对应的签到队列里
						for(j =0 ;j<allSignIn.countOfItem;j++)
						{
							if(lstrcmp(pSignInListTemp[i].name,allSignIn.SignList[j][0].name)==0)
							{
								allSignIn.countInList[j] += 1;
								allSignIn.SignList[j] = (pSignIn)realloc(allSignIn.SignList[j],sizeof(SignIn)*allSignIn.countInList[j]);
								allSignIn.SignList[j][allSignIn.countInList[j]-1] = pSignInListTemp[i];
								break;
							}
						}
						if(j==allSignIn.countOfItem) //若无出现，则新分配一个内存项，并更新相关数据
						{
							allSignIn.countOfItem += 1;
							allSignIn.countInList = (int*)realloc(allSignIn.countInList,sizeof(int)*allSignIn.countOfItem);
							allSignIn.countInList[allSignIn.countOfItem-1]=1;
							allSignIn.SignList = (pSignIn*)realloc(allSignIn.SignList,sizeof(pSignIn)*allSignIn.countOfItem);
							allSignIn.SignList[allSignIn.countOfItem-1] = 
								(pSignIn)malloc(sizeof(SignIn)*allSignIn.countInList[allSignIn.countOfItem-1]);
							allSignIn.SignList[allSignIn.countOfItem-1][0] = pSignInListTemp[i];
						}
					}
				}

				printf("countOfSignIn_All:%d\n",countOfSignIn_All);
				for(i=0;i<allSignIn.countOfItem;i++)
				{
					for(j=0;j<allSignIn.countInList[i];j++)
					{
						printf("N:%s %d %d %d\n",allSignIn.SignList[i][j].name,allSignIn.SignList[i][j].year,allSignIn.SignList[i][j].month,
							allSignIn.SignList[i][j].day);
					}
					printf("\n");
				}


				//根据数据更新ListBox的数据
				for(i=0;i<allSignIn.countOfItem;i++)
					SendMessage (hSignInListBox, LB_ADDSTRING, 0, (LPARAM) &allSignIn.SignList[i][0].name) ;
				//当前签到项的静态标签设为“未选择签到项”
				SetWindowText(hStaticCurrentItem,TEXT("——未选！请先选择或添加 ——"));

				currentSignInSel=-1;

		 } 
		 else if( id == 1 )//初始化第二个模块内容
		{
			/**注意:第二个模块管理器的布局 交由此【主模块管理器】静态ContentBoard 的消息处理函数 来绘制  */
			/**    而第二个模块管理器里的各个模块的布局 则由【第二个模块管理器】ModuleContentWindowProc2负责*/
		}

		break;

	case WM_SIZE:
		
		//获得“按钮窗口”的大小消息，和设备字体宽度
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;

		if( id == 0 )
		{
			//调整控件位置
			MoveWindow(hSignInListBox,15,10,240,cyWindow-2*cyChar-10-20,TRUE);
			MoveWindow(hAddItemButton,15,cyWindow-2*cyChar-10,110,2*cyChar,TRUE);
			MoveWindow(hDeleteItemButton,15+130,cyWindow-2*cyChar-10,110,2*cyChar,TRUE);
			
			MoveWindow(hStaticCurrent,300,20,200,2*cyChar,TRUE);
			MoveWindow(hStaticCurrentItem,300,20+2*cyChar+10,cxWindow-300-10,2*cyChar,TRUE);
			
			MoveWindow(hSignInButton,300,20+4*cyChar+40,180,2*cyChar,TRUE);
			MoveWindow(hStatisticsGroupBox,300+200,20+4*cyChar+40,200,200,TRUE);
			
			MoveWindow(hBaseDataButton,500+20,20+4*cyChar+40+50,160,2*cyChar,TRUE);
			MoveWindow(hHistogramButton,500+20,20+6*cyChar+40+70,160,2*cyChar,TRUE);
			MoveWindow(hAdvanceButton,500+20,20+8*cyChar+40+90,160,2*cyChar,TRUE);

		}
		else if(id == 1)
		{
			
			//设置ModuleManager2中底层hStaticBoard(第二个模块)的位置
			MoveWindow(ModuleManager2.hStaticBoard,5,5,cxWindow-10,cyWindow-10,TRUE);

			//而hStaticBoard(第二个模块)的子窗口hButtonBoard 和hContentBoard的位置 在hStaticBoard 的消息处理函数(第二个模块)中设置

		}
		else{
		}

		break;
	case WM_PAINT:


		break;

	case WM_COMMAND:
		if( id == 0 )//是第一个模块
		{
			switch(LOWORD (wParam))
			{
			case 1://hSignInListBox
				switch(HIWORD(wParam))
				{
				case LBN_SELCHANGE:
					currentSignInSel = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
					SendMessage((HWND)lParam,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
					SetWindowText(hStaticCurrentItem,szBuffer);

					break;
				}
				break;
			case 2: //hAddItemButton:
				//得到主窗口的左上角的在屏幕上的位置，并以此计算出子窗口的位置
				GetClientRect(hwnd,&rect);
				leftTopPoint.x=rect.left;
				leftTopPoint.y=rect.top;
				ClientToScreen(hwnd,&leftTopPoint);
				leftTopPoint.x += 50;
				leftTopPoint.y += 50;
				
				cxScreen = GetSystemMetrics(SM_CXSCREEN);
				cyScreen = GetSystemMetrics(SM_CYSCREEN);
				cyChar = HIWORD (GetDialogBaseUnits ()) ;

				leftTopPoint.x = min(leftTopPoint.x,cxScreen-400);
				leftTopPoint.y = min(leftTopPoint.y,cyScreen-400);

				//在SignInModuleSubWindow.cpp 中 初始化第一个模块的 hDlgAddItem
				InitializeAddItemWindow(hwnd,leftTopPoint,cyChar);
			
					
				break;
			case 3: //hDeleteItemButton:
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("未选择签到项"),TEXT("删除签到提示"),MB_ICONINFORMATION);
				}
				else{
					//由currentSignInSel 得到当前具体的签到名，再得到其对应的在 allSignIn.SignList 的第一个维度的位置
					SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
					for(i=0;i<allSignIn.countOfItem;i++)
						if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

					wsprintf(szBuffer2,TEXT("是否确认【删除】签到项： %s ？\n\n该签到项有【%d】条记录"),
						szBuffer,allSignIn.countInList[currentIndexSel]);
					dlgAnswer = MessageBox(hwnd,szBuffer2,TEXT("删除签到提示"),MB_ICONINFORMATION|MB_YESNO);

					//确认要删除的话
					if(dlgAnswer==IDYES){

						if(allSignIn.countInList[currentIndexSel]==0) updateTemp = false;
						else {
							updateTemp = true;
							//在这里只能确定有签到项时置isSignInChange为真，而无法在 上面if语句中 确定之前有没有在别处修改
							isSignInChange = true;
						}

						//更新allSignIn
						tempAllSignIn = allSignIn;
						allSignIn.countOfItem -= 1;
						allSignIn.countInList = (int*)malloc(sizeof(int)*allSignIn.countOfItem);
						allSignIn.SignList = (SignIn **)malloc(sizeof(SignIn*)*allSignIn.countOfItem);
						for(i=0,j=0;i<tempAllSignIn.countOfItem;i++)
						{
							if(i==currentIndexSel){free(tempAllSignIn.SignList[i]);continue;}//释放选中的项的内容
							allSignIn.SignList[j]=tempAllSignIn.SignList[i];//其他转移回allSignIn
						    allSignIn.countInList[j]=tempAllSignIn.countInList[i];
							j++;
						}

						if(updateTemp)//如果需要更新pSignInListTemp
						{
							printf("需要重新分配!\n");
							//重新分配pSignInListTemp
							printf("\n  countOfSignIn_All :%d ==> ",countOfSignIn_All);
							
							countOfSignIn_All=0;
							for(i=0;i<allSignIn.countOfItem;i++) countOfSignIn_All += allSignIn.countInList[i];

							printf("%d\n",countOfSignIn_All);
							pSignInListTemp = (SignIn *)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn_All);
							
							dlgAnswer = 0;	//把已经没用的 dlgAnswer 拿来作为计数器使用
							for(i=0; i<allSignIn.countOfItem ; i++)
							{
								for(j=0; j<allSignIn.countInList[i] ; j++)
								{
									lstrcpy(pSignInListTemp[dlgAnswer].name,allSignIn.SignList[i][j].name);
									pSignInListTemp[dlgAnswer].year = allSignIn.SignList[i][j].year;
									pSignInListTemp[dlgAnswer].month = allSignIn.SignList[i][j].month;
									pSignInListTemp[dlgAnswer].day = allSignIn.SignList[i][j].day;

									dlgAnswer+=1;
								}
							}
						}

for(i=0;i<countOfSignIn_All;i++)
{
	printf("n:%s  %d-%d-%d \n",pSignInListTemp[i].name,pSignInListTemp[i].year,pSignInListTemp[i].month,pSignInListTemp[i].day);
}
						//更新显示listBox	
						SendMessage(hSignInListBox,LB_DELETESTRING,currentSignInSel,0);
						//SendMessage (hSignInListBox, LB_SETCURSEL, -1, 0) ;
						
						currentIndexSel = -1;
					}
				}
				break;

			case  6://hSignInButton:
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("未选择签到项！\n\n请先选择或创建签到项！ :)"),TEXT("签到提示"),MB_ICONINFORMATION);
					break;
				}
				
				//获得当前系统的时间信息
				secondTime = time(NULL);
				pTime = localtime(&secondTime);
				//生成签到内容
				SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
				wsprintf(SignInTemp.name,"%s",szBuffer);
				SignInTemp.year = 1900 + pTime->tm_year;
				SignInTemp.month = pTime->tm_mon+1;
				SignInTemp.day = pTime->tm_mday;

				printf("------> szBuffer:SignInItemName:%s\n",szBuffer);
				
				//进行签到处理
				for(i=0;i<allSignIn.countOfItem;i++)
				{
					//循环查找所有的签到项，看是否出现过
					if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)
					{
						//如果出现过，进一步循环看具体项目是否已经签到过了
						for(j=0;j<allSignIn.countInList[i];j++)
						{
							//如果已签到，提示
							if(allSignIn.SignList[i][j].year == SignInTemp.year &&
							   allSignIn.SignList[i][j].month == SignInTemp.month &&
							   allSignIn.SignList[i][j].day == SignInTemp.day )
							{
								wsprintf(szBuffer2,TEXT("签到项：%s  %d-%d-%d 已经签过到了！"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
								MessageBox(hwnd,szBuffer2,TEXT("签到提示"),MB_ICONINFORMATION);
								break;
							}
						}
						//如果签到项未出现过
						if(j==allSignIn.countInList[i])
						{
							wsprintf(szBuffer2,TEXT("是否确认对签到项：%s  于%d-%d-%d 的签到？"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
							dlgAnswer=MessageBox(hwnd,szBuffer2,TEXT("签到提示"),MB_ICONINFORMATION|MB_YESNO);
							
							//当用户确认时，分配新的足够的内存，将签到信息写入缓冲结构allSignIn
							if(dlgAnswer==IDYES){
								allSignIn.countInList[i] += 1;
								allSignIn.SignList[i] = (pSignIn)realloc(allSignIn.SignList[i],sizeof(SignIn)*allSignIn.countInList[i]);
								allSignIn.SignList[i][allSignIn.countInList[i]-1] = SignInTemp;

								//同时更改countOfSignIn_All 和 pSignInListTemp 以备结束时写入文件
								countOfSignIn_All += 1;
								pSignInListTemp = (pSignIn)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn_All);
								pSignInListTemp[countOfSignIn_All-1] = SignInTemp;
								
								//签到成功提示
								wsprintf(szBuffer2,TEXT("签到项：%s  %d-%d-%d  签到成功！"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
								MessageBox(hwnd,szBuffer2,TEXT("签到提示"),MB_ICONINFORMATION);

								//修改全局的“修改标记”
								isSignInChange = true;
							}
						}
						break;
					}
				}

				printf("---After signIn---countOfSignIn_All:%d\n",countOfSignIn_All);
				for(i=0;i<allSignIn.countOfItem;i++)
				{
					for(j=0;j<allSignIn.countInList[i];j++)
					{
						printf("N:%s %d %d %d\n",allSignIn.SignList[i][j].name,allSignIn.SignList[i][j].year,allSignIn.SignList[i][j].month,
							allSignIn.SignList[i][j].day);
					}
					printf("\n");
				}
				printf("In Buffer SignIn\n");
				for(i=0;i<countOfSignIn_All;i++)
				{
					printf("N:%s %d %d %d\n",pSignInListTemp[i].name,pSignInListTemp[i].year,pSignInListTemp[i].month,pSignInListTemp[i].day);
				}
			
					

				break;
			case  9: //hBaseDataButton:
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("未选择签到项！\n\n请先选择或创建签到项！ :)"),TEXT("统计提示"),MB_ICONINFORMATION);
					break;
				}

				//打开前发送自定义消息，初始化窗口
				SendMessage(hDlgBaseData,WM_USER+2,0,0);

				ShowWindow(hDlgBaseData ,SW_SHOW);

				break;
			case 10: //hHistogramButton:
				
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("未选择签到项！\n\n请先选择或创建签到项！ :)"),TEXT("统计提示"),MB_ICONINFORMATION);
					break;
				}
				
				//打开前发送自定义消息，初始化窗口
				SendMessage(hDlgHistogram,WM_USER+2,0,0);

				ShowWindow(hDlgHistogram,SW_SHOW);

				break;
			case 11: //hAdvanceButton:
				
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("未选择签到项！\n\n请先选择或创建签到项！ :)"),TEXT("统计提示"),MB_ICONINFORMATION);
					break;
				}
				
				//打开前发送自定义消息，初始化窗口
				SendMessage(hDlgAdvance,WM_USER+2,0,0);

				ShowWindow(hDlgAdvance,SW_SHOW);

				break;
			}

		}
		else if(id == 2)//是第二个模块
		{

		}

		break;

	}

	return CallWindowProc (oldModuleContent, hwnd, message, wParam,lParam);
}


