/******************************************************************
	文件名：GlobleManager.h

	文件描述：定义整个程序的全局变量

*********************************************************************/
#ifndef GLOBLE_MANAGER_TOOL_FOR_BENSON
#define GLOBLE_MANAGER_TOOL_FOR_BENSON

#include "resource.h"
#include "MyDefine.h"

#define COUNT_OF_MODULE sizeof(ModuleName)/sizeof(ModuleName[0])
#define COUNT_OF_SUB_MODULE sizeof(CollectionModuleName)/sizeof(CollectionModuleName[0])

/***** 程序名 和 子窗口类别名（都用于在Entry.cpp 中窗口类型wndclass的创建）*******/
TCHAR szAppName[] = TEXT ("Tool_For_Benson") ;
TCHAR szSubWindowClassName[] = TEXT("subWindow");

/*********   模块的名称，需要时可以直接在下面添加名称从而 直接添加模块  ********/
TCHAR *ModuleName[] = {
TEXT("签到"),
TEXT("收藏"),
TEXT("收藏"),
TEXT("收藏收藏收藏收藏收藏收藏"),
TEXT("收藏"),
TEXT("收藏")
};
TCHAR *CollectionModuleName[] = {
TEXT("网络"),
TEXT("本地"),
TEXT("本地")
};

//模块管理器的 消息处理函数 组合
ModuleProc moduleProc[COUNT_OF_MODULE_MANAGER];

//主模块及其管理器句柄 和 第二个模块“收藏”及管理器句柄 
MODULEMANAGER ModuleManager,ModuleManager2;	 
HMODULEMANAGER hMM,hMM2;

//用于调用保存消息时，区分是否即将保存后退出程序
bool quitFlag =false;
//用于标记签到的内容是否发生改变
bool isSignInChange = false;

//用于标记当前键盘的焦点在哪一个模块
int currentKeyFocusModule =0;
//用于标记当前键盘在第一个模块的哪一个焦点
int currentKeyFocusSignIn =0;

//储存各个模块内的控件的消息处理函数,用于自定义快捷键的切换
WNDPROC oldModuleButtonWndProc[MAX_COUNT_OF_MODULE];
WNDPROC oldSignInControlWndProc[COUNT_OF_CONTROL_IN_SIGN_IN];
WNDPROC oldFavoriteButtonWndProc[3];
//储存第一个模块的空间的窗口句柄
HWND SignInControlHWND[COUNT_OF_CONTROL_IN_SIGN_IN];

#endif