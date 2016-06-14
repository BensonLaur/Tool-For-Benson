/******************************************************************
	�ļ�����MainModuleManagerWndProc.cpp

	�ļ���������ģ���������������Ϣ������

*********************************************************************/
#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>
#include "MyDefine.h"

//ʹ��ȫ�ֱ��� ModuleManager (��ģ�����������GlobleManager.h�ж��壬��Entry.cpp�г�ʼ��)
//ʹ��ȫ�ֱ��� ModuleManager2 (�ڶ���ģ�����������GlobleManager.h�ж��壬��Entry.cpp�г�ʼ��)
extern MODULEMANAGER ModuleManager,ModuleManager2;

//ʹ��ȫ�ֱ���szSubWindowClassName[] (�Ӵ������������GlobleManager.h�ж��壬��Entry.cpp�г�ʼ��)
extern TCHAR szSubWindowClassName[];	

//ʹ��ȫ�ֱ��� quitFlag (��GlobleManager.h�ж���)
extern bool quitFlag;

//���ڱ��ǩ���������Ƿ����ı�
extern bool isSignInChange;

//������ ������ģ��������� ��ָ����ǰ ��ťѡ�����Ǹ���ģ��
static int idOfCurrentAppModule;


/*********   �ڴ˶������ģ��ġ����ݴ��ڡ��������Ӵ��ڵġ�������Ա�ʹ��  ********/

//��һ��ģ�� ��ǩ���� ����Ҫ���ھ��
HWND hSignInListBox;
static HWND hAddItemButton , hDeleteItemButton ,  
	   hStaticCurrent , hStaticCurrentItem , 
	   hSignInButton , hStatisticsGroupBox , hBaseDataButton , hHistogramButton ,hAdvanceButton;
HWND hDlgBaseData , hDlgHistogram , hDlgAdvance;

//�ڶ���ģ�� ���ղء� �ľ���ľ����SecondModuleManagerWndProc.cpp �ж���

//������ģ�� 

/*********   �ڴ˶������ģ�����õ��ġ����ݡ�  *************************************/
//��һ��ģ�� ��ǩ����������
AllSignIn allSignIn;			 //��������ǩ�����ݵĸ��ģ�������ǩ����ʱ����ǩ���д��pSignInListTemp��ǩ����֮���д��
int currentSignInSel;
static FILE *pSignInFile;
pSignIn pSignInListTemp;  //�����������ļ���ȡ��رգ�ǩ�����ݵĸ��Ļ����ڴˣ�����ر�ʱ�Ÿ���д��
int countOfSignIn_All;
static TCHAR filePath[256];

static struct tm *pTime;
static time_t secondTime;

//������ģ�� ����������


//��SignInModuleSubWindow.cpp �� ��ʼ����һ��ģ��������Ӵ���hDlgBaseData �� hDlgHistogram
void InitializeSubWindow(HWND hwnd);
//��SignInModuleSubWindow.cpp �� ��ʼ����һ��ģ��� hDlgAddItem
void InitializeAddItemWindow(HWND hwnd,POINT leftTopPoint,int cyChar);

