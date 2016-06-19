/******************************************************************
	文件名：MyDefine.cpp

	文件描述：实现 MyDefine.h 中 除消息处理函数之外的函数

*********************************************************************/

#include <windows.h>
#include "MyDefine.h"
#include "KeyBoardControl.h"
#include <stdio.h>

/*********  函数定义  *********/

//检测 pointer 是否为空，如果为空，以对话框显示 错误码 errorCode，和错误提示信息 erroInfo ，然后退出程序
void CheckNullErrorAndQuit(void* pointer,LONG errorCode,CHAR* erroInfo)
{
	TCHAR szBuffer[256];
	wsprintf(szBuffer,TEXT("Error：%04d\n%s"),errorCode,erroInfo);
	if(pointer == NULL){
		MessageBox(NULL,szBuffer,TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
}

//初始化映射队列
void InitializeMapList()
{
	ManagerMapList.cUnit = 0;
	ManagerMapList.capacity = 5;
	ManagerMapList.list = (PMapBaseUnit)malloc(sizeof(MapBaseUnit)*ManagerMapList.capacity);
	CheckNullErrorAndQuit(ManagerMapList.list,1,TEXT("Memory for [ManagerMapList]  allocated error! "));

	ModuleMapList.cUnit = 0;
	ModuleMapList.capacity = 10;
	ModuleMapList.list = (PMapBaseUnit)malloc(sizeof(MapBaseUnit)*ModuleMapList.capacity);
	CheckNullErrorAndQuit(ManagerMapList.list,2,TEXT("Memory for [ModuleMapList]  allocated error! "));

}
void FreeMapList(){

}

//为映像队列 分配其新的唯一的句柄
HANDLE GetNewRandomHandle(MapList mapList)
{
	unsigned int  h;
	unsigned int i;
	bool repeat;
	do{ 
		repeat = false;
		h = rand()%MAX_COUNT_OF_MAP_HANDLE;
		for(i=0;i<ModuleMapList.cUnit;i++)
		{
			if(h == (unsigned int)mapList.list[i].handle ){
				repeat = true;
				break;
			}
		}
	}while(repeat);
	return (HANDLE)h;
}

//创建一个模块管理器 ModuleManager 以及其句柄 hMM
// ModuleManager 里面有 其相应的 hStaticBoard; hButtonBoard; hContentBoard;
HWND CreateStaticModuleManergerBoard(HMODULEMANAGER &hMM, MODULEMANAGER &pMM,ModuleProc &moduleProc,
									 HWND hWndParent ,HMENU hMenu,HINSTANCE hInstance)
{
	printf("In Created ModuleManager!\n");
	//创建static 控件
	pMM.hStaticBoard = CreateWindow(TEXT("static"),NULL,WS_CHILD|SS_WHITERECT|WS_VISIBLE|WS_BORDER,  
										0,0,0,0,
										hWndParent,hMenu,hInstance,NULL);

	//分配新的  模块管理器 句柄,初始化 MODULEMANAGER
	if(ManagerMapList.cUnit == 	ManagerMapList.capacity )
	{
		ManagerMapList.capacity *= 2;
		ManagerMapList.list = (PMapBaseUnit)realloc(ManagerMapList.list, sizeof(MapBaseUnit)*ManagerMapList.capacity);
		CheckNullErrorAndQuit(ManagerMapList.list,3,TEXT("Memory for [ManagerMapList]  reallocated error! "));
	}
	ManagerMapList.list[ManagerMapList.cUnit].handle = GetNewRandomHandle(ManagerMapList);
	ManagerMapList.list[ManagerMapList.cUnit].address = (LONG)&pMM;
	ManagerMapList.cUnit++;

	pMM.countOfModule = 0;
	pMM.OldModuleManager = (WNDPROC)SetWindowLong (pMM.hStaticBoard,GWL_WNDPROC, (LONG) moduleProc.ManagerProc) ;

	//在staticModuleManagerBoard 上创建 staticButtonBoard
	pMM.hButtonBoard = CreateWindow(TEXT("static"),NULL,WS_CHILD|SS_WHITERECT|WS_VISIBLE,   
										0,0,0,0,
										pMM.hStaticBoard,(HMENU)1,hInstance,NULL);
	pMM.OldModuleButton = (WNDPROC)SetWindowLong (pMM.hButtonBoard,GWL_WNDPROC, (LONG) moduleProc.ButtonWindowProc) ;

	//发送WM_CREATE消息来调用一次 新的消息处理函数 以启动"按钮窗口" 里的定时器
	SendMessage(pMM.hButtonBoard,WM_CREATE,0,0);

	//初始化模块管理器pMM中的模块等相关成员
	pMM.currentModuleCapacity = 5;
	pMM.pToolModule = (PTOOLMODULE)malloc(sizeof(TOOLMODULE) * pMM.currentModuleCapacity);
	CheckNullErrorAndQuit(pMM.pToolModule,4,TEXT("Memory for [pMM.pToolModule]  reallocated error In CreateStaticModuleManergerBoard()!"));


	//将得到的句柄传给 hMM
	hMM = (HMODULEMANAGER)ManagerMapList.list[ManagerMapList.cUnit-1].handle;

	return pMM.hStaticBoard;
}

//往 hMM对应的 管理器结构体中 添加新的子模块
void AddNewModule(HMODULEMANAGER hMM,ModuleProc &moduleProc, TCHAR *pstrName)
{
	//获得模块管理器 结构体指针
	PMODULEMANAGER pMM = GetHwndByHModuleManager(hMM);
	CheckNullErrorAndQuit(pMM,5,TEXT("Can't Get ModuleManager By HMODULEMANAGER in AddNewModule()!"));
	
	//如果模块管理器 和 映射列表 奉陪的空间已满，则重新分配更多内存
	if(pMM->countOfModule == pMM->currentModuleCapacity)
	{
		pMM->currentModuleCapacity *= 2;
		pMM->pToolModule = (PTOOLMODULE)realloc(pMM->pToolModule,sizeof(TOOLMODULE)*pMM->currentModuleCapacity);
		CheckNullErrorAndQuit(pMM->pToolModule,9,TEXT("Memory For [pMM->pToolModule] realloc error in AddNewModule()!"));
	}
	if(ModuleMapList.cUnit == ModuleMapList.capacity)
	{
		ModuleMapList.capacity *= 2;
		ModuleMapList.list = (PMapBaseUnit)realloc(ModuleMapList.list,sizeof(MapBaseUnit)*ModuleMapList.capacity);
		CheckNullErrorAndQuit(ModuleMapList.list,10,TEXT("Memory For [ModuleMapList.list] realloc error in AddNewModule()!"));
	}

	//添加新的“按钮”到 静态的“按钮窗口”，添加新的内容窗口到
	pMM->pToolModule[pMM->countOfModule].moduleName = pstrName;
	pMM->pToolModule[pMM->countOfModule].hButton = CreateWindow(TEXT("button"),pstrName,WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE,
													5+80*pMM->countOfModule,5,80,40,pMM->hButtonBoard,(HMENU)pMM->countOfModule,
																						//ID 设置为模块在数组pToolModule[]的位置下标
													hInstance2,NULL);
	//将模块按钮的消息处理函数 设置为 自定义的消息处理函数
	oldModuleButtonWndProc[pMM->countOfModule] = 
		(WNDPROC) SetWindowLong(pMM->pToolModule[pMM->countOfModule].hButton,GWL_WNDPROC,(LONG)ModuleButtonProc);

	pMM->pToolModule[pMM->countOfModule].hContentBoard = CreateWindow(TEXT("static"),NULL,WS_CHILD|SS_WHITERECT|WS_VISIBLE|WS_BORDER,
										0,0,0,0,
										pMM->hStaticBoard,(HMENU)(HMENU)pMM->countOfModule,
																						//ID 设置为模块在数组pToolModule[]的位置下标
										hInstance2,NULL);
	pMM->pToolModule[pMM->countOfModule].OldModuleContent = (WNDPROC)SetWindowLong (pMM->pToolModule[pMM->countOfModule].hContentBoard
										,GWL_WNDPROC, (LONG) moduleProc.ContentWindowProc) ;
	pMM->countOfModule++;
	
	//发送WM_CREATE消息来调用一次 新的消息处理函数 以初始化"内容窗口" 里的内容
	SendMessage(pMM->pToolModule[pMM->countOfModule-1].hContentBoard,WM_CREATE,0,0);

}

void DeleteModuleManage(HMODULEMANAGER hmm)
{

}
void DeleteModule(HMODULE hm)
{

}



//由每个模块各自static “内容窗口”的句柄得到 其对应的 模块管理器里储存的 原先的消息处理函数
WNDPROC GetOldContentWndProcByContentHWND(HWND hwnd){

	unsigned int i;
	int j;
	for(i=0;i<ManagerMapList.cUnit ; i++)
	{
		//先从管理器 句柄与 地址 映射列表中找到匹配的管理器
		if(GetParent(hwnd) == ((PMODULEMANAGER)ManagerMapList.list[i].address)->hStaticBoard)
		{
			//再从管理器里的 “内容窗口”句柄数组里找出对应的 消息处理函数
			for(j=0 ; j<((PMODULEMANAGER)ManagerMapList.list[i].address)->countOfModule ; j++)
			{
				if(hwnd == ((PMODULEMANAGER)ManagerMapList.list[i].address)->pToolModule[j].hContentBoard)
					return ((PMODULEMANAGER)ManagerMapList.list[i].address)->pToolModule[j].OldModuleContent;
			}

		}
	}
	return NULL;
}


//通过 管理器句柄hMM  得到管理器底层static 窗口的窗口句柄
PMODULEMANAGER GetHwndByHModuleManager(HMODULEMANAGER hMM)
{
	unsigned int i;
	for(i=0;i<ManagerMapList.cUnit ; i++)
	{
		if(hMM == (HMODULEMANAGER)ManagerMapList.list[i].handle)
			return (PMODULEMANAGER)ManagerMapList.list[i].address;
	}
	return NULL;
}


//由“内容窗口”的句柄 得到其所在模块的 ID
int GetIdInModuleManagerByContentHWND(HWND hwnd)
{
	int j;

	//由“内容窗口”的句柄  得到模块管理器指针
	PMODULEMANAGER pMM= GetPModuleManagerByContentHWND(hwnd);
	if(pMM != NULL)
	{
		//从管理器里的 “内容窗口”句柄数组里找出对应的 位置即为其 ID
			for(j=0 ; j<pMM->countOfModule ; j++)
			{
				if(hwnd ==pMM->pToolModule[j].hContentBoard)
					return j; 
			}
	}
	return -1;
}


//由“内容窗口”的句柄  得到模块管理器指针
PMODULEMANAGER GetPModuleManagerByContentHWND(HWND hwnd)
{
	unsigned int i;
	for(i=0;i<ManagerMapList.cUnit ; i++)
	{
		//先从管理器 句柄与 地址 映射列表中找到匹配的管理器
		if(GetParent(hwnd) == ((PMODULEMANAGER)ManagerMapList.list[i].address)->hStaticBoard)
		{
			return (PMODULEMANAGER)ManagerMapList.list[i].address;
		}
	}
	return NULL;

}

//由“按钮窗口”的句柄  得到模块管理器指针
PMODULEMANAGER GetPModuleManagerByButtonHWND(HWND hwnd)
{
	//由于 由“内容窗口”的句柄  得到模块管理器指针 中 GetParent(hwnd) 是得到父窗口句柄
	// “按钮窗口”和“内容窗口”获得模块管理器指针的逻辑是完全相同的
	return GetPModuleManagerByContentHWND(hwnd);
}

//由static board 的窗口句柄获得 其相对应的 模块管理器的指针
PMODULEMANAGER GetPModuleManagerByStaticHWND(HWND hwnd){

	unsigned int i;
	for(i=0;i<ManagerMapList.cUnit ; i++)
	{
		if(hwnd == ((PMODULEMANAGER)ManagerMapList.list[i].address)->hStaticBoard)
			return (PMODULEMANAGER)ManagerMapList.list[i].address;
	}
	return NULL;
}

/** 签到模块使用的函数 ***/
bool IsLeapYear(int year)
{
	return ((year%4==0 && year%100!=0) || (year%100==0 && year%400!=0));
}
//前一个签到必须保证时间前于后一个签到
int DaysBetween(SignIn &former,SignIn &latter)
{
	int baseYear = former.year;
	int DaysFromBase1,DaysFromBase2;
	int i,j;

	//计算前者的相对天数
	DaysFromBase1 =0;
	for(j=1; j<former.month ;j++) //计算 j=1...(former.month-1) 天数和
	{
		DaysFromBase1 += DAY_IN_MONTH[IsLeapYear(baseYear)][j-1];
	}
	DaysFromBase1 +=  former.day;

	//计算后者的相对天数
	DaysFromBase2 =0;
	for(i = baseYear ; i<latter.year ;i++)
	{
		DaysFromBase2 += IsLeapYear(i)? 366:365;
	}
	for(j=1; j<latter.month ;j++) //计算 j=1...(latter.month-1) 天数和
	{
		DaysFromBase2 += DAY_IN_MONTH[IsLeapYear(i)][j-1];
	}
	DaysFromBase2 +=  latter.day;

	return DaysFromBase2-DaysFromBase1;
}


