#include <windows.h>
#include <stdio.h>
#include <direct.h>
#include <time.h>
#include "MyDefine.h"
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"") 
#define COUNT_OF_MODULE sizeof(ModuleName)/sizeof(ModuleName[0])
#define COUNT_OF_SUB_MODULE sizeof(CollectionModuleName)/sizeof(CollectionModuleName[0])

static TCHAR szAppName[] = TEXT ("Tool For Benson") ;
static TCHAR szSubWindowClassName[] = TEXT("subWindow");

/*********   ģ������ƣ���Ҫʱ����ֱ��������������ƴӶ� ֱ�����ģ��  ********/
static TCHAR *ModuleName[] = {
TEXT("ǩ��"),
TEXT("�ղ�"),
TEXT("�ղ�"),
TEXT("�ղ��ղ��ղ��ղ��ղ��ղ�"),
TEXT("�ղ�"),
TEXT("�ղ�")
};
static TCHAR *CollectionModuleName[] = {
TEXT("����"),
TEXT("����"),
TEXT("����")
};

//��ģ�鼰���������� �� �ڶ���ģ�顰�ղء������������ 
static MODULEMANAGER ModuleManeger,ModuleManeger2;	 
static HMODULEMANAGER hMM,hMM2;
//��¼��ǰģ���ID
static int idOfCurrentAppModule;
static int idOfCurrentSubModule;


static ModuleProc moduleProc[COUNT_OF_MODULE_MANAGER];

/*********   �ڴ˶������ģ��ġ����ݴ��ڡ��������Ӵ��ڵľ���Ա�ʹ��  ********/

//��һ��ģ�� ��ǩ���� �����о��
 
static HWND hSignInListBox , hAddItemButton , hDeleteItemButton ,  
	   hStaticCurrent , hStaticCurrentItem ,
	   hSignInButton , hStatisticsGroupBox , hBaseDataButton , hHistogramButton ;
static HWND hDlgBaseData , hDlgHistogram;
static HWND hStaticSignRange1,hRangeListBox1 ,hStaticWord1, hStaticTimes , hStaticDaysOnce ,
       hRangeListBox2 , hLongOrShotListBox ,hTimesEdit, hStaticWord2,hStaticIntervalDays;
static HWND hStaticSignRange2,hStaticTime, hStaticMonth , *hStaticTimesNumber, *hStaticMonthNumber,
	   hDefaultYearButton,hStaticYearLabel, hNextYearButton, hLastYearButton;

//�ڶ���ģ�� ���ղء� �����о��  

static HWND hStaticCurrentFloder,hFloderListBox, hLinkListBox,
	   hAddFloderButton, hDeleteFloderButton,
	   hAddLinkButton,  hDeleteLinkButton;
static HWND hStaticCurrentFloder2,hFloderListBox2, hLinkListBox2,
	   hAddFloderButton2, hDeleteFloderButton2,
	   hAddLinkButton2,  hDeleteLinkButton2;
static HWND hDlgAddNetLink, hDlgAddLocalLink,
       hStaticName, hStaticLink, hNameEdit1, hLinkEdit1, hConformButton1 , hCancelbutton1,
       hStaticName2, hStaticLink2, hNameEdit2, hLinkEdit2, hConformButton2 , hCancelbutton2;

//������ģ�� 



//ǩ��ģ������
static AllSignIn allSignIn;
static FILE *pSignInFile;
static pSignIn pSignInListTemp;  //�����������ļ���ȡ��رգ�ǩ�����ݵĸ��Ļ����ڴˣ�����ر�ʱ�Ÿ���д��
static int countOfSignIn;
static SignIn SignInTemp;
static TCHAR filePath[256];
static int dlgAnswer;

static int currentSignInSel;
static struct tm *pTime;
static time_t secondTime;

/*********************************************************************
*		����Ϊ���������������ڵ���Ϣ���������͸�������
*********************************************************************/

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK SubWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
     HWND         hwnd ;
     MSG          msg ;
     WNDCLASS     wndclass ;
	// hInstance2 =hInstance;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }

     wndclass.lpfnWndProc   = SubWndProc ;
	 wndclass.lpszClassName = szSubWindowClassName;
	      if (!RegisterClass (&wndclass))
     {
          MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
                      szAppName, MB_ICONERROR) ;
          return 0 ;
     }

     hwnd = CreateWindow (szAppName,                  // window class name
                          TEXT ("Tool For Benson"), // window caption
                          WS_OVERLAPPEDWINDOW,        // window style
                          CW_USEDEFAULT,              // initial x position
                          CW_USEDEFAULT,              // initial y position
                          CW_USEDEFAULT,              // initial x size
                          CW_USEDEFAULT,              // initial y size
                          NULL,                       // parent window handle
                          NULL,                       // window menu handle
                          hInstance,                  // program instance handle
                          NULL) ;                     // creation parameters
     
     ShowWindow (hwnd, iCmdShow) ;
     UpdateWindow (hwnd) ;
     
     while (GetMessage (&msg, NULL, 0, 0))
     {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
     }
     return msg.wParam ;
}

