/******************************************************************
	�ļ�����MyDefine.cpp

	�ļ�������ʵ�� MyDefine.h �� ����Ϣ������֮��ĺ���

*********************************************************************/

#include <windows.h>
#include "MyDefine.h"
#include "KeyBoardControl.h"
#include <stdio.h>

/*********  ��������  *********/

//��� pointer �Ƿ�Ϊ�գ����Ϊ�գ��ԶԻ�����ʾ ������ errorCode���ʹ�����ʾ��Ϣ erroInfo ��Ȼ���˳�����
void CheckNullErrorAndQuit(void* pointer,LONG errorCode,CHAR* erroInfo)
{
	TCHAR szBuffer[256];
	wsprintf(szBuffer,TEXT("Error��%04d\n%s"),errorCode,erroInfo);
	if(pointer == NULL){
		MessageBox(NULL,szBuffer,TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
}

//��ʼ��ӳ�����
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

//Ϊӳ����� �������µ�Ψһ�ľ��
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

//����һ��ģ������� ModuleManager �Լ����� hMM
// ModuleManager ������ ����Ӧ�� hStaticBoard; hButtonBoard; hContentBoard;
HWND CreateStaticModuleManergerBoard(HMODULEMANAGER &hMM, MODULEMANAGER &pMM,ModuleProc &moduleProc,
									 HWND hWndParent ,HMENU hMenu,HINSTANCE hInstance)
{
	printf("In Created ModuleManager!\n");
	//����static �ؼ�
	pMM.hStaticBoard = CreateWindow(TEXT("static"),NULL,WS_CHILD|SS_WHITERECT|WS_VISIBLE|WS_BORDER,  
										0,0,0,0,
										hWndParent,hMenu,hInstance,NULL);

	//�����µ�  ģ������� ���,��ʼ�� MODULEMANAGER
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

	//��staticModuleManagerBoard �ϴ��� staticButtonBoard
	pMM.hButtonBoard = CreateWindow(TEXT("static"),NULL,WS_CHILD|SS_WHITERECT|WS_VISIBLE,   
										0,0,0,0,
										pMM.hStaticBoard,(HMENU)1,hInstance,NULL);
	pMM.OldModuleButton = (WNDPROC)SetWindowLong (pMM.hButtonBoard,GWL_WNDPROC, (LONG) moduleProc.ButtonWindowProc) ;

	//����WM_CREATE��Ϣ������һ�� �µ���Ϣ������ ������"��ť����" ��Ķ�ʱ��
	SendMessage(pMM.hButtonBoard,WM_CREATE,0,0);

	//��ʼ��ģ�������pMM�е�ģ�����س�Ա
	pMM.currentModuleCapacity = 5;
	pMM.pToolModule = (PTOOLMODULE)malloc(sizeof(TOOLMODULE) * pMM.currentModuleCapacity);
	CheckNullErrorAndQuit(pMM.pToolModule,4,TEXT("Memory for [pMM.pToolModule]  reallocated error In CreateStaticModuleManergerBoard()!"));


	//���õ��ľ������ hMM
	hMM = (HMODULEMANAGER)ManagerMapList.list[ManagerMapList.cUnit-1].handle;

	return pMM.hStaticBoard;
}

//�� hMM��Ӧ�� �������ṹ���� ����µ���ģ��
void AddNewModule(HMODULEMANAGER hMM,ModuleProc &moduleProc, TCHAR *pstrName)
{
	//���ģ������� �ṹ��ָ��
	PMODULEMANAGER pMM = GetHwndByHModuleManager(hMM);
	CheckNullErrorAndQuit(pMM,5,TEXT("Can't Get ModuleManager By HMODULEMANAGER in AddNewModule()!"));
	
	//���ģ������� �� ӳ���б� ����Ŀռ������������·�������ڴ�
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

	//����µġ���ť���� ��̬�ġ���ť���ڡ�������µ����ݴ��ڵ�
	pMM->pToolModule[pMM->countOfModule].moduleName = pstrName;
	pMM->pToolModule[pMM->countOfModule].hButton = CreateWindow(TEXT("button"),pstrName,WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE,
													5+80*pMM->countOfModule,5,80,40,pMM->hButtonBoard,(HMENU)pMM->countOfModule,
																						//ID ����Ϊģ��������pToolModule[]��λ���±�
													hInstance2,NULL);
	//��ģ�鰴ť����Ϣ������ ����Ϊ �Զ������Ϣ������
	oldModuleButtonWndProc[pMM->countOfModule] = 
		(WNDPROC) SetWindowLong(pMM->pToolModule[pMM->countOfModule].hButton,GWL_WNDPROC,(LONG)ModuleButtonProc);

	pMM->pToolModule[pMM->countOfModule].hContentBoard = CreateWindow(TEXT("static"),NULL,WS_CHILD|SS_WHITERECT|WS_VISIBLE|WS_BORDER,
										0,0,0,0,
										pMM->hStaticBoard,(HMENU)(HMENU)pMM->countOfModule,
																						//ID ����Ϊģ��������pToolModule[]��λ���±�
										hInstance2,NULL);
	pMM->pToolModule[pMM->countOfModule].OldModuleContent = (WNDPROC)SetWindowLong (pMM->pToolModule[pMM->countOfModule].hContentBoard
										,GWL_WNDPROC, (LONG) moduleProc.ContentWindowProc) ;
	pMM->countOfModule++;
	
	//����WM_CREATE��Ϣ������һ�� �µ���Ϣ������ �Գ�ʼ��"���ݴ���" �������
	SendMessage(pMM->pToolModule[pMM->countOfModule-1].hContentBoard,WM_CREATE,0,0);

}

void DeleteModuleManage(HMODULEMANAGER hmm)
{

}
void DeleteModule(HMODULE hm)
{

}



//��ÿ��ģ�����static �����ݴ��ڡ��ľ���õ� ���Ӧ�� ģ��������ﴢ��� ԭ�ȵ���Ϣ������
WNDPROC GetOldContentWndProcByContentHWND(HWND hwnd){

	unsigned int i;
	int j;
	for(i=0;i<ManagerMapList.cUnit ; i++)
	{
		//�ȴӹ����� ����� ��ַ ӳ���б����ҵ�ƥ��Ĺ�����
		if(GetParent(hwnd) == ((PMODULEMANAGER)ManagerMapList.list[i].address)->hStaticBoard)
		{
			//�ٴӹ�������� �����ݴ��ڡ�����������ҳ���Ӧ�� ��Ϣ������
			for(j=0 ; j<((PMODULEMANAGER)ManagerMapList.list[i].address)->countOfModule ; j++)
			{
				if(hwnd == ((PMODULEMANAGER)ManagerMapList.list[i].address)->pToolModule[j].hContentBoard)
					return ((PMODULEMANAGER)ManagerMapList.list[i].address)->pToolModule[j].OldModuleContent;
			}

		}
	}
	return NULL;
}


//ͨ�� ���������hMM  �õ��������ײ�static ���ڵĴ��ھ��
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


//�ɡ����ݴ��ڡ��ľ�� �õ�������ģ��� ID
int GetIdInModuleManagerByContentHWND(HWND hwnd)
{
	int j;

	//�ɡ����ݴ��ڡ��ľ��  �õ�ģ�������ָ��
	PMODULEMANAGER pMM= GetPModuleManagerByContentHWND(hwnd);
	if(pMM != NULL)
	{
		//�ӹ�������� �����ݴ��ڡ�����������ҳ���Ӧ�� λ�ü�Ϊ�� ID
			for(j=0 ; j<pMM->countOfModule ; j++)
			{
				if(hwnd ==pMM->pToolModule[j].hContentBoard)
					return j; 
			}
	}
	return -1;
}


//�ɡ����ݴ��ڡ��ľ��  �õ�ģ�������ָ��
PMODULEMANAGER GetPModuleManagerByContentHWND(HWND hwnd)
{
	unsigned int i;
	for(i=0;i<ManagerMapList.cUnit ; i++)
	{
		//�ȴӹ����� ����� ��ַ ӳ���б����ҵ�ƥ��Ĺ�����
		if(GetParent(hwnd) == ((PMODULEMANAGER)ManagerMapList.list[i].address)->hStaticBoard)
		{
			return (PMODULEMANAGER)ManagerMapList.list[i].address;
		}
	}
	return NULL;

}

//�ɡ���ť���ڡ��ľ��  �õ�ģ�������ָ��
PMODULEMANAGER GetPModuleManagerByButtonHWND(HWND hwnd)
{
	//���� �ɡ����ݴ��ڡ��ľ��  �õ�ģ�������ָ�� �� GetParent(hwnd) �ǵõ������ھ��
	// ����ť���ڡ��͡����ݴ��ڡ����ģ�������ָ����߼�����ȫ��ͬ��
	return GetPModuleManagerByContentHWND(hwnd);
}

//��static board �Ĵ��ھ����� �����Ӧ�� ģ���������ָ��
PMODULEMANAGER GetPModuleManagerByStaticHWND(HWND hwnd){

	unsigned int i;
	for(i=0;i<ManagerMapList.cUnit ; i++)
	{
		if(hwnd == ((PMODULEMANAGER)ManagerMapList.list[i].address)->hStaticBoard)
			return (PMODULEMANAGER)ManagerMapList.list[i].address;
	}
	return NULL;
}

/** ǩ��ģ��ʹ�õĺ��� ***/
bool IsLeapYear(int year)
{
	return ((year%4==0 && year%100!=0) || (year%100==0 && year%400!=0));
}
//ǰһ��ǩ�����뱣֤ʱ��ǰ�ں�һ��ǩ��
int DaysBetween(SignIn &former,SignIn &latter)
{
	int baseYear = former.year;
	int DaysFromBase1,DaysFromBase2;
	int i,j;

	//����ǰ�ߵ��������
	DaysFromBase1 =0;
	for(j=1; j<former.month ;j++) //���� j=1...(former.month-1) ������
	{
		DaysFromBase1 += DAY_IN_MONTH[IsLeapYear(baseYear)][j-1];
	}
	DaysFromBase1 +=  former.day;

	//������ߵ��������
	DaysFromBase2 =0;
	for(i = baseYear ; i<latter.year ;i++)
	{
		DaysFromBase2 += IsLeapYear(i)? 366:365;
	}
	for(j=1; j<latter.month ;j++) //���� j=1...(latter.month-1) ������
	{
		DaysFromBase2 += DAY_IN_MONTH[IsLeapYear(i)][j-1];
	}
	DaysFromBase2 +=  latter.day;

	return DaysFromBase2-DaysFromBase1;
}


