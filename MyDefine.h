/******************************************************************
	文件名：MyDefine.h

	文件描述：定义自定义的常量和结构体，以及声明主要的消息处理函数

*********************************************************************/
#ifndef MY_DEFINE_TOOL_FOR_BENSON
#define MY_DEFINE_TOOL_FOR_BENSON

/*********  常量定义  *********/
#define COUNT_OF_MODULE_MANAGER 2
#define MAX_COUNT_OF_MODULE 20
#define MAX_LEN_OF_MODULE_NAME 64
#define MAX_COUNT_OF_MAP_HANDLE 1000 

#define LBS_STANDARD_NO_SORT          (LBS_NOTIFY | WS_VSCROLL | WS_BORDER)


/*********  类型定义  *********/

//定义 模块 类型
typedef struct _TOOLMODULE{
TCHAR *moduleName;		//模块名
LONG pModuldeManager;							//所属模块管理器(使用时强制转换为PMODULEMANAGER 类型)
HWND hButton;
HWND hContentBoard;								//显示内容的静态窗口
WNDPROC OldModuleContent;						//用于ContentBoard原先的消息处理
}TOOLMODULE,*PTOOLMODULE;

//定义模块句柄
DECLARE_HANDLE(HTOOLMODULE);

//定义 模块管理器 类型
typedef struct _MODULEMANAGER{
int countOfModule;								//当前的模块数
//HTOOLMODULE hToolModule[MAX_COUNT_OF_MODULE];   //模块的句柄
PTOOLMODULE pToolModule;						//模块的数组指针

int currentModuleCapacity;						//存放当前备用的已分配内存的模块数

WNDPROC OldModuleManager;						//最底窗口原先的消息处理函数
WNDPROC OldModuleButton;						//按钮窗口原先的消息处理函数

HWND hStaticBoard;								//最底的窗口句柄
HWND hButtonBoard;								//按钮窗口句柄
}MODULEMANAGER,*PMODULEMANAGER;

//定义模块管理器句柄
DECLARE_HANDLE(HMODULEMANAGER);


//定义句柄的映射单元
typedef struct _MapBaseUnit{
HANDLE handle;
LONG address;
}MapBaseUnit,*PMapBaseUnit;

//定义映射队列结构
typedef struct _MapList{
PMapBaseUnit list;
unsigned int cUnit;
unsigned int capacity;
}MapList,*PMapList;

//每个模块管理器要有不同的消息处理函数
typedef struct _ModuleProc
{
WNDPROC ManagerProc;
WNDPROC ButtonWindowProc;
WNDPROC ContentWindowProc;

}ModuleProc,*PModuleProc;

//储存各个模块内的控件的消息处理函数，


/*********   在此定义各个模块使用的文件数据结构体  ********/
//第一个模块
//文件单元结构
typedef struct _SignIn{
TCHAR name[256];
int year;
int month;
int day;
} SignIn,*pSignIn;
//记录缓冲结构
typedef struct _AllSignIn{
int countOfItem;
SignIn ** SignList;  //二维的签到记录，第一维的个数为countOfItem ，第二维的个数分别存放于countList
int * countInList;
} AllSignIn;


/*********  内定变量声明  *********/
//定义程序全局静态变量

//用于函数AddNewModule中，添加新的“按钮”到 静态的“按钮窗口”时，创建窗口使用的 运行实例参数
//在这里程序没有对其初始化，但按钮窗口还是能创建
static HINSTANCE hInstance2;


//定义两个列表，用于管理全局的句柄
static MapList ManagerMapList;
static MapList ModuleMapList;

//用于全局经常会用到的wsprintf等函数，主要用于暂存字符串
static TCHAR szBuffer[256];
static TCHAR szBuffer2[256];

//定义用于计算签到间隔的 数组，用于 签到模块使用的函数 DaysBetween 
static int DAY_IN_MONTH[2][12] = {
	{31,28,31,30,31,30,31,31,30,31,30,31},
	{31,29,31,30,31,30,31,31,30,31,30,31}
};


/*********  函数定义  *********/


//检测 pointer 是否为空，如果为空，以对话框显示 错误码 errorCode，和错误提示信息 erroInfo ，然后退出程序
void CheckNullErrorAndQuit(void* pointer,LONG errorCode,CHAR* erroInfo);

//初始化映射队列
void InitializeMapList();

void FreeMapList();

//为映像队列 分配其新的唯一的句柄
HANDLE GetNewRandomHandle(MapList mapList);

//创建一个模块管理器 ModuleManager 以及其句柄 hMM
// ModuleManager 里面有 其相应的 hStaticBoard; hButtonBoard; hContentBoard;
HWND CreateStaticModuleManergerBoard(HMODULEMANAGER &hMM, MODULEMANAGER &pMM,ModuleProc &moduleProc,
									 HWND hWndParent ,HMENU hMenu,HINSTANCE hInstance);

//往hMM对应的 管理器结构体中 添加新的子模块
void AddNewModule(HMODULEMANAGER hMM,ModuleProc &moduleProc, TCHAR *pstrName);

void DeleteModuleManage(HMODULEMANAGER hmm);
void DeleteModule(HMODULE hm);


//由 每个模块各自static “内容窗口”的句柄得到 其对应的 模块管理器里储存的 原先的消息处理函数
WNDPROC GetOldContentWndProcByContentHWND(HWND hwnd);


//通过管理器句柄hMM  得到管理器底层static 窗口的窗口句柄
PMODULEMANAGER GetHwndByHModuleManager(HMODULEMANAGER hMM);


//由“内容窗口”的句柄 得到其所在模块的 ID
int GetIdInModuleManagerByContentHWND(HWND hwnd);


//由“内容窗口”的句柄  得到模块管理器指针
PMODULEMANAGER GetPModuleManagerByContentHWND(HWND hwnd);
//由“按钮窗口”的句柄  得到模块管理器指针
PMODULEMANAGER GetPModuleManagerByButtonHWND(HWND hwnd);
//由static board 的句柄获得 其相对应的 模块管理器的指针
PMODULEMANAGER GetPModuleManagerByStaticHWND(HWND hwnd);

/** 签到模块使用的函数 ***/
bool IsLeapYear(int year);
int DaysBetween(SignIn &former,SignIn &latter);


/**
*  消息处理函数  
*/
//主模块管理器的消息处理函数

//模块管理器 的 底层静态“静态窗口” 使用的消息处理函数 (主模块管理器)
LRESULT CALLBACK ModuleManagerProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//模块管理器 的 静态“按钮窗口”使用的消息处理函数 (主模块管理器)
LRESULT CALLBACK ModuleButtonWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//每个新模块的 静态“内容窗口”使用的消息处理函数 (主模块管理器)
LRESULT CALLBACK ModuleContentWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//第二个模块下的子模块管理器的消息处理函数

//模块管理器 的 底层静态“静态窗口” 使用的消息处理函数  (第二个模块管理器)
LRESULT CALLBACK ModuleManagerProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//模块管理器 的 静态“按钮窗口”使用的消息处理函数  (第二个模块管理器)
LRESULT CALLBACK ModuleButtonWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//每个新模块的 静态“内容窗口”使用的消息处理函数  (第二个模块管理器)
LRESULT CALLBACK ModuleContentWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//第一个模块中所有出现的Edit控件的新的消息处理函数
LRESULT CALLBACK newEditProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif
