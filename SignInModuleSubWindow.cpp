/******************************************************************
	�ļ�����SignInModuleSubWindow.cpp

	�ļ���������ʼ����һ��ģ��������Ӵ���hDlgBaseData �� hDlgHistogram, hDlgAdvance �Լ����ѡ��� hDlgAddItem

*********************************************************************/
#include <windows.h>
#include <stdio.h>
#include <TCHAR.H>
#include <time.h>
#include "MyDefine.h"

//ʹ��MainModuleManagerWndProc.cpp�ж����ȫ�ִ��ھ��
extern HWND hSignInListBox;
extern HWND hDlgBaseData, hDlgHistogram, hDlgAdvance;

static HWND hStaticSignRange1,hRangeListBox1 ,hStaticWord1, hStaticTimes , hStaticDaysOnce ,
       hRangeListBox2 , hLongOrShotListBox ,hTimesEdit, hStaticWord2,hStaticIntervalDays;
static HWND hStaticSignRange2,hStaticTime, hStaticMonth ,
	   hDefaultYearButton,hStaticYearLabel, hNextYearButton, hLastYearButton;
static HWND hStaticTip, hListListBox, hDeleteSignInButton, hAddSignInEdit,hAddSignInButton;

//��Ӵ��ڵľ��
static HWND hDlgAddItem ,hStaticAddTipLabel,hEditAddNewItem,hConfirmButton_AddSignIn,hCancelButton_AddSignIn;


//ʹ��ȫ�ֱ���szSubWindowClassName[] (�Ӵ������������GlobleManager.h�ж��壬��Entry.cpp�г�ʼ��)
extern TCHAR szSubWindowClassName[];

//ʹ��ȫ�ֱ���allSignIn (�洢��ǰ����ǩ�����Զ���Ľṹ�壬��MainModuleManagerWndProc.cpp����ͳ�ʼ��)
extern AllSignIn allSignIn;
extern pSignIn pSignInListTemp;  //�����������ļ���ȡ��رգ�ǩ�����ݵĸ��Ļ����ڴˣ�����ر�ʱ�Ÿ���д��
extern int countOfSignIn_All;

//ʹ��ȫ�ֱ���currentSignInSel(ָ����ǰhSignInListBox��ѡ����MainModuleManagerWndProc.cpp����ͳ�ʼ��)
extern int currentSignInSel;

//ʹ��ȫ�ֱ���DAY_IN_MONTH (��������ƽ����·���Ϣ����"MyDefine.h"�ж���)
extern int DAY_IN_MONTH[2][12];

//���ڴ���ɵ�Edit�ؼ�����Ϣ������
static WNDPROC oldEditProc[3]; //���µı༭�ؼ�Ҫʹ��ʱ����Ҫ����ָ���±�
//��ʱ�Ĵ���ǩ���ľ�̬����
static SignIn SignInTemp;	

//����ַ������ڵĺϷ��ԣ����ʵ��˵��
int checkValidate(TCHAR * string, SignIn & signInTemp,int currentIndexSel);
//�Ƚ�����ǩ����Ĵ�С��ϵ
int cmpSignIn(SignIn & s1,SignIn & s2);

