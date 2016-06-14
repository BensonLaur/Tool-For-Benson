/******************************************************************
	�ļ�����MyDefine.h

	�ļ������������Զ���ĳ����ͽṹ�壬�Լ�������Ҫ����Ϣ������

*********************************************************************/
#ifndef MY_DEFINE_TOOL_FOR_BENSON
#define MY_DEFINE_TOOL_FOR_BENSON

/*********  ��������  *********/
#define COUNT_OF_MODULE_MANAGER 2
#define MAX_COUNT_OF_MODULE 20
#define MAX_LEN_OF_MODULE_NAME 64
#define MAX_COUNT_OF_MAP_HANDLE 1000 

#define LBS_STANDARD_NO_SORT          (LBS_NOTIFY | WS_VSCROLL | WS_BORDER)


/*********  ���Ͷ���  *********/

//���� ģ�� ����
typedef struct _TOOLMODULE{
TCHAR *moduleName;		//ģ����
LONG pModuldeManager;							//����ģ�������(ʹ��ʱǿ��ת��ΪPMODULEMANAGER ����)
HWND hButton;
HWND hContentBoard;								//��ʾ���ݵľ�̬����
WNDPROC OldModuleContent;						//����ContentBoardԭ�ȵ���Ϣ����
}TOOLMODULE,*PTOOLMODULE;

//����ģ����
DECLARE_HANDLE(HTOOLMODULE);

//���� ģ������� ����
typedef struct _MODULEMANAGER{
int countOfModule;								//��ǰ��ģ����
//HTOOLMODULE hToolModule[MAX_COUNT_OF_MODULE];   //ģ��ľ��
PTOOLMODULE pToolModule;						//ģ�������ָ��

int currentModuleCapacity;						//��ŵ�ǰ���õ��ѷ����ڴ��ģ����

WNDPROC OldModuleManager;						//��״���ԭ�ȵ���Ϣ������
WNDPROC OldModuleButton;						//��ť����ԭ�ȵ���Ϣ������

HWND hStaticBoard;								//��׵Ĵ��ھ��
HWND hButtonBoard;								//��ť���ھ��
}MODULEMANAGER,*PMODULEMANAGER;

//����ģ����������
DECLARE_HANDLE(HMODULEMANAGER);


//��������ӳ�䵥Ԫ
typedef struct _MapBaseUnit{
HANDLE handle;
LONG address;
}MapBaseUnit,*PMapBaseUnit;

//����ӳ����нṹ
typedef struct _MapList{
PMapBaseUnit list;
unsigned int cUnit;
unsigned int capacity;
}MapList,*PMapList;

//ÿ��ģ�������Ҫ�в�ͬ����Ϣ������
typedef struct _ModuleProc
{
WNDPROC ManagerProc;
WNDPROC ButtonWindowProc;
WNDPROC ContentWindowProc;

}ModuleProc,*PModuleProc;

//�������ģ���ڵĿؼ�����Ϣ��������


/*********   �ڴ˶������ģ��ʹ�õ��ļ����ݽṹ��  ********/
//��һ��ģ��
//�ļ���Ԫ�ṹ
typedef struct _SignIn{
TCHAR name[256];
int year;
int month;
int day;
} SignIn,*pSignIn;
//��¼����ṹ
typedef struct _AllSignIn{
int countOfItem;
SignIn ** SignList;  //��ά��ǩ����¼����һά�ĸ���ΪcountOfItem ���ڶ�ά�ĸ����ֱ�����countList
int * countInList;
} AllSignIn;


/*********  �ڶ���������  *********/
//�������ȫ�־�̬����

//���ں���AddNewModule�У�����µġ���ť���� ��̬�ġ���ť���ڡ�ʱ����������ʹ�õ� ����ʵ������
//���������û�ж����ʼ��������ť���ڻ����ܴ���
static HINSTANCE hInstance2;


//���������б����ڹ���ȫ�ֵľ��
static MapList ManagerMapList;
static MapList ModuleMapList;

//����ȫ�־������õ���wsprintf�Ⱥ�������Ҫ�����ݴ��ַ���
static TCHAR szBuffer[256];
static TCHAR szBuffer2[256];