/*****************************************************************
*	ģ������� �� �ײ㾲̬����̬���ڡ� ʹ�õ���Ϣ������ (��ģ�������)
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
			//���ϵͳĬ������ĸ߶�
			cyChar = HIWORD (GetDialogBaseUnits ()) ;
			cyChar*=2;

			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);

			//����hStaticBoard���Ӵ���hButtonBoard��hContentBoard ��λ��
			MoveWindow(ModuleManager.hButtonBoard,3,3,cxClient-2*3,cyChar+5,TRUE);

			for(i=0 ; i<ModuleManager.countOfModule ; i++)
				MoveWindow(ModuleManager.pToolModule[i].hContentBoard,3,3+cyChar+6,cxClient-2*3,cyClient-3-cyChar-6-3,TRUE);
			
			//��ʾ��ǰ��ѡ���ġ����ݴ��ڡ�
			for(i=0; i<ModuleManager.countOfModule ;i++){
				
				if(i==idOfCurrentAppModule){
					ShowWindow(ModuleManager.pToolModule[i].hContentBoard,SW_SHOW);
				}
				else 
					ShowWindow(ModuleManager.pToolModule[i].hContentBoard,SW_HIDE);
			}

		break;

	case WM_USER+1: //�����ڳ�������������ڵ���Ϣ������ �� WM_DESTROY ���� ���� �û���Ҫ��������ʱ����
		
		  //��������������д���ļ�
		  if((pSignInFile = fopen(filePath,"w+b"))==NULL)
		  {
			  wsprintf(szBuffer,TEXT("ǩ�������޷����£�\n\nԭ��\n�޷����ļ�:%s \n\n������Ŀ¼�Ƿ�ȱʧ��"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("��ʾ"),MB_ICONERROR);
			  return -1;
		  }
		  ret1=fwrite(&countOfSignIn_All,sizeof(int),1,pSignInFile);
		  ret2=fwrite(pSignInListTemp,sizeof(SignIn),countOfSignIn_All,pSignInFile);
		  if(ret1==0 || ret2==0)
		  {
			  wsprintf(szBuffer,TEXT("ǩ�������޷����£�\n\nԭ��\n�Ѿ����ļ�:%s \n\n��������δ֪ԭ��д��ʧ�ܣ�"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("��ʾ"),MB_ICONERROR);
			  return -1;
		  }
		  
		  if(fclose(pSignInFile)==EOF)
		  {
			  wsprintf(szBuffer,TEXT("ǩ�����ݸ��²��ȶ���\n\nԭ��\n�Ѿ��򿪲��޸��ļ�:%s \n\n����δ�������رգ�"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("��ʾ"),MB_ICONERROR);
			  return -1;
		  }

		  //���Ҫ�˳������˲��ͷ�pSignInListTemp
		  if(quitFlag == true)
			free(pSignInListTemp);
			
		  return 0;
		break;
	}

	return  CallWindowProc (pMM->OldModuleManager, hwnd, message, wParam,lParam) ;
}


/*****************************************************************
*	ģ������� �� ��̬����ť���ڡ�ʹ�õ���Ϣ������ (��ģ�������)
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
	static int blockLeft,blockRight,newLeft,newRight,oldLeft,oldRight; //���ڻ��Ƹ�������
	static int firstTimeFlag = 1;
	static bool toRight = true;
	static int currentId = 0;

	//"��ť����"���������ҷ���ť
	static HWND hLeftButton,hRightButton;
	//��¼����ť���ڡ��а�ť�Ŀ�ʼ�����λ��
	static int xButtonBegin,xButtonEnd;

	switch(message)
	{
	case WM_CREATE:
		//���ö�ʱ�������ڻ滭�����ƶ��ĸ�������
		SetTimer (hwnd, 1, 10, NULL) ;
		blockLeft = blockRight = 0;
		xButtonBegin = 5;

		hLeftButton = CreateWindow(TEXT("button"),TEXT("<<"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // ʹ�� MAX_COUNT_OF_MODULE+1 ��Ϊ�����ƶ���ť��ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+1), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		hRightButton = CreateWindow(TEXT("button"),TEXT(">>"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // ʹ�� MAX_COUNT_OF_MODULE+2 ��Ϊ�����ƶ���ť��ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+2), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

		break;

	case WM_SIZE:
		//��á���ť���ڡ��Ĵ�С��Ϣ�����豸������
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cxChar = LOWORD (GetDialogBaseUnits ()) ;

		//������һ��
		if(firstTimeFlag)
		{
			//��ÿ����ť����������Ϣ�������б���
			intList = (int*)malloc(sizeof(int)*ModuleManager.countOfModule);
			CheckNullErrorAndQuit(intList,13,TEXT("Memory for [intList] alloc failed in ModuleButtonWindowProc()!"));

			for(i=0 ; i< ModuleManager.countOfModule ; i++)
			{
				intList[i] = GetWindowTextLength (ModuleManager.pToolModule[i].hButton) ;
			}
	
			//��һ�ε���ʱ������������ָ��Ϊ��һ����ť
			newLeft = xButtonBegin;
			newRight = xButtonBegin + intList[0]*cxChar*2+30;
			
			firstTimeFlag = 0;
		}

		//����xButtonBegin�������ȵ����ð�ť�� λ�úʹ�С
		lengthToLeft =0;
		for(i=0 ; i< ModuleManager.countOfModule ; i++)
		{
			MoveWindow(ModuleManager.pToolModule[i].hButton,
				xButtonBegin+lengthToLeft*cxChar*2+i*30,10, intList[i]*cxChar*2+30,cyWindow-10-2,TRUE);
			lengthToLeft += intList[i];
		}

		//����xButtonBegin �� ��ť�ܿ�Ȼ�� xButtonEnd
		totalWidth = lengthToLeft*cxChar*2 + i*30;
		xButtonEnd = xButtonBegin + totalWidth;

		//�� xButtonBegin ��xButtonEnd �� windows size �Ĺ�ϵ�����������ƶ�����ť�Ƿ����
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

		//�ı��С���ʱ������ಿ�����أ��Ҳ����п��࣬��ȫ����ť�����ұ߿�����ƶ�
		if(xButtonBegin < 5 && xButtonEnd < cxWindow-5)
		{
			xButtonBegin += cxWindow- 5 - xButtonEnd;
			newLeft += cxWindow- 5 - xButtonEnd;
			newRight += cxWindow - 5-xButtonEnd;
			toRight = true;
		}


		break;

	case WM_COMMAND:
		//�ɰ�ť�ľ�������Id ���õ�����ģ��pToolModle[]���±�
		idGot = GetWindowLong ((HWND)lParam, GWL_ID) ;
		idOfCurrentAppModule = idGot < MAX_COUNT_OF_MODULE ? idGot : idOfCurrentAppModule;

		if(idGot < MAX_COUNT_OF_MODULE) //Ϊģ�鰴ť����Ϣ
		{
			for(i=0; i<pMM->countOfModule ;i++){
				if(i==idOfCurrentAppModule)
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_SHOW);
				else 
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_HIDE);
			}

			//��������ť��λ�ã�������newLeft �� newRight��
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
		
			//ȷ�������ƶ�����
			if(idOfCurrentAppModule > currentId) toRight =true;
			else 
				if(idOfCurrentAppModule < currentId) toRight = false;
			currentId = idOfCurrentAppModule;
		}
		else //Ϊ�������ƶ�����ť����Ϣ
		{
			if(idGot == MAX_COUNT_OF_MODULE + 1) //Ϊ�����󡱰�ť
			{
				if(xButtonEnd > cxWindow-5){
					buttonShift=xButtonEnd;
					//�ı�Buttonλ��
					xButtonEnd = max( cxWindow-5, xButtonEnd-30);
					xButtonBegin = xButtonEnd - totalWidth;
					
					//�ı����λ��
					buttonShift = xButtonEnd -buttonShift;
					newLeft += buttonShift;
					newRight += buttonShift;
					toRight = false;
				}
			}
			else  //Ϊ�����ҡ���ť
			{
				if(xButtonBegin < 5){
					buttonShift=xButtonBegin;
					//�ı�Buttonλ��
					xButtonBegin = min( 5, xButtonBegin + 30);
					xButtonEnd = xButtonBegin + totalWidth;

					//�ı����λ��
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

		//���»滭��������ķ�Χ
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
		//���Ƹ�������
		//�����ɵ�����
		SetRect(&rect,oldLeft,1,oldRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		//�����Ե�����
		SetRect(&rect,blockLeft,1,blockRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));

         EndPaint (hwnd, &ps) ;
		break;

	}
	return CallWindowProc (pMM->OldModuleButton, hwnd, message, wParam,lParam);
}


/*****************************************************************
*	ÿ����ģ��� ��̬�����ݴ��ڡ�ʹ�õ���Ϣ������ (��ģ�������)
******************************************************************/

