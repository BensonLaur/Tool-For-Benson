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
TCHAR szAppName[] = TEXT ("Tool For Benson") ;
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
MODULEMANAGER ModuleManeger,ModuleManeger2;	 
HMODULEMANAGER hMM,hMM2;

//���ڵ��ñ�����Ϣʱ�������Ƿ񼴽�������˳�����
bool quitFlag =false;
//���ڱ��ǩ���������Ƿ����ı�
bool isSignInChange = false;

#endif