//�������ڼ���ǩ������� ���飬���� ǩ��ģ��ʹ�õĺ��� DaysBetween 
static int DAY_IN_MONTH[2][12] = {
	{31,28,31,30,31,30,31,31,30,31,30,31},
	{31,29,31,30,31,30,31,31,30,31,30,31}
};


/*********  ��������  *********/


//��� pointer �Ƿ�Ϊ�գ����Ϊ�գ��ԶԻ�����ʾ ������ errorCode���ʹ�����ʾ��Ϣ erroInfo ��Ȼ���˳�����
void CheckNullErrorAndQuit(void* pointer,LONG errorCode,CHAR* erroInfo);

//��ʼ��ӳ�����
void InitializeMapList();

void FreeMapList();

//Ϊӳ����� �������µ�Ψһ�ľ��
HANDLE GetNewRandomHandle(MapList mapList);

//����һ��ģ������� ModuleManager �Լ����� hMM
// ModuleManager ������ ����Ӧ�� hStaticBoard; hButtonBoard; hContentBoard;
HWND CreateStaticModuleManergerBoard(HMODULEMANAGER &hMM, MODULEMANAGER &pMM,ModuleProc &moduleProc,
									 HWND hWndParent ,HMENU hMenu,HINSTANCE hInstance);

//��hMM��Ӧ�� �������ṹ���� ����µ���ģ��
void AddNewModule(HMODULEMANAGER hMM,ModuleProc &moduleProc, TCHAR *pstrName);

void DeleteModuleManage(HMODULEMANAGER hmm);
void DeleteModule(HMODULE hm);


//�� ÿ��ģ�����static �����ݴ��ڡ��ľ���õ� ���Ӧ�� ģ��������ﴢ��� ԭ�ȵ���Ϣ������
WNDPROC GetOldContentWndProcByContentHWND(HWND hwnd);


//ͨ�����������hMM  �õ��������ײ�static ���ڵĴ��ھ��
PMODULEMANAGER GetHwndByHModuleManager(HMODULEMANAGER hMM);


//�ɡ����ݴ��ڡ��ľ�� �õ�������ģ��� ID
int GetIdInModuleManagerByContentHWND(HWND hwnd);


//�ɡ����ݴ��ڡ��ľ��  �õ�ģ�������ָ��
PMODULEMANAGER GetPModuleManagerByContentHWND(HWND hwnd);
//�ɡ���ť���ڡ��ľ��  �õ�ģ�������ָ��
PMODULEMANAGER GetPModuleManagerByButtonHWND(HWND hwnd);
//��static board �ľ����� �����Ӧ�� ģ���������ָ��
PMODULEMANAGER GetPModuleManagerByStaticHWND(HWND hwnd);

/** ǩ��ģ��ʹ�õĺ��� ***/
bool IsLeapYear(int year);
int DaysBetween(SignIn &former,SignIn &latter);


/**
*  ��Ϣ������  
*/
//��ģ�����������Ϣ������

//ģ������� �� �ײ㾲̬����̬���ڡ� ʹ�õ���Ϣ������ (��ģ�������)
LRESULT CALLBACK ModuleManagerProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//ģ������� �� ��̬����ť���ڡ�ʹ�õ���Ϣ������ (��ģ�������)
LRESULT CALLBACK ModuleButtonWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//ÿ����ģ��� ��̬�����ݴ��ڡ�ʹ�õ���Ϣ������ (��ģ�������)
LRESULT CALLBACK ModuleContentWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//�ڶ���ģ���µ���ģ�����������Ϣ������

//ģ������� �� �ײ㾲̬����̬���ڡ� ʹ�õ���Ϣ������  (�ڶ���ģ�������)
LRESULT CALLBACK ModuleManagerProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//ģ������� �� ��̬����ť���ڡ�ʹ�õ���Ϣ������  (�ڶ���ģ�������)
LRESULT CALLBACK ModuleButtonWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//ÿ����ģ��� ��̬�����ݴ��ڡ�ʹ�õ���Ϣ������  (�ڶ���ģ�������)
LRESULT CALLBACK ModuleContentWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//��һ��ģ�������г��ֵ�Edit�ؼ����µ���Ϣ������
LRESULT CALLBACK newEditProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#endif