LRESULT CALLBACK ModuleContentWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//�õ�ԭ������Ϣ����
	WNDPROC oldModuleContent = GetOldContentWndProcByContentHWND(hwnd);
	CheckNullErrorAndQuit(oldModuleContent,8,TEXT("Can't Get oldModuleContent By HWND in ModuleButtonWindowProc()!"));
	
	//�õ�ģ���ID
	int id = GetIdInModuleManagerByContentHWND(hwnd);
	if(id == -1){
		MessageBox(hwnd,TEXT("ErrorCode: 0X 0011\nCant't Get [idModule] in ModuleContentWindowProc()"),TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
	//�õ�ģ�������ָ��
	PMODULEMANAGER pMM = GetPModuleManagerByContentHWND(hwnd);
	CheckNullErrorAndQuit(pMM,12,TEXT("Can't Get ModuleManager By ContentHWND in ModuleContentWindowProc()!"));

	static int cxWindow,cyWindow,cxChar,cyChar,i,j,cxScreen,cyScreen;
	//��һ��ģ�顰ǩ���ı�����
	int dlgAnswer,currentIndexSel;
	static SignIn SignInTemp;
	POINT leftTopPoint;
	RECT rect;
	AllSignIn tempAllSignIn;
	bool updateTemp;


	//�����ڴ���Ϣ�������г�ʼ������ģ�������
	switch(message)
	{
	case WM_CREATE:
		/** ע�⣺���Ӵ������ʱ�����µĻص�������WM_CREATE ���ᱻ���ã�����ú����ĸ���Ϣ����
			AddNewModule()��ʹ�� SendMessage(hContentBoard,WM_CREATE,0,0); ���͵�	*/
		//��ʼ����һ��ģ������
		if( id == 0 )
		{
				// ǩ��ListBox�����ӡ�ɾ��ǩ���ť
				 hSignInListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									5,5,80,80,
									hwnd, (HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hAddItemButton = CreateWindow(TEXT("button"),TEXT("���ǩ����"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									5,90,40,20,
									hwnd, (HMENU)2, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hDeleteItemButton = CreateWindow(TEXT("button"),TEXT("ɾ��ǩ����"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									100,90,40,20,
									hwnd, (HMENU)3, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				 //������̬��ǩ����ʾ��ǰǩ����
				 hStaticCurrent = CreateWindow(TEXT("static"),TEXT("��ǰǩ���"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									200,20,100,30,
									hwnd, (HMENU)4, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hStaticCurrentItem =CreateWindow(TEXT("static"),TEXT("***��ǰǩ����***"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									200,60,200,30,
									hwnd, (HMENU)5, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
					
				 //ǩ����ť
				hSignInButton = CreateWindow(TEXT("button"),TEXT("����ǩ��"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									200,100,40,20,
									hwnd, (HMENU)6, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				 //ǩ��ͳ��ģ��
				hStatisticsGroupBox =CreateWindow(TEXT("button"),TEXT("ǩ��ͳ��"),WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
									250,100,80,100,
									hwnd, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
				hBaseDataButton =CreateWindow(TEXT("button"),TEXT("��������չʾ"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									255,130,70,30,
									hwnd, (HMENU)9, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				hHistogramButton =CreateWindow(TEXT("button"),TEXT("��״ͼ"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									255,155,70,30,
									hwnd, (HMENU)10, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
				hAdvanceButton =CreateWindow(TEXT("button"),TEXT("�߼�����"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)11, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				//��SignInModuleSubWindow.cpp �� ��ʼ����һ��ģ��������Ӵ���hDlgBaseData �� hDlgHistogram
				InitializeSubWindow(hwnd);

				/** ҵ�������߼�������  ***/

				GetCurrentDirectory(256,filePath);
				wsprintf(filePath,TEXT("%s\\Data"),filePath);
				printf(TEXT("mkdir:%d"),mkdir(filePath));
				wsprintf(filePath,TEXT("%s\\SignIn.dat"),filePath);


				//��ʼ�������ListBox��
				
				//��ǩ����¼�ļ�
				if((pSignInFile = fopen(filePath,"r+b"))==NULL)
				{
					if((pSignInFile = fopen(filePath,"w+b"))==NULL){
						wsprintf(szBuffer,TEXT("Can't Create File [%s]!"),filePath);
						CheckNullErrorAndQuit(NULL,20,szBuffer);
						exit(0);
					}
					//�½��ļ�֮�󣬳�ʼ������
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
				//��ͷ��ʼ��ȡ�ļ�����
				fseek(pSignInFile,0,SEEK_SET);
				fread(&countOfSignIn_All,sizeof(int),1,pSignInFile);

				//���ļ�������ݶ�����������ڽṹ��AllSignIn��
				pSignInListTemp = (pSignIn)malloc(sizeof(SignIn)*countOfSignIn_All);
				fread(pSignInListTemp,sizeof(SignIn),countOfSignIn_All,pSignInFile);
				fclose(pSignInFile);

				for(i=0;i<countOfSignIn_All;i++)
				{
					if(i==0){
						
						//��ʼ��һ����Ŀ���ṹ����
						allSignIn.countOfItem =1;
						allSignIn.countInList = (int*)malloc(sizeof(int)*allSignIn.countOfItem);
						allSignIn.countInList[0]=1;
						allSignIn.SignList = (pSignIn*)malloc(sizeof(pSignIn)*allSignIn.countOfItem);
						allSignIn.SignList[0] = (pSignIn)malloc(sizeof(SignIn)*allSignIn.countInList[0]);
						allSignIn.SignList[0][0]= pSignInListTemp[i];
					}
					else
					{

						//�ڶ�ά���������ǩ�����Ƿ���֣������֣�����뵽��Ӧ��ǩ��������
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
						if(j==allSignIn.countOfItem) //���޳��֣����·���һ���ڴ���������������
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


				//�������ݸ���ListBox������
				for(i=0;i<allSignIn.countOfItem;i++)
					SendMessage (hSignInListBox, LB_ADDSTRING, 0, (LPARAM) &allSignIn.SignList[i][0].name) ;
				//��ǰǩ����ľ�̬��ǩ��Ϊ��δѡ��ǩ���
				SetWindowText(hStaticCurrentItem,TEXT("����δѡ������ѡ������ ����"));

				currentSignInSel=-1;

		 } 
		 else if( id == 1 )//��ʼ���ڶ���ģ������
		{
			/**ע��:�ڶ���ģ��������Ĳ��� ���ɴˡ���ģ�����������̬ContentBoard ����Ϣ������ ������  */
			/**    ���ڶ���ģ���������ĸ���ģ��Ĳ��� ���ɡ��ڶ���ģ���������ModuleContentWindowProc2����*/
		}

		break;

	case WM_SIZE:
		
		//��á���ť���ڡ��Ĵ�С��Ϣ�����豸������
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;

		if( id == 0 )
		{
			//�����ؼ�λ��
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
			
			//����ModuleManager2�еײ�hStaticBoard(�ڶ���ģ��)��λ��
			MoveWindow(ModuleManager2.hStaticBoard,5,5,cxWindow-10,cyWindow-10,TRUE);

			//��hStaticBoard(�ڶ���ģ��)���Ӵ���hButtonBoard ��hContentBoard��λ�� ��hStaticBoard ����Ϣ������(�ڶ���ģ��)������

		}
		else{
		}

		break;
	case WM_PAINT:


		break;

	case WM_COMMAND:
		if( id == 0 )//�ǵ�һ��ģ��
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
				//�õ������ڵ����Ͻǵ�����Ļ�ϵ�λ�ã����Դ˼�����Ӵ��ڵ�λ��
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

				//��SignInModuleSubWindow.cpp �� ��ʼ����һ��ģ��� hDlgAddItem
				InitializeAddItemWindow(hwnd,leftTopPoint,cyChar);
			
					
				break;
			case 3: //hDeleteItemButton:
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("δѡ��ǩ����"),TEXT("ɾ��ǩ����ʾ"),MB_ICONINFORMATION);
				}
				else{
					//��currentSignInSel �õ���ǰ�����ǩ�������ٵõ����Ӧ���� allSignIn.SignList �ĵ�һ��ά�ȵ�λ��
					SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
					for(i=0;i<allSignIn.countOfItem;i++)
						if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

					wsprintf(szBuffer2,TEXT("�Ƿ�ȷ�ϡ�ɾ����ǩ��� %s ��\n\n��ǩ�����С�%d������¼"),
						szBuffer,allSignIn.countInList[currentIndexSel]);
					dlgAnswer = MessageBox(hwnd,szBuffer2,TEXT("ɾ��ǩ����ʾ"),MB_ICONINFORMATION|MB_YESNO);

					//ȷ��Ҫɾ���Ļ�
					if(dlgAnswer==IDYES){

						if(allSignIn.countInList[currentIndexSel]==0) updateTemp = false;
						else {
							updateTemp = true;
							//������ֻ��ȷ����ǩ����ʱ��isSignInChangeΪ�棬���޷��� ����if����� ȷ��֮ǰ��û���ڱ��޸�
							isSignInChange = true;
						}

						//����allSignIn
						tempAllSignIn = allSignIn;
						allSignIn.countOfItem -= 1;
						allSignIn.countInList = (int*)malloc(sizeof(int)*allSignIn.countOfItem);
						allSignIn.SignList = (SignIn **)malloc(sizeof(SignIn*)*allSignIn.countOfItem);
						for(i=0,j=0;i<tempAllSignIn.countOfItem;i++)
						{
							if(i==currentIndexSel){free(tempAllSignIn.SignList[i]);continue;}//�ͷ�ѡ�е��������
							allSignIn.SignList[j]=tempAllSignIn.SignList[i];//����ת�ƻ�allSignIn
						    allSignIn.countInList[j]=tempAllSignIn.countInList[i];
							j++;
						}

						if(updateTemp)//�����Ҫ����pSignInListTemp
						{
							printf("��Ҫ���·���!\n");
							//���·���pSignInListTemp
							printf("\n  countOfSignIn_All :%d ==> ",countOfSignIn_All);
							
							countOfSignIn_All=0;
							for(i=0;i<allSignIn.countOfItem;i++) countOfSignIn_All += allSignIn.countInList[i];

							printf("%d\n",countOfSignIn_All);
							pSignInListTemp = (SignIn *)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn_All);
							
							dlgAnswer = 0;	//���Ѿ�û�õ� dlgAnswer ������Ϊ������ʹ��
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
						//������ʾlistBox	
						SendMessage(hSignInListBox,LB_DELETESTRING,currentSignInSel,0);
						//SendMessage (hSignInListBox, LB_SETCURSEL, -1, 0) ;
						
						currentIndexSel = -1;
					}
				}
				break;

			case  6://hSignInButton:
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("δѡ��ǩ���\n\n����ѡ��򴴽�ǩ��� :)"),TEXT("ǩ����ʾ"),MB_ICONINFORMATION);
					break;
				}
				
				//��õ�ǰϵͳ��ʱ����Ϣ
				secondTime = time(NULL);
				pTime = localtime(&secondTime);
				//����ǩ������
				SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
				wsprintf(SignInTemp.name,"%s",szBuffer);
				SignInTemp.year = 1900 + pTime->tm_year;
				SignInTemp.month = pTime->tm_mon+1;
				SignInTemp.day = pTime->tm_mday;

				printf("------> szBuffer:SignInItemName:%s\n",szBuffer);
				
				//����ǩ������
				for(i=0;i<allSignIn.countOfItem;i++)
				{
					//ѭ���������е�ǩ������Ƿ���ֹ�
					if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)
					{
						//������ֹ�����һ��ѭ����������Ŀ�Ƿ��Ѿ�ǩ������
						for(j=0;j<allSignIn.countInList[i];j++)
						{
							//�����ǩ������ʾ
							if(allSignIn.SignList[i][j].year == SignInTemp.year &&
							   allSignIn.SignList[i][j].month == SignInTemp.month &&
							   allSignIn.SignList[i][j].day == SignInTemp.day )
							{
								wsprintf(szBuffer2,TEXT("ǩ���%s  %d-%d-%d �Ѿ�ǩ�����ˣ�"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
								MessageBox(hwnd,szBuffer2,TEXT("ǩ����ʾ"),MB_ICONINFORMATION);
								break;
							}
						}
						//���ǩ����δ���ֹ�
						if(j==allSignIn.countInList[i])
						{
							wsprintf(szBuffer2,TEXT("�Ƿ�ȷ�϶�ǩ���%s  ��%d-%d-%d ��ǩ����"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
							dlgAnswer=MessageBox(hwnd,szBuffer2,TEXT("ǩ����ʾ"),MB_ICONINFORMATION|MB_YESNO);
							
							//���û�ȷ��ʱ�������µ��㹻���ڴ棬��ǩ����Ϣд�뻺��ṹallSignIn
							if(dlgAnswer==IDYES){
								allSignIn.countInList[i] += 1;
								allSignIn.SignList[i] = (pSignIn)realloc(allSignIn.SignList[i],sizeof(SignIn)*allSignIn.countInList[i]);
								allSignIn.SignList[i][allSignIn.countInList[i]-1] = SignInTemp;

								//ͬʱ����countOfSignIn_All �� pSignInListTemp �Ա�����ʱд���ļ�
								countOfSignIn_All += 1;
								pSignInListTemp = (pSignIn)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn_All);
								pSignInListTemp[countOfSignIn_All-1] = SignInTemp;
								
								//ǩ���ɹ���ʾ
								wsprintf(szBuffer2,TEXT("ǩ���%s  %d-%d-%d  ǩ���ɹ���"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
								MessageBox(hwnd,szBuffer2,TEXT("ǩ����ʾ"),MB_ICONINFORMATION);

								//�޸�ȫ�ֵġ��޸ı�ǡ�
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
					MessageBox(hwnd,TEXT("δѡ��ǩ���\n\n����ѡ��򴴽�ǩ��� :)"),TEXT("ͳ����ʾ"),MB_ICONINFORMATION);
					break;
				}

				//��ǰ�����Զ�����Ϣ����ʼ������
				SendMessage(hDlgBaseData,WM_USER+2,0,0);

				ShowWindow(hDlgBaseData ,SW_SHOW);

				break;
			case 10: //hHistogramButton:
				
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("δѡ��ǩ���\n\n����ѡ��򴴽�ǩ��� :)"),TEXT("ͳ����ʾ"),MB_ICONINFORMATION);
					break;
				}
				
				//��ǰ�����Զ�����Ϣ����ʼ������
				SendMessage(hDlgHistogram,WM_USER+2,0,0);

				ShowWindow(hDlgHistogram,SW_SHOW);

				break;
			case 11: //hAdvanceButton:
				
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("δѡ��ǩ���\n\n����ѡ��򴴽�ǩ��� :)"),TEXT("ͳ����ʾ"),MB_ICONINFORMATION);
					break;
				}
				
				//��ǰ�����Զ�����Ϣ����ʼ������
				SendMessage(hDlgAdvance,WM_USER+2,0,0);

				ShowWindow(hDlgAdvance,SW_SHOW);

				break;
			}

		}
		else if(id == 2)//�ǵڶ���ģ��
		{

		}

		break;

	}

	return CallWindowProc (oldModuleContent, hwnd, message, wParam,lParam);
}