/*****************************************************************
*	              �������õ���Ϣ������
******************************************************************/

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     HDC         hdc ;
     PAINTSTRUCT ps ;

	 static int cxClient,cyClient,cyChar;
	 static int i;
	 static PMODULEMANAGER pMM;
	 
     switch (message)
     {
     case WM_CREATE:
			//��ʼ���Զ���ľ��ӳ�����
			InitializeMapList();

			//����һ��ģ������� ModuleManager �Լ����� hMM
			
			moduleProc[0].ManagerProc = ModuleManagerProc;
			moduleProc[0].ButtonWindowProc = ModuleButtonWindowProc;
			moduleProc[0].ContentWindowProc = ModuleContentWindowProc;

			// ModuleManager ������ ����Ӧ�� hStaticBoard; hButtonBoard; hContentBoard;
			CreateStaticModuleManergerBoard(hMM,ModuleManeger,moduleProc[0],
				         hwnd,(HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));

			for(i=0; i <COUNT_OF_MODULE ; i++ )
			   AddNewModule(hMM,moduleProc[0],ModuleName[i]);

			//Ϊ�ڶ���ģ�鴴��һ��ģ������� ModuleManager2 �Լ����� hMM2
			
			moduleProc[1].ManagerProc = ModuleManagerProc2;
			moduleProc[1].ButtonWindowProc = ModuleButtonWindowProc2;
			moduleProc[1].ContentWindowProc = ModuleContentWindowProc2;
			
			CreateStaticModuleManergerBoard(hMM2,ModuleManeger2,moduleProc[1],
				         ModuleManeger.pToolModule[1].hContentBoard,(HMENU)1, //�µ�ģ��������ĸ�����Ϊ�ڶ���ģ����Ӵ���
						 (HINSTANCE)GetWindowLong(ModuleManeger.pToolModule[1].hContentBoard,GWL_HINSTANCE));
			
			for(i=0; i <COUNT_OF_SUB_MODULE ; i++ )
			   AddNewModule(hMM2,moduleProc[1],CollectionModuleName[i]);

			/**ע��:�ڶ���ģ��������Ĳ��� ���ɡ���ģ�����������̬ContentBoard ����Ϣ������ ������  */
			/**    ���ڶ���ģ���������ĸ���ģ��Ĳ��� ���ɡ��ڶ���ģ���������ModuleContentWindowProc2����*/
			
			//���ϵͳĬ������ĸ߶�
			cyChar = HIWORD (GetDialogBaseUnits ()) ;
			cyChar*=2;

			idOfCurrentAppModule=0;
			idOfCurrentSubModule=0;

          return 0 ;

	 case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			
			//����ModuleManeger�����Ӵ��ڵ�λ�ã�hButtonBoard ��hContentBoard ��hStaticBoard���Ӵ���
			MoveWindow(ModuleManeger.hStaticBoard,5,5,cxClient-10,cyClient-10,TRUE);
			MoveWindow(ModuleManeger.hButtonBoard,3,3,cxClient-16,cyChar+5,TRUE);

			for(i=0 ; i<ModuleManeger.countOfModule ; i++)
				MoveWindow(ModuleManeger.pToolModule[i].hContentBoard,3,3+cyChar+6,cxClient-16,cyClient-10-3-cyChar-6-3,TRUE);
			
			//��ʾ��ǰ��ѡ���ġ����ݴ��ڡ�
			for(i=0; i<ModuleManeger.countOfModule ;i++){
				
				if(i==idOfCurrentAppModule){
					ShowWindow(ModuleManeger.pToolModule[i].hContentBoard,SW_SHOW);
				}
				else 
					ShowWindow(ModuleManeger.pToolModule[i].hContentBoard,SW_HIDE);
			}


		 return 0;
     case WM_PAINT:
          hdc = BeginPaint (hwnd, &ps) ;
          
          EndPaint (hwnd, &ps) ;
          return 0 ;
          

     case WM_DESTROY:
		  //�ͷž�����ڴ�
		  FreeMapList();

		  //�ڹر��ļ�֮ǰ���Ƚ�������������д���ļ�
		  if((pSignInFile = fopen(filePath,"w+b"))==NULL)
		  {
			  wsprintf(szBuffer,TEXT("ǩ�������޷����£�\n\nԭ��\n�޷����ļ�:%s \n\n������Ŀ¼�Ƿ�ȱʧ��"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("��ʾ"),MB_ICONERROR);
			  PostQuitMessage (0) ;
			  return 0;
		  }
		  fwrite(&countOfSignIn,sizeof(int),1,pSignInFile);
		  fwrite(pSignInListTemp,sizeof(SignIn),countOfSignIn,pSignInFile);
		  fclose(pSignInFile);
		  free(pSignInListTemp);
          PostQuitMessage (0) ;
          return 0 ;
     }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

/*****************************************************************
*	�����Ӵ��� hDlgBaseData �� hDlgHistogram ����Ϣ������
******************************************************************/

LRESULT CALLBACK SubWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
    PAINTSTRUCT ps ;
	RECT rect;
	static int xOrigin=50,yOrigin=180,widthHistogram=280,heightHistogram=160,cxBlock,cyBlock,maxCount,
				monthCountArray[12],yearToShow,defaultYearToShow,cYearInRecord;
	static int i,timeInputted=2,countOfSignIn,range,minIntervalDay=-1,maxIntervalDay=-1,flagForEdit;
    static int sel_RangeListBox1=-1, sel_RangeListBox2=-1, sel_LongOrShotListBox=-1;

	SignIn signInTemp1,signInTemp2;
	
	struct tm *pTime;
	time_t secondTime;

	switch(message)
	{
	case WM_USER+1://�Զ�����Ϣ�������ڴ���ʱ����ʱ����
		 if(hwnd==hDlgBaseData)  
		 {
			//��ʼ��������List
			wsprintf(szBuffer,"%s",TEXT("һ����"));
			SendMessage (hRangeListBox1, LB_ADDSTRING, 0, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("һ����"));
			SendMessage (hRangeListBox1, LB_ADDSTRING, 1, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("һ����"));
			SendMessage (hRangeListBox1, LB_ADDSTRING, 2, (LPARAM) szBuffer) ;
			
			wsprintf(szBuffer,"%s",TEXT("һ����"));
			SendMessage (hRangeListBox2, LB_ADDSTRING, 0, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("һ����"));
			SendMessage (hRangeListBox2, LB_ADDSTRING, 1, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("һ����"));
			SendMessage (hRangeListBox2, LB_ADDSTRING, 2, (LPARAM) szBuffer) ;
			
			wsprintf(szBuffer,"%s",TEXT("�"));
			SendMessage (hLongOrShotListBox, LB_ADDSTRING, 0, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("���"));
			SendMessage (hLongOrShotListBox, LB_ADDSTRING, 1, (LPARAM) szBuffer) ;

			//hTimesEdit ������Ĭ��Ϊ��2��
			wsprintf(szBuffer,"%s",TEXT("2"));
			SetWindowText(hTimesEdit,szBuffer);

			//���õ�ǰ����ListBox��ѡΪ LB_ERR����ûѡ��״̬
			sel_RangeListBox1 = sel_RangeListBox2 = sel_LongOrShotListBox = LB_ERR;
		 }
		 break;
		 
	case WM_USER+2://��ÿ�ν����ʱ���ʼ����������
		
		if(hwnd==hDlgBaseData)
		{
			//���õ�ǰ����ListBox��ѡΪ LB_ERR����ûѡ��״̬
			sel_RangeListBox1 = sel_RangeListBox2 = sel_LongOrShotListBox = LB_ERR;

			//��ʼ��ѡ��
			SendMessage (hRangeListBox1, LB_SETCURSEL, -1, 0) ;
			SendMessage (hRangeListBox2, LB_SETCURSEL, -1, 0) ;
			SendMessage (hLongOrShotListBox, LB_SETCURSEL, -1, 0) ;

			//��ʼ��������ǩ
			wsprintf(szBuffer,TEXT("��ʾָ����Χ�ڵ�ǩ�����ݷ�Χ1"));
			SetWindowText(hStaticSignRange1,szBuffer);
			wsprintf(szBuffer,TEXT("��ʾָ����Χ�ڵ�ǩ�����ݷ�Χ2"));
			SetWindowText(hStaticSignRange2,szBuffer);

			wsprintf(szBuffer,TEXT(""));
			SetWindowText(hStaticTimes,szBuffer);
			SetWindowText(hStaticDaysOnce,szBuffer);
			SetWindowText(hStaticIntervalDays,szBuffer);
			wsprintf(szBuffer,TEXT("2"));
			SetWindowText(hTimesEdit,szBuffer);//hTimesEdit�ںܶ�ʱ��ᷢ����Ϣ��WM_COMMAND����������
		}
		else if(hwnd==hDlgHistogram) 
		 {
			//��õ�ǰϵͳ��ʱ����Ϣ
			secondTime = time(NULL);
			pTime = localtime(&secondTime);
			//��ǩ�����ݽṹ���ݴ浱ǰʱ����Ϣ
			SignInTemp.year = 1900 + pTime->tm_year;
			SignInTemp.month = pTime->tm_mon+1;
			SignInTemp.day = pTime->tm_mday;
			
			//�趨Ĭ����ʾ��ݣ�ָ����ǰҪ��ʾ�����ΪĬ��
			defaultYearToShow = SignInTemp.year;
			yearToShow =defaultYearToShow;

			//��¼��ǰ��ǩ����¼������
			cYearInRecord= defaultYearToShow - allSignIn.SignList[currentSignInSel][0].year+1;

			//������ʾ��ݵ�static��ǩ
			wsprintf(szBuffer,TEXT("%d"),yearToShow);
			SetWindowText(hStaticYearLabel,szBuffer);

			//�������������ť������״̬
			EnableWindow(hNextYearButton,FALSE);
			if(cYearInRecord==1)
				EnableWindow(hLastYearButton,FALSE);
			
		 }
		break;
	case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps) ;
		if(hwnd==hDlgHistogram)
		{
			//��������
			MoveToEx (hdc, xOrigin, yOrigin, NULL) ;
			LineTo (hdc, xOrigin, yOrigin-heightHistogram-10) ;
			MoveToEx (hdc, xOrigin, yOrigin, NULL) ;
			LineTo (hdc, xOrigin+widthHistogram+10, yOrigin);

			//���ָ����ݵ�ǩ����¼�ó�������״ͼ����Ϣ
			cxBlock = widthHistogram/(12+1);
			maxCount = 0;
			memset(monthCountArray,0,12*sizeof(int));
			for(i=0;i<allSignIn.countInList[currentSignInSel];i++)
			{
				if(allSignIn.SignList[currentSignInSel][i].year==yearToShow)
				{
					monthCountArray[allSignIn.SignList[currentSignInSel][i].month-1]++;
				}
			}
			for(i=0;i<12;i++)
			{
				maxCount = max(maxCount,monthCountArray[i]);
			}
			if(maxCount!=0){

				cyBlock = heightHistogram/maxCount;

				//������״ͼ
				for(i=0;i<12;i++)
				{
					SetRect(&rect,xOrigin+(i+1)*cxBlock-cxBlock/3,yOrigin-cyBlock*monthCountArray[i],
								xOrigin+(i+1)*cxBlock+cxBlock/3,yOrigin);
					//��������ݾͻ�
					if(monthCountArray[i]!=0){
						FillRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
						TextOut(hdc,rect.left+2,rect.top-20,szBuffer,wsprintf(szBuffer,TEXT("%d"),monthCountArray[i]));
					}
					
					TextOut(hdc,rect.left+2,rect.bottom+5,szBuffer,wsprintf(szBuffer,TEXT("%d"),i+1));
				}
			}
			else
			{
				TextOut(hdc,xOrigin+50,yOrigin-50,szBuffer,wsprintf(szBuffer,TEXT("����û��ǩ�����ݣ�")));
			}
/*
static HWND hRangeListBox1 ,hStaticWord1, hStaticTimes , hStaticDaysOnce ,
       hRangeListBox2 , hLongOrShotListBox ,hTimesEdit, hStaticWord2,hStaticIntervalDays;
  */
		}
        
         EndPaint (hwnd, &ps) ;

		break;

	case WM_COMMAND:
				//��õ�ǰϵͳ��ʱ����Ϣ
				secondTime = time(NULL);
				pTime = localtime(&secondTime);
				//��ǩ�����ݽṹ���ݴ浱ǰʱ����Ϣ
				SignInTemp.year = 1900 + pTime->tm_year;
				SignInTemp.month = pTime->tm_mon+1;
				SignInTemp.day = pTime->tm_mday;
		if(hwnd==hDlgBaseData)
		{
			switch(LOWORD (wParam))
			{
			case 1://hRangeListBox1
				switch(HIWORD(wParam))
				{
				case LBN_SELCHANGE:
					sel_RangeListBox1 = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);

					//ֱ�Ӽ������Ӧʱ���� �ж��ٴ�ǩ������ƽ��������һ�Σ���ֱ�Ӹ�����ʾ����
					countOfSignIn=0;
					range = -1;
					if(sel_RangeListBox1==0) //һ��
					{
						range=7;
					}
					else if(sel_RangeListBox1==1)//һ��
					{
						range=30;
					}
					else if(sel_RangeListBox1==2)//һ��
					{
						range=365;
					}
					//������ǰ������ѡ��ĵ�ǩ����
						for(i=0;i<allSignIn.countInList[currentSignInSel] ;i++)
						{
							//���ǩ������ʱ�䷶Χ�ڣ����м�¼
							if(DaysBetween(allSignIn.SignList[currentSignInSel][i],SignInTemp)<=range)
							{
								countOfSignIn+=1;
								if(countOfSignIn==1)//��¼�÷�Χ�ڵ�һ��ǩ����
									signInTemp1=allSignIn.SignList[currentSignInSel][i];
							}
						}
						if(countOfSignIn!=0)//����м�¼ǩ�����¼���һ��
							signInTemp2 = allSignIn.SignList[currentSignInSel][allSignIn.countInList[currentSignInSel]-1];

					//ֱ�Ӹ�����ʾ����
					if(countOfSignIn==0){

						SetWindowText(hStaticSignRange1,TEXT("��ʱ�䷶Χ��û��¼���ǩ����"));
						wsprintf(szBuffer,TEXT("%d"),0);
						SetWindowText(hStaticTimes,szBuffer);
						wsprintf(szBuffer,TEXT("%d"),-1);
						SetWindowText(hStaticDaysOnce,szBuffer);
					}
					else{
							wsprintf(szBuffer,TEXT("�����м�¼��ǩ���� %d-%d-%d  ~ %d-%d-%d "),
								signInTemp1.year , signInTemp1.month , signInTemp1.day,
								signInTemp2.year , signInTemp2.month , signInTemp2.day);
							SetWindowText(hStaticSignRange1,szBuffer);

							wsprintf(szBuffer,TEXT("%d"),countOfSignIn);
							SetWindowText(hStaticTimes,szBuffer);
							wsprintf(szBuffer,TEXT("%d"),range/countOfSignIn);
							SetWindowText(hStaticDaysOnce,szBuffer);
					}
					break;
				}
				return 0;
			
			case 5://hRangeListBox2
				switch(HIWORD(wParam))
				{
				case LBN_SELCHANGE:
					sel_RangeListBox2 = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);

					//��case 7�У�����hLongOrShotListBox����hTimeEdit �������Ӧʱ���� ��̻�� N�μ����������ֱ�Ӹ�����ʾ����
					break;
				}
				
				//����ִ��

			case 6://hLongOrShotListBox
				if(LOWORD (wParam)==6)
				switch(HIWORD(wParam))
				{
				case LBN_SELCHANGE:
					sel_LongOrShotListBox = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);

					//��case 7�У�����hRangeListBox2����hTimeEdit �������Ӧʱ���� ��̻�� N�μ����������ֱ�Ӹ�����ʾ����
					break;
				}

				//����ִ��
			case 7://hTimesEdit
					if(LOWORD (wParam)==7)
					if (HIWORD (wParam) == EN_KILLFOCUS ){
						GetWindowText(hTimesEdit,szBuffer,256);
						timeInputted= atoi(szBuffer);
						if(timeInputted<2 || timeInputted>100)
						{
							wsprintf(szBuffer,"%s",TEXT("2"));
							SetWindowText(hTimesEdit,szBuffer);
							MessageBox(hwnd,TEXT("������Ч��\n\n����Ϊ2-100������"),TEXT("������ʾ"),MB_ICONINFORMATION);
							return 0;
						}
					}
					//case 5,6,7:
					//����hRangeListBox2 ��hLongOrShotListBox �������Ӧʱ���� ��̻�� N�μ����������ֱ�Ӹ�����ʾ����
					countOfSignIn=0;
					range = -1;
					flagForEdit = 0;//��ʼΪ0,���û�����������δѡ��RangeListBox2���򲻸���hStaticSignRange2
					if(sel_RangeListBox2!=-1)//���sel_RangeListBox2�Ѿ�ѡ��
					{
						flagForEdit=1;
						//���� hStaticSignRange2 ��Ӧ������
						if(sel_RangeListBox2==0) //һ��
						{
							range=365;
						}
						else if(sel_RangeListBox2==1)//һ��
						{
							range=30;
						}
						else if(sel_RangeListBox2==2)//һ��
						{
							range=7;
						}
						//������ǰ������ѡ��ĵ�ǩ����
						for(i=0;i<allSignIn.countInList[currentSignInSel] ;i++)
						{
							//���ǩ������ʱ�䷶Χ�ڣ����м�¼
							if(DaysBetween(allSignIn.SignList[currentSignInSel][i],SignInTemp)<=range)
							{
								countOfSignIn+=1;
								if(countOfSignIn==1)//��¼�÷�Χ�ڵ�һ��ǩ����
									signInTemp1=allSignIn.SignList[currentSignInSel][i];
							}
						}
						if(countOfSignIn!=0)//����м�¼ǩ�����¼���һ��
							signInTemp2 = allSignIn.SignList[currentSignInSel][allSignIn.countInList[currentSignInSel]-1];

						//��sel_RangeListBox2�Ѿ�ѡ��Ļ�����
						if(sel_LongOrShotListBox==-1)//���sel_LongOrShotListBox��δѡ��
						{
							wsprintf(szBuffer,TEXT(""));
							SetWindowText(hStaticIntervalDays,szBuffer);
						}
						else //���sel_LongOrShotListBox�Ѿ�ѡ��
						{
							minIntervalDay = 2147483647;
							maxIntervalDay = -1;
							//������ǰ������ѡ��ĵ�ǩ����
							for(i=0;i<allSignIn.countInList[currentSignInSel] ;i++)
							{
								//���ǩ������ʱ�䷶Χ��
								if(DaysBetween(allSignIn.SignList[currentSignInSel][i],SignInTemp)<=range)
								{
									//�����ǰ��i ��i+timeInputted-1 ����ڵĻ����������Ƚ�
									if(i+timeInputted-1<allSignIn.countInList[currentSignInSel])
									{
										//��sel_LongOrShotListBox =0 ���N�μ��
										if(sel_LongOrShotListBox==0)
										{
											maxIntervalDay =max(maxIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentSignInSel][i],
												allSignIn.SignList[currentSignInSel][i+timeInputted-1]));
										}
										else 	//��sel_LongOrShotListBox =1 �����N�μ��
										{
											minIntervalDay =min(minIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentSignInSel][i],
												allSignIn.SignList[currentSignInSel][i+timeInputted-1]));
										}
									}
								}//end of <if //���ǩ������ʱ�䷶Χ��> 

							}//end of <for //������ǰ������ѡ��ĵ�ǩ����>

							if(sel_LongOrShotListBox==0)//��sel_LongOrShotListBox =0 �����N�μ����hStaticSignRange2
							{
								wsprintf(szBuffer,TEXT("%d"),maxIntervalDay);
							}
							else//��sel_LongOrShotListBox =1 �������N�μ����hStaticSignRange2
							{
								if(minIntervalDay == 2147483647)
									wsprintf(szBuffer,TEXT("-1"));
								else 
									wsprintf(szBuffer,TEXT("%d"),minIntervalDay);
							}
							SetWindowText(hStaticIntervalDays,szBuffer);

						}//end of <else //���sel_LongOrShotListBox�Ѿ�ѡ��>

					}//end of <if//���sel_RangeListBox2�Ѿ�ѡ��>

					if(flagForEdit==1)//�������count�Ǿ���if(sel_RangeListBox2!=-1)
					if(countOfSignIn==0)//sel_RangeListBox2 ��δѡ��
					{
						//���� hStaticSignRange2 ��Ӧ������
						SetWindowText(hStaticSignRange2,TEXT("��ʱ�䷶Χ��û��¼���ǩ����"));
					}
					else//sel_RangeListBox2 �Ѿ�ѡ��
					{
						//���� hStaticSignRange2 ��Ӧ������
						wsprintf(szBuffer,TEXT("�����м�¼��ǩ���� %d-%d-%d  ~ %d-%d-%d "),
								signInTemp1.year , signInTemp1.month , signInTemp1.day,
								signInTemp2.year , signInTemp2.month , signInTemp2.day);
						SetWindowText(hStaticSignRange2,szBuffer);
					}
					break;
			}
		}
		/*������Ϊ����״ͼ������ʱ*/
		else if(hwnd==hDlgHistogram) 
		{
			switch(LOWORD (wParam))
			{
				case 4://	hDefaultYearButton 

					if(HIWORD(wParam)==BN_CLICKED){
						//���������ΪĬ�ϲ��ػ�

						yearToShow = defaultYearToShow;
						//���ð�ť������״̬
						EnableWindow(hNextYearButton,FALSE);
						if(cYearInRecord==1)
							EnableWindow(hLastYearButton,FALSE);

					}
					//�������� case 7 ˢ��

				case 6://	 hLastYearButton
					
					if(LOWORD (wParam)==6)
						if(HIWORD(wParam)==BN_CLICKED){
							//�������һ�겢�ػ�
							yearToShow = yearToShow-1;

							//���ð�ť������״̬
							EnableWindow(hNextYearButton,TRUE);
							if(yearToShow == defaultYearToShow-cYearInRecord+1)
								EnableWindow(hLastYearButton,FALSE);
						}

					//�������� case 7 ˢ��

				case 7://  hNextYearButton

					if(LOWORD (wParam)==7)
						if(HIWORD(wParam)==BN_CLICKED){
							//�������һ�겢�ػ�
							yearToShow = yearToShow+1;

							//���ð�ť������״̬
							EnableWindow(hLastYearButton,TRUE);
							if(yearToShow == defaultYearToShow)
								EnableWindow(hNextYearButton,FALSE);
						}

						
					//������ʾ��ݵ�static��ǩ
					wsprintf(szBuffer,TEXT("%d"),yearToShow);
					SetWindowText(hStaticYearLabel,szBuffer);
					InvalidateRect(hwnd,NULL,TRUE);
					break;

			}

		}
		return 0;

	case WM_CLOSE:
		ShowWindow(hwnd,SW_HIDE);	

		return 0;

	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
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
			intList = (int*)malloc(sizeof(int)*ModuleManeger.countOfModule);
			CheckNullErrorAndQuit(intList,13,TEXT("Memory for [intList] alloc failed in ModuleButtonWindowProc()!"));

			for(i=0 ; i< ModuleManeger.countOfModule ; i++)
			{
				intList[i] = GetWindowTextLength (ModuleManeger.pToolModule[i].hButton) ;
			}
	
			//��һ�ε���ʱ������������ָ��Ϊ��һ����ť
			newLeft = xButtonBegin;
			newRight = xButtonBegin + intList[0]*cxChar*2+30;
			
			firstTimeFlag = 0;
		}

		//����xButtonBegin�������ȵ����ð�ť�� λ�úʹ�С
		lengthToLeft =0;
		for(i=0 ; i< ModuleManeger.countOfModule ; i++)
		{
			MoveWindow(ModuleManeger.pToolModule[i].hButton,
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
			for(i=0 ; i< ModuleManeger.countOfModule ; i++)
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

	static int cxWindow,cyWindow,cxChar,cyChar,i,j;


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

				//ǩ��ͳ�ƴ��ڵĴ���
				hDlgBaseData = CreateWindow(szSubWindowClassName,TEXT("��������չʾ"),WS_POPUP|WS_OVERLAPPEDWINDOW,
									200,100,400,300,
									hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				hDlgHistogram = CreateWindow(szSubWindowClassName,TEXT("��״ͼ"),WS_POPUP|WS_OVERLAPPEDWINDOW,
									230,130,400,300,
									hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				//ͳ�ƴ���"�������ݴ���"�Ļ���
				hStaticSignRange1 = CreateWindow(TEXT("static"),TEXT("��ʾָ����Χ�ڵ�ǩ�����ݷ�Χ"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									10,2,400,30,
									hDlgBaseData, (HMENU)10, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);
				hStaticSignRange2 = CreateWindow(TEXT("static"),TEXT("��ʾָ����Χ�ڵ�ǩ�����ݷ�Χ"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									10,120,400,30,
									hDlgBaseData, (HMENU)11, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

				hRangeListBox1 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD_NO_SORT,
									20,35,60,70,
									hDlgBaseData, (HMENU)1, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
				hStaticWord1 = CreateWindow(TEXT("static"),TEXT(" �� _____ ��, ƽ�� _____ ��1��"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									80,35,250,30,
									hDlgBaseData, (HMENU)2, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

				hStaticTimes = CreateWindow(TEXT("static"),TEXT(""),WS_CHILD|WS_VISIBLE|SS_CENTER,
									105,35,40,30,
									hDlgBaseData, (HMENU)3, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

				hStaticDaysOnce = CreateWindow(TEXT("static"),TEXT(""),WS_CHILD|WS_VISIBLE|SS_CENTER,
									205,35,40,30,
									hDlgBaseData, (HMENU)4, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

				hRangeListBox2 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD_NO_SORT,
									20,150+20,60,78,
									hDlgBaseData, (HMENU)5, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				hLongOrShotListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD_NO_SORT,
									90,150+26+10,60,52,
									hDlgBaseData, (HMENU)6, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
				hStaticWord2 = CreateWindow(TEXT("static"),TEXT("  _____ �μ��������"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									155,150+52,200,26,
									hDlgBaseData, (HMENU)8, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

				hTimesEdit = CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
									160,150+52,40,26,
									hDlgBaseData, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				oldEditProc = (WNDPROC)SetWindowLong(hTimesEdit,GWL_WNDPROC,(LONG)newEditProc);

				hStaticIntervalDays = CreateWindow(TEXT("static"),TEXT(""),WS_CHILD|WS_VISIBLE|SS_CENTER,
									300,150+52,40,26,
									hDlgBaseData, (HMENU)9, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);
				
				//��hDlgBaseData����һ���Զ������Ϣ������ɴ��ڵĽ�һ���ĳ�ʼ��
				SendMessage(hDlgBaseData,WM_USER+1,0,0);

				//ͳ�ƴ���"��״ͼ����"�Ļ���
				hStaticTime = CreateWindow(TEXT("static"),TEXT("����"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									5,20,40,30,
									hDlgHistogram, (HMENU)1, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

				hStaticMonth = CreateWindow(TEXT("static"),TEXT("�·�"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									345,170,40,30,
									hDlgHistogram, (HMENU)2, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);
				//	 *hStaticTimesNumber, *hStaticMonthNumber

				hDefaultYearButton = CreateWindow(TEXT("button"),TEXT("Ĭ�����"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									20,220,80,30,
									hDlgHistogram, (HMENU)4, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL);

				hStaticYearLabel = CreateWindow(TEXT("static"),TEXT(""),WS_CHILD|WS_VISIBLE|SS_CENTER,
									110,220,60,30,
									hDlgHistogram, (HMENU)5, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL);

				hLastYearButton = CreateWindow(TEXT("button"),TEXT("��һ��"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									200,220,60,30,
									hDlgHistogram, (HMENU)6, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL); 

				hNextYearButton = CreateWindow(TEXT("button"),TEXT("��һ��"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									260,220,60,30,
									hDlgHistogram, (HMENU)7, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL);
	
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
					countOfSignIn=6;
					fwrite(&countOfSignIn,sizeof(int),1,pSignInFile);
					pSignInListTemp = (pSignIn)malloc(sizeof(SignIn)*countOfSignIn);
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
					
					fwrite(pSignInListTemp,sizeof(SignIn),countOfSignIn,pSignInFile);
					
					free(pSignInListTemp);
				}
				//��ͷ��ʼ��ȡ�ļ�����
				fseek(pSignInFile,0,SEEK_SET);
				fread(&countOfSignIn,sizeof(int),1,pSignInFile);

				//���ļ�������ݶ�����������ڽṹ��AllSignIn��
				pSignInListTemp = (pSignIn)malloc(sizeof(SignIn)*countOfSignIn);
				fread(pSignInListTemp,sizeof(SignIn),countOfSignIn,pSignInFile);
				fclose(pSignInFile);

				for(i=0;i<countOfSignIn;i++)
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

				printf("countOfSignIn:%d\n",countOfSignIn);
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

		//printf("%s",filePath);
/*
typedef struct _AllSignIn{
int countOfItem;
SignIn ** SignInList;  //��ά��ǩ����¼����һά�ĸ���ΪcountOfItem ���ڶ�ά�ĸ����ֱ�����countList
int * countList;
} AllSignIn;

typedef struct _SignIn{
TCHAR name[256];
int year;
int month;
int day;
} SignIn,*PSignIn;
  */
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
			/*

HWND hDlgBaseData , hDlgHistogram;
HWND hRangeListBox1 ,hStaticWord1, hStaticTimes , hStaticDaysOnce ,
     hRangeListBox2 , hLongOrShotListBox ,hTimesEdit, hStaticWord2,hStaticIntervalDays;
HWND hStaticTime, hStaticMonth , *hStaticTimesNumber, *hStaticMonthNumber,
	 hDefaultYearButton,hStaticYearLabel, hNextYearButton, hLastYearButton;
  */
		}
		else if(id == 1)
		{
			//����ModuleManeger2�����Ӵ��ڵ�λ�ã�hButtonBoard ��hContentBoard ��hStaticBoard���Ӵ���
			MoveWindow(ModuleManeger2.hStaticBoard,5,5,cxWindow-10,cyWindow-10,TRUE);
			MoveWindow(ModuleManeger2.hButtonBoard,3,3,cxWindow-16,cyChar*2,TRUE);

			for(i=0 ; i<ModuleManeger2.countOfModule ; i++)
				MoveWindow(ModuleManeger2.pToolModule[i].hContentBoard,3,3+cyChar*2+3,cxWindow-16,cyWindow-3*2-3-cyChar*2,TRUE);
			
			//��ʾ��ǰ��ѡ���ġ����ݴ��ڡ�
			for(i=0; i<ModuleManeger2.countOfModule ;i++){
				
				if(i==idOfCurrentSubModule){
					ShowWindow(ModuleManeger2.pToolModule[i].hContentBoard,SW_SHOW);
				}
				else 
					ShowWindow(ModuleManeger2.pToolModule[i].hContentBoard,SW_HIDE);
			}
		}
		else{
		}

		break;
	case WM_PAINT:


		break;

	case WM_COMMAND:
		if( id == 0 )
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
					
				break;
			case 3: //hDeleteItemButton:

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

								//ͬʱ����countOfSignIn �� pSignInListTemp �Ա�����ʱд���ļ�
								countOfSignIn += 1;
								pSignInListTemp = (pSignIn)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn);
								pSignInListTemp[countOfSignIn-1] = SignInTemp;
								
								//ǩ���ɹ���ʾ
								wsprintf(szBuffer2,TEXT("ǩ���%s  %d-%d-%d  ǩ���ɹ���"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
								MessageBox(hwnd,szBuffer2,TEXT("ǩ����ʾ"),MB_ICONINFORMATION);
							}
						}
						break;
					}
				}
/*
				printf("countOfSignIn:%d\n",countOfSignIn);
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
				for(i=0;i<countOfSignIn;i++)
				{
					printf("N:%s %d %d %d\n",pSignInListTemp[i].name,pSignInListTemp[i].year,pSignInListTemp[i].month,pSignInListTemp[i].day);
				}
		*/		
					

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


			}

		}
		else if(id == 2)
		{

		}


		break;

	}

	return CallWindowProc (oldModuleContent, hwnd, message, wParam,lParam);
}

/*****************************************************************
*  �ڶ���ģ���µ���ģ�����������Ϣ������
******************************************************************/

/*****************************************************************
*  ģ������� �� �ײ㾲̬����̬���ڡ� ʹ�õ���Ϣ������  (�ڶ���ģ�������)
******************************************************************/

LRESULT CALLBACK ModuleManagerProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	PMODULEMANAGER pMM = GetPModuleManagerByStaticHWND(hwnd);
	CheckNullErrorAndQuit(pMM,14,TEXT("Can't Get ModuleManager By HWND in ModuleManagerProc2()!"));
	RECT rect;

	switch(message)
	{
		case WM_SIZE:

			break;

	}

	return  CallWindowProc (pMM->OldModuleManager, hwnd, message, wParam,lParam) ;
}

/*****************************************************************
*  ģ������� �� ��̬����ť���ڡ�ʹ�õ���Ϣ������ (�ڶ���ģ�������)
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
			/** ע�⣺���Ӵ������ʱ�����µĻص�������WM_CREATE ���ᱻ���ã�����ú����ĸ���Ϣ����
			CreateStaticModuleManergerBoard()��ʹ�� SendMessage(pMM.hButtonBoard,WM_CREATE,0,0); ���͵�	*/

		//���ö�ʱ�������ڻ滭�����ƶ��ĸ�������
		SetTimer (hwnd, 2, 10, NULL) ;
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
			intList = (int*)malloc(sizeof(int)*ModuleManeger2.countOfModule);
			CheckNullErrorAndQuit(intList,19,TEXT("Memory for [intList] alloc failed in ModuleButtonWindowProc2()!"));

			for(i=0 ; i< ModuleManeger2.countOfModule ; i++)
			{
				intList[i] = GetWindowTextLength (ModuleManeger2.pToolModule[i].hButton) ;
			}
	
			//��һ�ε���ʱ������������ָ��Ϊ��һ����ť
			newLeft = xButtonBegin;
			newRight = xButtonBegin + intList[0]*cxChar*2+30;
			
			firstTimeFlag = 0;
		}

		//����xButtonBegin�������ȵ����ð�ť�� λ�úʹ�С
		lengthToLeft =0;
		for(i=0 ; i< ModuleManeger2.countOfModule ; i++)
		{
			MoveWindow(ModuleManeger2.pToolModule[i].hButton,
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
		idOfCurrentSubModule = idGot < MAX_COUNT_OF_MODULE ? idGot : idOfCurrentSubModule;

		if(idGot < MAX_COUNT_OF_MODULE) //Ϊģ�鰴ť����Ϣ
		{
			for(i=0; i<pMM->countOfModule ;i++){
				if(i==idOfCurrentSubModule)
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_SHOW);
				else 
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_HIDE);
			}

			//��������ť��λ�ã�������newLeft �� newRight��
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
		
			//ȷ�������ƶ�����
			if(idOfCurrentSubModule > currentId) toRight =true;
			else 
				if(idOfCurrentSubModule < currentId) toRight = false;
			currentId = idOfCurrentSubModule;
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
*  ÿ����ģ��� ��̬�����ݴ��ڡ�ʹ�õ���Ϣ������  (�ڶ���ģ�������)
******************************************************************/

LRESULT CALLBACK ModuleContentWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	//�õ�ԭ������Ϣ����
	WNDPROC oldModuleContent = GetOldContentWndProcByContentHWND(hwnd);
	CheckNullErrorAndQuit(oldModuleContent,16,TEXT("Can't Get oldModuleContent By HWND in ModuleButtonWindowProc()!"));
	
	//�õ�ģ���ID
	int id = GetIdInModuleManagerByContentHWND(hwnd);
	if(id == -1){
		MessageBox(hwnd,TEXT("ErrorCode: 0X 0017\nCant't Get [idModule] in ModuleContentWindowProc2()"),TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
	//�õ�ģ�������ָ��
	PMODULEMANAGER pMM = GetPModuleManagerByContentHWND(hwnd);
	CheckNullErrorAndQuit(pMM,18,TEXT("Can't Get ModuleManager By ContentHWND in ModuleContentWindowProc2()!"));


	static int cxWindow,cyWindow,cxChar,cyChar,i;

	switch(message)
	{
		
	case WM_CREATE:
		/** ע�⣺���Ӵ������ʱ�����µĻص�������WM_CREATE ���ᱻ���ã�����ú����ĸ���Ϣ����
			AddNewModule()��ʹ�� SendMessage(hContentBoard,WM_CREATE,0,0); ���͵�	*/

		//��ʼ����һ����ģ��Ŀؼ�
		if(id==0)
		{
			//��ʾ��ǰ�ļ��еľ�̬��ǩ
			hStaticCurrentFloder = CreateWindow(TEXT("static"),TEXT("��ǰ�ļ���"),WS_CHILD|WS_VISIBLE|SS_CENTER,
									0,0,0,0,
									hwnd, (HMENU)1, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//������ʾ��ǰ�ļ��к����ӵ�ListBox
			hFloderListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)2,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hLinkListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)3,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//�ĸ���ӻ�ɾ����ť
			hAddFloderButton = CreateWindow(TEXT("button"),TEXT("�����ļ���"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)4, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteFloderButton = CreateWindow(TEXT("button"),TEXT("ɾ���ļ���"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)5, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
			
			hAddLinkButton = CreateWindow(TEXT("button"),TEXT("�������"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)6, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteLinkButton = CreateWindow(TEXT("button"),TEXT("ɾ������"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		}
		else if(id==1)//��ʼ���ڶ�����ģ��Ŀؼ�
		{
						//��ʾ��ǰ�ļ��еľ�̬��ǩ
			hStaticCurrentFloder2 = CreateWindow(TEXT("static"),TEXT("��ǰ�ļ���"),WS_CHILD|WS_VISIBLE|SS_CENTER,
									0,0,0,0,
									hwnd, (HMENU)8, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//������ʾ��ǰ�ļ��к����ӵ�ListBox
			hFloderListBox2 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)9,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hLinkListBox2 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)10,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//�ĸ���ӻ�ɾ����ť
			hAddFloderButton2 = CreateWindow(TEXT("button"),TEXT("�����ļ���"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)11, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteFloderButton2 = CreateWindow(TEXT("button"),TEXT("ɾ���ļ���"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)12, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
			
			hAddLinkButton2 = CreateWindow(TEXT("button"),TEXT("�������"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)13, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteLinkButton2 = CreateWindow(TEXT("button"),TEXT("ɾ������"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)14, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		}
		else
		{

		}

		break;

	case WM_SIZE:
		
		//��á����ݴ��ڡ��Ĵ�С��Ϣ�����豸������
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cyChar = LOWORD (GetDialogBaseUnits ()) ;

		//������һ����ģ��Ŀؼ���λ�úʹ�С
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
		else if(id==1)//�����ڶ�����ģ��Ŀؼ���λ�úʹ�С
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

/*****************************************************************
*          ���������г��ֵ�Edit�ؼ����µ���Ϣ������
******************************************************************/

LRESULT CALLBACK newEditProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_KEYDOWN:
		if(wParam==VK_RETURN)
			if(GetFocus()==hTimesEdit){
				SetFocus(GetParent(hwnd));
			}
		break;

	}

	return  CallWindowProc (oldEditProc, hwnd, message, wParam,lParam) ;
}

// ��ǰ������ʾ���õ�20