//��ʼ����һ��ģ��������Ӵ���hDlgBaseData �� hDlgHistogram,hDlgAdvance
void InitializeSubWindow(HWND hwnd)
{
		//ǩ��ͳ�ƴ��ڵĴ���
		hDlgBaseData = CreateWindow(szSubWindowClassName,TEXT("��������չʾ"),WS_POPUP|WS_OVERLAPPEDWINDOW,
									200,100,400,300,
									hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		hDlgHistogram = CreateWindow(szSubWindowClassName,TEXT("��״ͼ"),WS_POPUP|WS_OVERLAPPEDWINDOW,
									200,400,400,300,
									hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		hDlgAdvance = CreateWindow(szSubWindowClassName,TEXT("�߼�����"),WS_POPUP|WS_OVERLAPPEDWINDOW,
									600,100,400,325,
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
		oldEditProc[0] = (WNDPROC)SetWindowLong(hTimesEdit,GWL_WNDPROC,(LONG)newEditProc);

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

		//���߼����á����ڵĻ���
		
		hStaticTip = CreateWindow(TEXT("static"),TEXT("��ʾ��ǰǩ������"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									5,5,400,30,
									hDlgAdvance, (HMENU)1, (HINSTANCE)GetWindowLong(hDlgAdvance,GWL_HINSTANCE),NULL);
		hListListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD_NO_SORT,
									5,40,250,250,
									hDlgAdvance, (HMENU)2, (HINSTANCE)GetWindowLong(hDlgAdvance,GWL_HINSTANCE),NULL);
		hDeleteSignInButton = CreateWindow(TEXT("button"),TEXT("ɾ����ѡǩ��"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									260,40,120,30,
									hDlgAdvance, (HMENU)3, (HINSTANCE)GetWindowLong(hDlgAdvance,GWL_HINSTANCE),NULL); 
		
		hAddSignInEdit = CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
									260,280-70,120,26,
									hDlgAdvance, (HMENU)4, (HINSTANCE)GetWindowLong(hDlgAdvance,GWL_HINSTANCE),NULL);
		oldEditProc[2] = (WNDPROC)SetWindowLong(hAddSignInEdit,GWL_WNDPROC,(LONG)newEditProc);

									
		hAddSignInButton = CreateWindow(TEXT("button"),TEXT("��ǩ"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									300,280-35,80,30,
									hDlgAdvance, (HMENU)5, (HINSTANCE)GetWindowLong(hDlgAdvance,GWL_HINSTANCE),NULL); 
}

//��ʼ����һ��ģ��� hDlgAddItem
void InitializeAddItemWindow(HWND hwnd,POINT leftTopPoint,int cyChar)
{

	//�����Ӵ��ڣ����ڸղŵó����µ�λ��
	hDlgAddItem = CreateWindowEx(WS_EX_DLGMODALFRAME,szSubWindowClassName,TEXT("���ǩ����"),WS_OVERLAPPED|WS_SYSMENU|WS_CAPTION|WS_DLGFRAME,
											leftTopPoint.x,leftTopPoint.y,300,150,
											hwnd,NULL,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);//������ΪstaticContentBoard

	hStaticAddTipLabel = CreateWindow(TEXT("static"),TEXT("�µ�ǩ����"),WS_CHILD|WS_VISIBLE|SS_LEFT,
											20,25,80,cyChar*2,
											hDlgAddItem,(HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

	hEditAddNewItem = CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,
											101,25,180,cyChar*2,
											hDlgAddItem,(HMENU)2,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
	oldEditProc[1]=(WNDPROC)SetWindowLong(hEditAddNewItem,GWL_WNDPROC,(LONG)newEditProc);

	hConfirmButton_AddSignIn = CreateWindow(TEXT("button"),TEXT("ȷ��"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
											100,30+cyChar*2+10,50,cyChar*2,
											hDlgAddItem,(HMENU)3,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

	hCancelButton_AddSignIn = CreateWindow(TEXT("button"),TEXT("ȡ��"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
											170,30+cyChar*2+10,50,cyChar*2,
											hDlgAddItem,(HMENU)4,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
	//Ĭ�ϲ�����ȷ�ϰ�ť
	EnableWindow(hConfirmButton_AddSignIn,FALSE);

	//����ɳ�ʼ����ʹ�Ի��򴰿ڿɼ�
	ShowWindow(hDlgAddItem,SW_SHOW);					
}

/*****************************************************************
*	�����Ӵ��� hDlgBaseData �� hDlgHistogram ,hDlgAdvance����Ϣ������
*   �Լ� ����ǩ����ȷ�ϴ��� hDlgAddItem ����Ϣ������
******************************************************************/

LRESULT CALLBACK SubWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
    PAINTSTRUCT ps ;
	RECT rect;
	static int xOrigin=50,yOrigin=180,widthHistogram=280,heightHistogram=160,cxBlock,cyBlock,maxCount,
				monthCountArray[12],yearToShow,defaultYearToShow,cYearInRecord;
	static int i,j,timeInputted=2,countOfSignIn_inRange,range,minIntervalDay=-1,maxIntervalDay=-1,flagForEdit;
    static int sel_RangeListBox1=-1, sel_RangeListBox2=-1, sel_LongOrShotListBox=-1,currentIndexSel;
	static int sel_ListListBox,newInsertIndex;

	SignIn signInTemp1,signInTemp2;
	int checkResult;
	bool haveInsert;

	struct tm *pTime;
	time_t secondTime;

	static COLORREF colorOfEditText;

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

			//��currentSignInSel �õ���ǰ�����ǩ�������ٵõ����Ӧ���� allSignIn.SignList �ĵ�һ��ά�ȵ�λ��
			SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
			for(i=0;i<allSignIn.countOfItem;i++)
				if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;


			//��¼��ǰ��ǩ����¼������
			cYearInRecord= defaultYearToShow - allSignIn.SignList[currentIndexSel][0].year+1;

			//�����Ϊ0��˵��û���κ�ǩ�������
			if(allSignIn.SignList[currentIndexSel][0].year==0)
				cYearInRecord=0;

			//������ʾ��ݵ�static��ǩ
			wsprintf(szBuffer,TEXT("%d"),yearToShow);
			SetWindowText(hStaticYearLabel,szBuffer);

			//�������������ť������״̬
			EnableWindow(hNextYearButton,FALSE);
			if(cYearInRecord==1 || cYearInRecord==0){
				EnableWindow(hLastYearButton,FALSE);
			}
			else{//ÿ�δ򿪴��������Σ�������һ�ε�hLastYearButton״̬��֪���п����ǽ���״̬�����Ա�����ȷ����������
				EnableWindow(hLastYearButton,TRUE);
			}
		 }
		else if(hwnd==hDlgAdvance)
		{
			//���listbox
			SendMessage(hListListBox,LB_RESETCONTENT,0,0);

			//��currentSignInSel �õ���ǰ�����ǩ�������ٵõ����Ӧ���� allSignIn.SignList �ĵ�һ��ά�ȵ�λ��
			SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
			for(i=0;i<allSignIn.countOfItem;i++)
				if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

			//����������ӵ�listbox����ʾ
			for(i=0;i<allSignIn.countInList[currentIndexSel];i++){
				wsprintf(szBuffer,TEXT("%d-%d-%d"),allSignIn.SignList[currentIndexSel][i].year,
					allSignIn.SignList[currentIndexSel][i].month,allSignIn.SignList[currentIndexSel][i].day);
				SendMessage(hListListBox,LB_INSERTSTRING,-1,(LPARAM)szBuffer);
			}

			//������ʾ
			wsprintf(szBuffer,TEXT("ǩ����: %s \t\t ǩ������: %d"),allSignIn.SignList[currentIndexSel][0].name,
				allSignIn.countInList[currentIndexSel]);
			SetWindowText(hStaticTip,szBuffer);

			sel_ListListBox = LB_ERR;
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
			
			//��currentSignInSel �õ���ǰ�����ǩ�������ٵõ����Ӧ���� allSignIn.SignList �ĵ�һ��ά�ȵ�λ��
			SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
			for(i=0;i<allSignIn.countOfItem;i++)
				if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

			for(i=0;i<allSignIn.countInList[currentIndexSel];i++)
			{
				if(allSignIn.SignList[currentIndexSel][i].year==yearToShow)
				{
					monthCountArray[allSignIn.SignList[currentIndexSel][i].month-1]++;
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
					countOfSignIn_inRange=0;
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

					//��currentSignInSel �õ���ǰ�����ǩ�������ٵõ����Ӧ���� allSignIn.SignList �ĵ�һ��ά�ȵ�λ��
					SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
					for(i=0;i<allSignIn.countOfItem;i++)
						if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

					//������ǰ������ѡ��ĵ�ǩ����
					for(i=0;i<allSignIn.countInList[currentIndexSel] ;i++)
						{
							//���ǩ������ʱ�䷶Χ�ڣ����м�¼
							if(DaysBetween(allSignIn.SignList[currentIndexSel][i],SignInTemp)<=range)
							{
								countOfSignIn_inRange+=1;
								if(countOfSignIn_inRange==1)//��¼�÷�Χ�ڵ�һ��ǩ����
									signInTemp1=allSignIn.SignList[currentIndexSel][i];
							}
						}
					if(countOfSignIn_inRange!=0)//����м�¼ǩ�����¼���һ��
							signInTemp2 = allSignIn.SignList[currentIndexSel][allSignIn.countInList[currentIndexSel]-1];

					//ֱ�Ӹ�����ʾ����
					if(countOfSignIn_inRange==0){

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

							wsprintf(szBuffer,TEXT("%d"),countOfSignIn_inRange);
							SetWindowText(hStaticTimes,szBuffer);
							wsprintf(szBuffer,TEXT("%d"),range/countOfSignIn_inRange);
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
					countOfSignIn_inRange=0;
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

						//��currentSignInSel �õ���ǰ�����ǩ�������ٵõ����Ӧ���� allSignIn.SignList �ĵ�һ��ά�ȵ�λ��
						SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
						for(i=0;i<allSignIn.countOfItem;i++)
							if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

						//������ǰ������ѡ��ĵ�ǩ����
						for(i=0;i<allSignIn.countInList[currentIndexSel] ;i++)
						{
							//���ǩ������ʱ�䷶Χ�ڣ����м�¼
							if(DaysBetween(allSignIn.SignList[currentIndexSel][i],SignInTemp)<=range)
							{
								countOfSignIn_inRange+=1;
								if(countOfSignIn_inRange==1)//��¼�÷�Χ�ڵ�һ��ǩ����
									signInTemp1=allSignIn.SignList[currentIndexSel][i];
							}
						}
						if(countOfSignIn_inRange!=0)//����м�¼ǩ�����¼���һ��
							signInTemp2 = allSignIn.SignList[currentIndexSel][allSignIn.countInList[currentIndexSel]-1];

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
							for(i=0;i<allSignIn.countInList[currentIndexSel] ;i++)
							{
								//���ǩ������ʱ�䷶Χ��
								if(DaysBetween(allSignIn.SignList[currentIndexSel][i],SignInTemp)<=range)
								{
									//�����ǰ��i ��i+timeInputted-1 ����ڵĻ����������Ƚ�
									if(i+timeInputted-1<allSignIn.countInList[currentIndexSel])
									{
										//��sel_LongOrShotListBox =0 ���N�μ��
										if(sel_LongOrShotListBox==0)
										{
											maxIntervalDay =max(maxIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentIndexSel][i],
												allSignIn.SignList[currentIndexSel][i+timeInputted-1]));
										}
										else 	//��sel_LongOrShotListBox =1 �����N�μ��
										{
											minIntervalDay =min(minIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentIndexSel][i],
												allSignIn.SignList[currentIndexSel][i+timeInputted-1]));
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
					if(countOfSignIn_inRange==0)//sel_RangeListBox2 ��δѡ��
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

		/*�������� "�߼�����" �Ի��򴰿�ʱ*/
		else if(hwnd==hDlgAdvance)
		{
			switch(LOWORD (wParam))
			{
			case 2: // hListListBox
				if(HIWORD(wParam)==LBN_SELCHANGE)
					sel_ListListBox = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);
				
				printf("HIWORD(wParam)=%d Current Sel:%d\n",HIWORD(wParam),sel_ListListBox);
				break;

			case 3: //hDeleteSignInButton
				if(sel_ListListBox ==LB_ERR){
					MessageBox(hwnd,TEXT("δѡ���κ�ǩ�����ڣ�����"),TEXT("������ʾ"),MB_ICONINFORMATION);
					break;
				}
				else
				{
					lstrcpy(signInTemp1.name,allSignIn.SignList[currentIndexSel][sel_ListListBox].name);
					signInTemp1.year = allSignIn.SignList[currentIndexSel][sel_ListListBox].year;
					signInTemp1.month = allSignIn.SignList[currentIndexSel][sel_ListListBox].month;
					signInTemp1.day = allSignIn.SignList[currentIndexSel][sel_ListListBox].day;

					wsprintf(szBuffer,TEXT("����ɾ��ǩ���%d-%d-%d\n\n�Ƿ�ȷ����"),signInTemp1.year,signInTemp1.month,signInTemp1.day);
					//û�������˵�checkResult�������շ��ؽ��
					checkResult = MessageBox(hwnd,szBuffer,TEXT("ɾ��ȷ��"),MB_ICONINFORMATION|MB_YESNO);

					//���ǿ϶��ش��򷵻�
					if(checkResult!=IDYES)
					{
						break;
					}
					
					//��allSignIn ��ɾ����ɾ������
					SignIn	*listUpdated;
					listUpdated = allSignIn.SignList[currentIndexSel];

					allSignIn.countInList[currentIndexSel] -= 1;
					allSignIn.SignList[currentIndexSel] = (SignIn *)malloc(sizeof(SignIn)*allSignIn.countInList[currentIndexSel]);

					//iָ���µ�allSignIn.SignList[currentIndexSel]
					//jָ��ɵĴ�ɾ����listUpdated
					for(j=0,i=0; j<allSignIn.countInList[currentIndexSel]+1 ;j++)
					{
						if(j==sel_ListListBox)
							continue;

						lstrcpy(allSignIn.SignList[currentIndexSel][i].name,listUpdated[j].name);
						allSignIn.SignList[currentIndexSel][i].year = listUpdated[j].year;
						allSignIn.SignList[currentIndexSel][i].month = listUpdated[j].month;
						allSignIn.SignList[currentIndexSel][i].day = listUpdated[j].day;

						i++;
					}

					free(listUpdated);

					//����allSignIn���·���
							
					countOfSignIn_All=0;
					for(i=0;i<allSignIn.countOfItem;i++) countOfSignIn_All += allSignIn.countInList[i];

					pSignInListTemp = (SignIn *)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn_All);
							
					checkResult = 0;	//û�������˵�checkResult�������շ��ؽ��
					for(i=0; i<allSignIn.countOfItem ; i++)
					{
						for(j=0; j<allSignIn.countInList[i] ; j++)
						{
							lstrcpy(pSignInListTemp[checkResult].name,allSignIn.SignList[i][j].name);
							pSignInListTemp[checkResult].year = allSignIn.SignList[i][j].year;
							pSignInListTemp[checkResult].month = allSignIn.SignList[i][j].month;
							pSignInListTemp[checkResult].day = allSignIn.SignList[i][j].day;

							checkResult+=1;
						}
					}	

					
					//����ListListBox �� ������ʾ
					SendMessage (hListListBox, LB_DELETESTRING,sel_ListListBox, 0) ;
					SendMessage (hListListBox, LB_SETCURSEL,-1, 0) ;
					sel_ListListBox = LB_ERR;
					wsprintf(szBuffer,TEXT("ǩ����: %s \t\t ǩ������: %d"),allSignIn.SignList[currentIndexSel][0].name,
						allSignIn.countInList[currentIndexSel]);
					SetWindowText(hStaticTip,szBuffer);

					//��ʾ�ɹ�
					wsprintf(szBuffer2,TEXT("ɾ���ɹ�:%d-%d-%d"),signInTemp1.year,signInTemp1.month,signInTemp1.day);
					MessageBox(hwnd,szBuffer2,TEXT("ɾ����ʾ"),MB_ICONINFORMATION);

				}
				break;

			case 5: //hAddSignInButton
				GetWindowText(hAddSignInEdit,szBuffer,1+SendMessage(hAddSignInEdit,EM_LINELENGTH,-1,0));
				
				checkResult = checkValidate(szBuffer,signInTemp1,currentIndexSel);

				if(checkResult<0)
				{
					if(checkResult==-1)//��ʽ����ȷ
						wsprintf(szBuffer2,TEXT("��Ч���������룺%s\n\n��ȷ��ʽΪ��2XXX-(X)X-(X)X ! (XΪ����)"),szBuffer);
					else if(checkResult==-2)//�����ϲ����ڸ�����
						wsprintf(szBuffer2,TEXT("��Ч���������룺%s\n\n�������޸�����!"),szBuffer);
					else if(checkResult==-3)//��ʽ��ȷ������ʱ�ڳ�ǰ
						wsprintf(szBuffer2,TEXT("��Ч���������룺%s \n\n����С�ڵ��ڵ�ǰ����:%d-%d-%d"),
							szBuffer,signInTemp1.year,signInTemp1.month,signInTemp1.day);
					else if(checkResult==-4)//��ʽ��ȷ�������ظ���
						wsprintf(szBuffer2,TEXT("ǩ���ظ��ˣ�%s\n\n"),szBuffer);

					MessageBox(hwnd,szBuffer2,TEXT("������ʾ"),MB_ICONINFORMATION);
				}
				else
				{
					wsprintf(szBuffer,TEXT("�������ǩ���%d-%d-%d\n\n�Ƿ�ȷ����"),signInTemp1.year,signInTemp1.month,signInTemp1.day);
					//û�������˵�checkResult�������շ��ؽ��
					checkResult = MessageBox(hwnd,szBuffer,TEXT("��ǩȷ��"),MB_ICONINFORMATION|MB_YESNO);

					//���ǿ϶��ش��򷵻�
					if(checkResult!=IDYES)
					{
						break;
					}

					lstrcpy(signInTemp1.name,allSignIn.SignList[currentIndexSel][0].name);

					//����allSignIn ���� ��������
					if(allSignIn.countInList[currentIndexSel]==0)
					{
						allSignIn.countInList[currentIndexSel] += 1;
						allSignIn.SignList[currentIndexSel][0].year = signInTemp1.year;
						allSignIn.SignList[currentIndexSel][0].month = signInTemp1.month;
						allSignIn.SignList[currentIndexSel][0].day = signInTemp1.day;
						newInsertIndex = 0;

					}else
					{
						SignIn	*listUpdated;
						listUpdated = allSignIn.SignList[currentIndexSel];

						allSignIn.countInList[currentIndexSel] += 1;
						allSignIn.SignList[currentIndexSel] = (SignIn *)malloc(sizeof(SignIn)*allSignIn.countInList[currentIndexSel]);

						//iָ���µ�allSignIn.SignList[currentIndexSel]
						//jָ��ɵĴ���ӵ�listUpdated
						haveInsert = false;
						for(i=0,j=0; i<allSignIn.countInList[currentIndexSel];i++)
						{
							//�����һ�γ���С������ǩ���������������ӵ���ƽ�ȥ
							if(cmpSignIn(signInTemp1,listUpdated[j])<0 && !haveInsert)
							{
								lstrcpy(allSignIn.SignList[currentIndexSel][i].name,signInTemp1.name);
								allSignIn.SignList[currentIndexSel][i].year = signInTemp1.year;
								allSignIn.SignList[currentIndexSel][i].month = signInTemp1.month;
								allSignIn.SignList[currentIndexSel][i].day = signInTemp1.day;
								newInsertIndex = i;
								haveInsert=true;

								continue; // ָ������µ�listUpdated��j������
							}
							//��j��Ӧ��listUpdate ���Ƶ� i ��Ӧ��allSignIn.SignList[currentIndexSel] ��ȥ
							if(j!=allSignIn.countInList[currentIndexSel]-1)
							{
								lstrcpy(allSignIn.SignList[currentIndexSel][i].name,listUpdated[j].name);
								allSignIn.SignList[currentIndexSel][i].year = listUpdated[j].year;
								allSignIn.SignList[currentIndexSel][i].month = listUpdated[j].month;
								allSignIn.SignList[currentIndexSel][i].day = listUpdated[j].day;
							
								j++;
							}
							else
							{
								lstrcpy(allSignIn.SignList[currentIndexSel][i].name,signInTemp1.name);
								allSignIn.SignList[currentIndexSel][i].year = signInTemp1.year;
								allSignIn.SignList[currentIndexSel][i].month = signInTemp1.month;
								allSignIn.SignList[currentIndexSel][i].day = signInTemp1.day;
								newInsertIndex = i;
							}
						}
						free(listUpdated);
					}

					//����allSignIn���·���
							
					countOfSignIn_All=0;
					for(i=0;i<allSignIn.countOfItem;i++) countOfSignIn_All += allSignIn.countInList[i];

					pSignInListTemp = (SignIn *)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn_All);
							
					checkResult = 0;	//û�������˵�checkResult�������շ��ؽ��
					for(i=0; i<allSignIn.countOfItem ; i++)
					{
						for(j=0; j<allSignIn.countInList[i] ; j++)
						{
							lstrcpy(pSignInListTemp[checkResult].name,allSignIn.SignList[i][j].name);
							pSignInListTemp[checkResult].year = allSignIn.SignList[i][j].year;
							pSignInListTemp[checkResult].month = allSignIn.SignList[i][j].month;
							pSignInListTemp[checkResult].day = allSignIn.SignList[i][j].day;

							checkResult+=1;
						}
					}	

					
					//����ListListBox �� ������ʾ
					wsprintf(szBuffer,TEXT("%d-%d-%d"),signInTemp1.year,signInTemp1.month,signInTemp1.day);
					SendMessage(hListListBox,LB_INSERTSTRING,newInsertIndex, (LPARAM)szBuffer);
					SendMessage (hListListBox, LB_SETCURSEL,newInsertIndex, 0) ;
					sel_ListListBox = newInsertIndex;

					wsprintf(szBuffer,TEXT("ǩ����: %s \t\t ǩ������: %d"),allSignIn.SignList[currentIndexSel][0].name,
						allSignIn.countInList[currentIndexSel]);
					SetWindowText(hStaticTip,szBuffer);


					//��ʾ�ɹ�
					wsprintf(szBuffer2,TEXT("��ǩ�ɹ�:%d-%d-%d"),signInTemp1.year,signInTemp1.month,signInTemp1.day);
					MessageBox(hwnd,szBuffer2,TEXT("��ǩ��ʾ"),MB_ICONINFORMATION);
				}
				break;

			}
		}
		
		/*�������� ����ǩ�� �Ի��򴰿�ʱ*/
		else if(hwnd==hDlgAddItem)
		{
			switch(LOWORD (wParam))
			{
				case 2://	hEditAddNewItem
					if(HIWORD (wParam)==EN_UPDATE)//EN_UPDATE ��������ı���򸸴��ڷ���WM_CTLCOLOREDIT
					{
						GetWindowText(hEditAddNewItem,szBuffer,256);
						for(i=0;i<allSignIn.countOfItem;i++)
						{
							if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)//��������ɫ��ʾ
							{	
								EnableWindow(hConfirmButton_AddSignIn,FALSE);
								//�ı�������ɫΪ����ɫ 
								colorOfEditText = RGB(150,0,0);
								break;
							}
						}
						
						if(i==allSignIn.countOfItem)//����ͬ��ƥ��
						{
							//������������������
							if(szBuffer[0]=='\0')
							{
								EnableWindow(hConfirmButton_AddSignIn,FALSE);
								colorOfEditText = RGB(150,0,0);
								break;
							}

							//���Ϲ��򣬸ı�������ɫΪ����ɫ
							EnableWindow(hConfirmButton_AddSignIn,TRUE);
							colorOfEditText = RGB(0,150,0);
						}
					}
					break;
				case 3://	hConfirmButton_AddSignIn
					GetWindowText(hEditAddNewItem,szBuffer,256);

					//��allSignIn �ṹ���·���һ���ڴ���������������
					allSignIn.countOfItem += 1;
					allSignIn.countInList = (int*)realloc(allSignIn.countInList,sizeof(int)*allSignIn.countOfItem);
					allSignIn.countInList[allSignIn.countOfItem-1]=0;
					allSignIn.SignList = (pSignIn*)realloc(allSignIn.SignList,sizeof(pSignIn)*allSignIn.countOfItem);
					allSignIn.SignList[allSignIn.countOfItem-1] = (pSignIn)malloc(sizeof(SignIn)*1);
					lstrcpy(allSignIn.SignList[allSignIn.countOfItem-1][0].name,szBuffer);
					allSignIn.SignList[allSignIn.countOfItem-1][0].year =0;
					allSignIn.SignList[allSignIn.countOfItem-1][0].month =0;
					allSignIn.SignList[allSignIn.countOfItem-1][0].day =0;

					//����ȥ����countOfSignIn_All �� pSignInListTemp ��ֻ��ǩ����֮���Ա�����ʱд���ļ�

					//������ʾǩ�����ListBox
					SendMessage (hSignInListBox, LB_ADDSTRING, 0, (LPARAM) &allSignIn.SignList[allSignIn.countOfItem-1][0].name) ;
					
					//�������ڷ��͹ر���Ϣ
					SendMessage(hDlgAddItem,WM_CLOSE,0,0);

					break;
				case 4://	hCancelButton_AddSignIn
					//�������ڷ��͹ر���Ϣ
					SendMessage(hDlgAddItem,WM_CLOSE,0,0);

					break;
			}
		}
		return 0;
	case  WM_CTLCOLOREDIT:
		if((HWND)lParam==hEditAddNewItem){
			//����colorOfEditText��ɫ������
			SetTextColor((HDC)wParam,colorOfEditText);
        
           return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;
	case WM_CLOSE:
		if(hwnd==hDlgAddItem)
			break; //����� ����ǩ�� �Ի��򣬽���DefWindowProc (hwnd, message, wParam, lParam) ;Ĭ�ϴ������٣���

		ShowWindow(hwnd,SW_HIDE);
		
		return 0;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}


/*****************************************************************
*          ��һ��ģ�������г��ֵ�Edit�ؼ����µ���Ϣ������
******************************************************************/

LRESULT CALLBACK newEditProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldEditProcChosen;
//	HDC hdc;

	switch(message)
	{
	case WM_KEYDOWN:
		if(wParam==VK_RETURN){//��׽Edit�ؼ��Ļس��¼�

			//ʹ��Ӧ��Edit����ʧȥ���㣬�Ӷ��ڸ����ڵõ���EN_KILLFOCUS�¼�
			if(GetFocus()==hTimesEdit){
				SetFocus(GetParent(hwnd));
			}
			if(GetFocus()==hEditAddNewItem){
				SetFocus(GetParent(hwnd));
			}
		}
		break;

	}

	//���ݵ�����Ϣ��Edit����ѡ�������Ӧ�Ļص�����
	if(hwnd==hTimesEdit)	
		oldEditProcChosen = oldEditProc[0];
	else if(hwnd==hEditAddNewItem)
		oldEditProcChosen = oldEditProc[1];
	else if(hwnd==hAddSignInEdit)
		oldEditProcChosen = oldEditProc[2];


	return  CallWindowProc (oldEditProcChosen, hwnd, message, wParam,lParam) ;
}


//ʹ�õ�һЩ����

bool isDigit(TCHAR ch)
{
	if(ch>='0' && ch <= '9')return true;
	else return false;
}

//����ַ����ĸ�ʽ�Ƿ�Ϸ���ʱ���Ƿ�Ϸ���ʱ���Ƿ��ظ�
//return 1 ��ʾ�Ϸ������ظ�			 signInTemp ���� ��ʽ�����ǩ��
//return -1 ��ʽ����ȷ				 signInTemp ��������Ϣ
//return -2 α����					 signInTemp ��������Ϣ
//return -3 ��ʽ��ȷ������ʱ�ڳ�ǰ   signInTemp ���ص�ǰ���ڵĵ���Ϣ
//return -4 ��ʽ��ȷ�������ظ���	 signInTemp ��������Ϣ
int checkValidate(TCHAR * string, SignIn & signInTemp,int currentIndexSel)
{
	struct tm *pTime;
	time_t secondTime;
	SignIn signInTaday;
	TCHAR year[5],month[3],day[3];
	int nYear,nMonth,nDay;
	int i,j;

	//��ȡת��Ϊ������Ϣ����ʽ�����򷵻�-1
	for(i=0,j=0; string[i]!='-' ; i++,j++)
	{
		year[j]=string[i];
		if(!isDigit(year[j]))return -1;
		if(string[i]=='\0')return -1;
	}
	year[j]='\0';
	if(lstrlen(year)!=4)return -1;
	for(i++,j=0;string[i]!='-'; i++,j++)
	{
		month[j]=string[i];
		if(!isDigit(month[j]))return -1;
		if(string[i]=='\0')return -1;
	}
	month[j]='\0';
	if(lstrlen(month)!=1 && lstrlen(month)!=2)return -1;

	for(i++,j=0;string[i]!='\0';i++,j++)
	{
		day[j]=string[i];
		if(!isDigit(day[j]))return -1;
	}
	day[j]='\0';
	if(lstrlen(day)!=1 && lstrlen(day)!=2)return -1;
	
	nYear =_ttoi(year);
	nMonth =_ttoi(month); 
	nDay =_ttoi(day);
	
	//����Ƿ�Ϊ��ʵ���ڵ����ڣ��������򷵻�-2
	if(nMonth<=0 || nMonth>12)
		return -2;
	if(nDay<=0 || nDay > DAY_IN_MONTH[IsLeapYear(nYear)][nMonth-1] )
		return -2;

	//��������Ƿ��ظ�,�ظ�����-4

	//��õ�ǰϵͳ��ʱ����Ϣ
	secondTime = time(NULL);
	pTime = localtime(&secondTime);
	//��ǩ�����ݽṹ���ݴ浱ǰʱ����Ϣ
	signInTemp.year  = signInTaday.year = 1900 + pTime->tm_year;
	signInTemp.month =signInTaday.month = pTime->tm_mon+1;
	signInTemp.day   =signInTaday.day   = pTime->tm_mday;
	

	for(i=0;i<allSignIn.countInList[currentIndexSel];i++)
	{
		if(nYear==allSignIn.SignList[currentIndexSel][i].year &&
		   nMonth==allSignIn.SignList[currentIndexSel][i].month &&
		   nDay==allSignIn.SignList[currentIndexSel][i].day )
		return -4;
	}

	//��������Ƿ�ǰ,��ǰ�򷵻�-3, ����ǰ����1
	if(nYear>signInTaday.year)
		return -3;
	
	if(nYear<signInTaday.year)
	{
		signInTemp.year  =nYear;
		signInTemp.month =nMonth;
		signInTemp.day   =nDay;
		return 1;
	}
	else// nYear==signInTaday.year
	{
		if(nMonth>signInTaday.month)
			return -3;
		if(nMonth<signInTaday.month)
		{
			signInTemp.year  =nYear;
			signInTemp.month =nMonth;
			signInTemp.day   =nDay;
			return 1;
		}
		else// nMonth==signInTaday.month
		{
			if(nDay>signInTaday.day)
				return -3;
			else //nDay<signInTaday.day
			{
				signInTemp.year  =nYear;
				signInTemp.month =nMonth;
				signInTemp.day   =nDay;
				return 1;
			}
		}
	}

}

//�Ƚ��������ȵ�ǩ����Ĵ�С��ϵ
int cmpSignIn(SignIn & s1,SignIn & s2)
{
	if(s1.year < s2.year) return -1;
	else if(s1.year > s2.year)return 1;
	else 
	{
		if(s1.month<s2.month)return -1;
		else if(s1.month > s2.month)return 1;
		else
		{
			if(s1.day < s2.day) return -1;
			else return 1;
		}
	}
}