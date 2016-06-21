/******************************************************************
	�ļ�����GlobleManager.h

	�ļ��������������������ȫ�ֱ���

*********************************************************************/
#ifndef GLOBLE_MANAGER_TOOL_FOR_BENSON
#define GLOBLE_MANAGER_TOOL_FOR_BENSON

#include "resource.h"
#include "MyDefine.h"

#define COUNT_OF_MODULE sizeof(ModuleName)/sizeof(ModuleName[0])
#define COUNT_OF_SUB_MODULE sizeof(CollectionModuleName)/sizeof(CollectionModuleName[0])

/***** ������ �� �Ӵ������������������Entry.cpp �д�������wndclass�Ĵ�����*******/
TCHAR szAppName[] = TEXT ("Tool_For_Benson") ;
TCHAR szSubWindowClassName[] = TEXT("subWindow");

/*********   ģ������ƣ���Ҫʱ����ֱ��������������ƴӶ� ֱ�����ģ��  ********/
TCHAR *ModuleName[] = {
TEXT("ǩ��"),
TEXT("�ղ�"),
TEXT("�ղ�"),
TEXT("�ղ��ղ��ղ��ղ��ղ��ղ�"),
TEXT("�ղ�"),
TEXT("�ղ�")
};
TCHAR *CollectionModuleName[] = {
TEXT("����"),
TEXT("����"),
TEXT("����")
};

//ģ��������� ��Ϣ������ ���
ModuleProc moduleProc[COUNT_OF_MODULE_MANAGER];

//��ģ�鼰���������� �� �ڶ���ģ�顰�ղء������������ 
MODULEMANAGER ModuleManager,ModuleManager2;	 
HMODULEMANAGER hMM,hMM2;

//���ڵ��ñ�����Ϣʱ�������Ƿ񼴽�������˳�����
bool quitFlag =false;
//���ڱ��ǩ���������Ƿ����ı�
bool isSignInChange = false;

//���ڱ�ǵ�ǰ���̵Ľ�������һ��ģ��
int currentKeyFocusModule =0;
//���ڱ�ǵ�ǰ�����ڵ�һ��ģ�����һ������
int currentKeyFocusSignIn =0;

//�������ģ���ڵĿؼ�����Ϣ������,�����Զ����ݼ����л�
WNDPROC oldModuleButtonWndProc[MAX_COUNT_OF_MODULE];
WNDPROC oldSignInControlWndProc[COUNT_OF_CONTROL_IN_SIGN_IN];
WNDPROC oldFavoriteButtonWndProc[3];
//�����һ��ģ��Ŀռ�Ĵ��ھ��
HWND SignInControlHWND[COUNT_OF_CONTROL_IN_SIGN_IN];

#endif