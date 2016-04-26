/******************************************************************
	文件名：SignInModuleSubWindow.cpp

	文件描述：初始化第一个模块的两个子窗口hDlgBaseData 和 hDlgHistogram 以及添加选项窗口 hDlgAddItem

*********************************************************************/
#include <windows.h>
#include <time.h>
#include "MyDefine.h"

//使用MainModuleManagerWndProc.cpp中定义的全局窗口句柄
extern HWND hSignInListBox;
extern HWND hDlgBaseData, hDlgHistogram;

static HWND hStaticSignRange1,hRangeListBox1 ,hStaticWord1, hStaticTimes , hStaticDaysOnce ,
       hRangeListBox2 , hLongOrShotListBox ,hTimesEdit, hStaticWord2,hStaticIntervalDays;
static HWND hStaticSignRange2,hStaticTime, hStaticMonth , *hStaticTimesNumber, *hStaticMonthNumber,
	   hDefaultYearButton,hStaticYearLabel, hNextYearButton, hLastYearButton;
//添加窗口的句柄
static HWND hDlgAddItem ,hStaticAddTipLabel,hEditAddNewItem,hConfirmButton_AddSignIn,hCancelButton_AddSignIn;


//使用全局变量szSubWindowClassName[] (子窗口类别名，在GlobleManager.h中定义，在Entry.cpp中初始化)
extern TCHAR szSubWindowClassName[];

//使用全局变量allSignIn (存储当前所有签到的自定义的结构体，在MainModuleManagerWndProc.cpp定义和初始化)
extern AllSignIn allSignIn;
//使用全局变量currentSignInSel(指出当前hSignInListBox所选，在MainModuleManagerWndProc.cpp定义和初始化)
extern int currentSignInSel;

//用于储存旧的Edit控件的消息处理函数
static WNDPROC oldEditProc[2]; //有新的编辑控件要使用时，需要重新指定下标
//临时的储存签到的静态变量
static SignIn SignInTemp;	

//初始化第一个模块的两个子窗口hDlgBaseData 和 hDlgHistogram
void InitializeSubWindow(HWND hwnd)
{
		//签到统计窗口的创建
		hDlgBaseData = CreateWindow(szSubWindowClassName,TEXT("基本数据展示"),WS_POPUP|WS_OVERLAPPEDWINDOW,
									200,100,400,300,
									hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		hDlgHistogram = CreateWindow(szSubWindowClassName,TEXT("柱状图"),WS_POPUP|WS_OVERLAPPEDWINDOW,
									230,130,400,300,
									hwnd, NULL, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

		//统计窗口"基本数据窗口"的绘制
		hStaticSignRange1 = CreateWindow(TEXT("static"),TEXT("显示指定范围内的签到数据范围"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									10,2,400,30,
									hDlgBaseData, (HMENU)10, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);
		hStaticSignRange2 = CreateWindow(TEXT("static"),TEXT("显示指定范围内的签到数据范围"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									10,120,400,30,
									hDlgBaseData, (HMENU)11, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

		hRangeListBox1 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD_NO_SORT,
									20,35,60,70,
									hDlgBaseData, (HMENU)1, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
		hStaticWord1 = CreateWindow(TEXT("static"),TEXT(" 共 _____ 次, 平均 _____ 天1次"),WS_CHILD|WS_VISIBLE|SS_LEFT,
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
				
		hStaticWord2 = CreateWindow(TEXT("static"),TEXT("  _____ 次间隔天数："),WS_CHILD|WS_VISIBLE|SS_LEFT,
									155,150+52,200,26,
									hDlgBaseData, (HMENU)8, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

		hTimesEdit = CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT,
									160,150+52,40,26,
									hDlgBaseData, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		oldEditProc[0] = (WNDPROC)SetWindowLong(hTimesEdit,GWL_WNDPROC,(LONG)newEditProc);

		hStaticIntervalDays = CreateWindow(TEXT("static"),TEXT(""),WS_CHILD|WS_VISIBLE|SS_CENTER,
									300,150+52,40,26,
									hDlgBaseData, (HMENU)9, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);
				
		//给hDlgBaseData发送一个自定义的消息，以完成窗口的进一步的初始化
		SendMessage(hDlgBaseData,WM_USER+1,0,0);

		//统计窗口"柱状图窗口"的绘制
		hStaticTime = CreateWindow(TEXT("static"),TEXT("次数"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									5,20,40,30,
									hDlgHistogram, (HMENU)1, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);

		hStaticMonth = CreateWindow(TEXT("static"),TEXT("月份"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									345,170,40,30,
									hDlgHistogram, (HMENU)2, (HINSTANCE)GetWindowLong(hDlgBaseData,GWL_HINSTANCE),NULL);
		//	 *hStaticTimesNumber, *hStaticMonthNumber

		hDefaultYearButton = CreateWindow(TEXT("button"),TEXT("默认年份"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									20,220,80,30,
									hDlgHistogram, (HMENU)4, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL);

		hStaticYearLabel = CreateWindow(TEXT("static"),TEXT(""),WS_CHILD|WS_VISIBLE|SS_CENTER,
									110,220,60,30,
									hDlgHistogram, (HMENU)5, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL);

		hLastYearButton = CreateWindow(TEXT("button"),TEXT("上一年"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									200,220,60,30,
									hDlgHistogram, (HMENU)6, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL); 

		hNextYearButton = CreateWindow(TEXT("button"),TEXT("下一年"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									260,220,60,30,
									hDlgHistogram, (HMENU)7, (HINSTANCE)GetWindowLong(hDlgHistogram,GWL_HINSTANCE),NULL);
}

//初始化第一个模块的 hDlgAddItem
void InitializeAddItemWindow(HWND hwnd,POINT leftTopPoint,int cyChar)
{

	//创建子窗口，置于刚才得出的新的位置
	hDlgAddItem = CreateWindow(szSubWindowClassName,TEXT("添加签到项"),WS_OVERLAPPED|WS_SYSMENU|WS_CAPTION|WS_DLGFRAME,
											leftTopPoint.x,leftTopPoint.y,300,150,
											hwnd,NULL,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);//父窗口为staticContentBoard

	hStaticAddTipLabel = CreateWindow(TEXT("static"),TEXT("新的签到名"),WS_CHILD|WS_VISIBLE|SS_LEFT,
											20,25,80,cyChar*2,
											hDlgAddItem,(HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

	hEditAddNewItem = CreateWindow(TEXT("edit"),NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL,
											101,25,180,cyChar*2,
											hDlgAddItem,(HMENU)2,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
	oldEditProc[1]=(WNDPROC)SetWindowLong(hEditAddNewItem,GWL_WNDPROC,(LONG)newEditProc);

	hConfirmButton_AddSignIn = CreateWindow(TEXT("button"),TEXT("确认"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
											100,30+cyChar*2+10,50,cyChar*2,
											hDlgAddItem,(HMENU)3,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

	hCancelButton_AddSignIn = CreateWindow(TEXT("button"),TEXT("取消"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
											170,30+cyChar*2+10,50,cyChar*2,
											hDlgAddItem,(HMENU)4,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
	//默认不启用确认按钮
	EnableWindow(hConfirmButton_AddSignIn,FALSE);

	//已完成初始化，使对话框窗口可见
	ShowWindow(hDlgAddItem,SW_SHOW);					
}

/*****************************************************************
*	两个子窗口 hDlgBaseData 和 hDlgHistogram 的消息处理函数
*   以及 新添签到的确认窗口 hDlgAddItem 的消息处理函数
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

	SignIn signInTemp1,signInTemp2;
	
	struct tm *pTime;
	time_t secondTime;

	static COLORREF colorOfEditText;

	switch(message)
	{
	case WM_USER+1://自定义消息，可用于创建时窗口时调用
		 if(hwnd==hDlgBaseData)  
		 {
			//初始化其所有List
			wsprintf(szBuffer,"%s",TEXT("一周内"));
			SendMessage (hRangeListBox1, LB_ADDSTRING, 0, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("一月内"));
			SendMessage (hRangeListBox1, LB_ADDSTRING, 1, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("一年内"));
			SendMessage (hRangeListBox1, LB_ADDSTRING, 2, (LPARAM) szBuffer) ;
			
			wsprintf(szBuffer,"%s",TEXT("一年内"));
			SendMessage (hRangeListBox2, LB_ADDSTRING, 0, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("一月内"));
			SendMessage (hRangeListBox2, LB_ADDSTRING, 1, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("一周内"));
			SendMessage (hRangeListBox2, LB_ADDSTRING, 2, (LPARAM) szBuffer) ;
			
			wsprintf(szBuffer,"%s",TEXT("最长"));
			SendMessage (hLongOrShotListBox, LB_ADDSTRING, 0, (LPARAM) szBuffer) ;
			wsprintf(szBuffer,"%s",TEXT("最短"));
			SendMessage (hLongOrShotListBox, LB_ADDSTRING, 1, (LPARAM) szBuffer) ;

			//hTimesEdit 的内容默认为“2”
			wsprintf(szBuffer,"%s",TEXT("2"));
			SetWindowText(hTimesEdit,szBuffer);

			//设置当前各个ListBox所选为 LB_ERR，即没选中状态
			sel_RangeListBox1 = sel_RangeListBox2 = sel_LongOrShotListBox = LB_ERR;
		 }
		 break;
		 
	case WM_USER+2://在每次进入的时候初始化窗口内容
		
		if(hwnd==hDlgBaseData)
		{
			//设置当前各个ListBox所选为 LB_ERR，即没选中状态
			sel_RangeListBox1 = sel_RangeListBox2 = sel_LongOrShotListBox = LB_ERR;

			//初始化选项
			SendMessage (hRangeListBox1, LB_SETCURSEL, -1, 0) ;
			SendMessage (hRangeListBox2, LB_SETCURSEL, -1, 0) ;
			SendMessage (hLongOrShotListBox, LB_SETCURSEL, -1, 0) ;

			//初始化各个标签
			wsprintf(szBuffer,TEXT("显示指定范围内的签到数据范围1"));
			SetWindowText(hStaticSignRange1,szBuffer);
			wsprintf(szBuffer,TEXT("显示指定范围内的签到数据范围2"));
			SetWindowText(hStaticSignRange2,szBuffer);

			wsprintf(szBuffer,TEXT(""));
			SetWindowText(hStaticTimes,szBuffer);
			SetWindowText(hStaticDaysOnce,szBuffer);
			SetWindowText(hStaticIntervalDays,szBuffer);
			wsprintf(szBuffer,TEXT("2"));
			SetWindowText(hTimesEdit,szBuffer);//hTimesEdit在很多时候会发送消息到WM_COMMAND到父窗口上
		}
		else if(hwnd==hDlgHistogram) 
		 {
			//获得当前系统的时间信息
			secondTime = time(NULL);
			pTime = localtime(&secondTime);
			//用签到内容结构体暂存当前时间信息
			SignInTemp.year = 1900 + pTime->tm_year;
			SignInTemp.month = pTime->tm_mon+1;
			SignInTemp.day = pTime->tm_mday;
			
			//设定默认显示年份，指定当前要显示的年份为默认
			defaultYearToShow = SignInTemp.year;
			yearToShow =defaultYearToShow;

			//由currentSignInSel 得到当前具体的签到名，再得到其对应的在 allSignIn.SignList 的第一个维度的位置
			SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
			for(i=0;i<allSignIn.countOfItem;i++)
				if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;


			//记录当前有签到记录的年数
			cYearInRecord= defaultYearToShow - allSignIn.SignList[currentIndexSel][0].year+1;

			//更新显示年份的static标签
			wsprintf(szBuffer,TEXT("%d"),yearToShow);
			SetWindowText(hStaticYearLabel,szBuffer);

			//根据情况决定按钮的启用状态
			EnableWindow(hNextYearButton,FALSE);
			if(cYearInRecord==1){
				EnableWindow(hLastYearButton,FALSE);
			}
			else{//由于上一次的签到项状态不知，有可能是禁用状态，所以必须启用
				EnableWindow(hLastYearButton,TRUE);
			}
		 }
		break;
	case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps) ;
		if(hwnd==hDlgHistogram)
		{
			//画坐标轴
			MoveToEx (hdc, xOrigin, yOrigin, NULL) ;
			LineTo (hdc, xOrigin, yOrigin-heightHistogram-10) ;
			MoveToEx (hdc, xOrigin, yOrigin, NULL) ;
			LineTo (hdc, xOrigin+widthHistogram+10, yOrigin);

			//结合指定年份的签到记录得出绘制柱状图的信息
			cxBlock = widthHistogram/(12+1);
			maxCount = 0;
			memset(monthCountArray,0,12*sizeof(int));
			
			//由currentSignInSel 得到当前具体的签到名，再得到其对应的在 allSignIn.SignList 的第一个维度的位置
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

				//绘制柱状图
				for(i=0;i<12;i++)
				{
					SetRect(&rect,xOrigin+(i+1)*cxBlock-cxBlock/3,yOrigin-cyBlock*monthCountArray[i],
								xOrigin+(i+1)*cxBlock+cxBlock/3,yOrigin);
					//如果有数据就画
					if(monthCountArray[i]!=0){
						FillRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
						TextOut(hdc,rect.left+2,rect.top-20,szBuffer,wsprintf(szBuffer,TEXT("%d"),monthCountArray[i]));
					}
					
					TextOut(hdc,rect.left+2,rect.bottom+5,szBuffer,wsprintf(szBuffer,TEXT("%d"),i+1));
				}
			}
			else
			{
				TextOut(hdc,xOrigin+50,yOrigin-50,szBuffer,wsprintf(szBuffer,TEXT("该年没有签到数据！")));
			}
/*
static HWND hRangeListBox1 ,hStaticWord1, hStaticTimes , hStaticDaysOnce ,
       hRangeListBox2 , hLongOrShotListBox ,hTimesEdit, hStaticWord2,hStaticIntervalDays;
  */
		}
        
         EndPaint (hwnd, &ps) ;

		break;

	case WM_COMMAND:
				//获得当前系统的时间信息
				secondTime = time(NULL);
				pTime = localtime(&secondTime);
				//用签到内容结构体暂存当前时间信息
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

					//直接计算出对应时间内 有多少次签到，和平均多少天一次，并直接更新显示数据
					countOfSignIn_inRange=0;
					range = -1;
					if(sel_RangeListBox1==0) //一周
					{
						range=7;
					}
					else if(sel_RangeListBox1==1)//一月
					{
						range=30;
					}
					else if(sel_RangeListBox1==2)//一年
					{
						range=365;
					}

					//由currentSignInSel 得到当前具体的签到名，再得到其对应的在 allSignIn.SignList 的第一个维度的位置
					SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
					for(i=0;i<allSignIn.countOfItem;i++)
						if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

					//遍历当前所有所选项的的签到项
					for(i=0;i<allSignIn.countInList[currentIndexSel] ;i++)
						{
							//如果签到项在时间范围内，进行记录
							if(DaysBetween(allSignIn.SignList[currentIndexSel][i],SignInTemp)<=range)
							{
								countOfSignIn_inRange+=1;
								if(countOfSignIn_inRange==1)//记录该范围内第一个签到项
									signInTemp1=allSignIn.SignList[currentIndexSel][i];
							}
						}
					if(countOfSignIn_inRange!=0)//如果有记录签到项记录最后一个
							signInTemp2 = allSignIn.SignList[currentIndexSel][allSignIn.countInList[currentIndexSel]-1];

					//直接更新显示数据
					if(countOfSignIn_inRange==0){

						SetWindowText(hStaticSignRange1,TEXT("该时间范围内没有录入的签到项"));
						wsprintf(szBuffer,TEXT("%d"),0);
						SetWindowText(hStaticTimes,szBuffer);
						wsprintf(szBuffer,TEXT("%d"),-1);
						SetWindowText(hStaticDaysOnce,szBuffer);
					}
					else{
							wsprintf(szBuffer,TEXT("从已有记录的签到项 %d-%d-%d  ~ %d-%d-%d "),
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

					//在case 7中，根据hLongOrShotListBox、和hTimeEdit 计算出对应时间内 最短或最长 N次间隔天数，并直接更新显示数据
					break;
				}
				
				//穿过执行

			case 6://hLongOrShotListBox
				if(LOWORD (wParam)==6)
				switch(HIWORD(wParam))
				{
				case LBN_SELCHANGE:
					sel_LongOrShotListBox = SendMessage((HWND)lParam,LB_GETCURSEL,0,0);

					//在case 7中，根据hRangeListBox2、和hTimeEdit 计算出对应时间内 最短或最长 N次间隔天数，并直接更新显示数据
					break;
				}

				//穿过执行
			case 7://hTimesEdit
					if(LOWORD (wParam)==7)
					if (HIWORD (wParam) == EN_KILLFOCUS ){
						GetWindowText(hTimesEdit,szBuffer,256);
						timeInputted= atoi(szBuffer);
						if(timeInputted<2 || timeInputted>100)
						{
							wsprintf(szBuffer,"%s",TEXT("2"));
							SetWindowText(hTimesEdit,szBuffer);
							MessageBox(hwnd,TEXT("输入无效！\n\n必须为2-100的整数"),TEXT("输入提示"),MB_ICONINFORMATION);
							return 0;
						}
					}
					//case 5,6,7:
					//根据hRangeListBox2 和hLongOrShotListBox 计算出对应时间内 最短或最长 N次间隔天数，并直接更新显示数据
					countOfSignIn_inRange=0;
					range = -1;
					flagForEdit = 0;//初始为0,如果没进入则表明，未选择RangeListBox2，则不更新hStaticSignRange2
					if(sel_RangeListBox2!=-1)//如果sel_RangeListBox2已经选择
					{
						flagForEdit=1;
						//更新 hStaticSignRange2 对应的数据
						if(sel_RangeListBox2==0) //一年
						{
							range=365;
						}
						else if(sel_RangeListBox2==1)//一月
						{
							range=30;
						}
						else if(sel_RangeListBox2==2)//一周
						{
							range=7;
						}

						//由currentSignInSel 得到当前具体的签到名，再得到其对应的在 allSignIn.SignList 的第一个维度的位置
						SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
						for(i=0;i<allSignIn.countOfItem;i++)
							if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)currentIndexSel=i;

						//遍历当前所有所选项的的签到项
						for(i=0;i<allSignIn.countInList[currentIndexSel] ;i++)
						{
							//如果签到项在时间范围内，进行记录
							if(DaysBetween(allSignIn.SignList[currentIndexSel][i],SignInTemp)<=range)
							{
								countOfSignIn_inRange+=1;
								if(countOfSignIn_inRange==1)//记录该范围内第一个签到项
									signInTemp1=allSignIn.SignList[currentIndexSel][i];
							}
						}
						if(countOfSignIn_inRange!=0)//如果有记录签到项记录最后一个
							signInTemp2 = allSignIn.SignList[currentIndexSel][allSignIn.countInList[currentIndexSel]-1];

						//在sel_RangeListBox2已经选择的基础上
						if(sel_LongOrShotListBox==-1)//如果sel_LongOrShotListBox还未选择
						{
							wsprintf(szBuffer,TEXT(""));
							SetWindowText(hStaticIntervalDays,szBuffer);
						}
						else //如果sel_LongOrShotListBox已经选择
						{
							minIntervalDay = 2147483647;
							maxIntervalDay = -1;
							//遍历当前所有所选项的的签到项
							for(i=0;i<allSignIn.countInList[currentIndexSel] ;i++)
							{
								//如果签到项在时间范围内
								if(DaysBetween(allSignIn.SignList[currentIndexSel][i],SignInTemp)<=range)
								{
									//如果当前项i 与i+timeInputted-1 项都存在的话，求间隔并比较
									if(i+timeInputted-1<allSignIn.countInList[currentIndexSel])
									{
										//当sel_LongOrShotListBox =0 求最长N次间隔
										if(sel_LongOrShotListBox==0)
										{
											maxIntervalDay =max(maxIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentIndexSel][i],
												allSignIn.SignList[currentIndexSel][i+timeInputted-1]));
										}
										else 	//当sel_LongOrShotListBox =1 求最短N次间隔
										{
											minIntervalDay =min(minIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentIndexSel][i],
												allSignIn.SignList[currentIndexSel][i+timeInputted-1]));
										}
									}
								}//end of <if //如果签到项在时间范围内> 

							}//end of <for //遍历当前所有所选项的的签到项>

							if(sel_LongOrShotListBox==0)//当sel_LongOrShotListBox =0 保存最长N次间隔到hStaticSignRange2
							{
								wsprintf(szBuffer,TEXT("%d"),maxIntervalDay);
							}
							else//当sel_LongOrShotListBox =1 保存最短N次间隔到hStaticSignRange2
							{
								if(minIntervalDay == 2147483647)
									wsprintf(szBuffer,TEXT("-1"));
								else 
									wsprintf(szBuffer,TEXT("%d"),minIntervalDay);
							}
							SetWindowText(hStaticIntervalDays,szBuffer);

						}//end of <else //如果sel_LongOrShotListBox已经选择>

					}//end of <if//如果sel_RangeListBox2已经选择>

					if(flagForEdit==1)//表明这个count是经过if(sel_RangeListBox2!=-1)
					if(countOfSignIn_inRange==0)//sel_RangeListBox2 还未选择
					{
						//更新 hStaticSignRange2 对应的数据
						SetWindowText(hStaticSignRange2,TEXT("该时间范围内没有录入的签到项"));
					}
					else//sel_RangeListBox2 已经选择
					{
						//更新 hStaticSignRange2 对应的数据
						wsprintf(szBuffer,TEXT("从已有记录的签到项 %d-%d-%d  ~ %d-%d-%d "),
								signInTemp1.year , signInTemp1.month , signInTemp1.day,
								signInTemp2.year , signInTemp2.month , signInTemp2.day);
						SetWindowText(hStaticSignRange2,szBuffer);
					}
					break;
			}
		}
		/*当窗口为“柱状图”窗口时*/
		else if(hwnd==hDlgHistogram) 
		{
			switch(LOWORD (wParam))
			{
				case 4://	hDefaultYearButton 

					if(HIWORD(wParam)==BN_CLICKED){
						//将年份设置为默认并重画

						yearToShow = defaultYearToShow;
						//设置按钮的启用状态
						EnableWindow(hNextYearButton,FALSE);
						if(cYearInRecord==1)
							EnableWindow(hLastYearButton,FALSE);

					}
					//穿过，到 case 7 刷新

				case 6://	 hLastYearButton
					
					if(LOWORD (wParam)==6)
						if(HIWORD(wParam)==BN_CLICKED){
							//将年份上一年并重画
							yearToShow = yearToShow-1;

							//设置按钮的启用状态
							EnableWindow(hNextYearButton,TRUE);
							if(yearToShow == defaultYearToShow-cYearInRecord+1)
								EnableWindow(hLastYearButton,FALSE);
						}

					//穿过，到 case 7 刷新

				case 7://  hNextYearButton

					if(LOWORD (wParam)==7)
						if(HIWORD(wParam)==BN_CLICKED){
							//将年份上一年并重画
							yearToShow = yearToShow+1;

							//设置按钮的启用状态
							EnableWindow(hLastYearButton,TRUE);
							if(yearToShow == defaultYearToShow)
								EnableWindow(hNextYearButton,FALSE);
						}

						
					//更新显示年份的static标签
					wsprintf(szBuffer,TEXT("%d"),yearToShow);
					SetWindowText(hStaticYearLabel,szBuffer);
					InvalidateRect(hwnd,NULL,TRUE);
					break;

			}

		}
		
		/*当窗口是 新添签到 对话框窗口时*/
		else if(hwnd==hDlgAddItem)
		{
			switch(LOWORD (wParam))
			{
				case 2://	hEditAddNewItem
					if(HIWORD (wParam)==EN_UPDATE)//EN_UPDATE 会在输入改变后向父窗口发送WM_CTLCOLOREDIT
					{
						GetWindowText(hEditAddNewItem,szBuffer,256);
						for(i=0;i<allSignIn.countOfItem;i++)
						{
							if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)//重名，红色提示
							{	
								EnableWindow(hConfirmButton_AddSignIn,FALSE);
								//改变字体颜色为暗红色 
								colorOfEditText = RGB(150,0,0);
								break;
							}
						}
						
						if(i==allSignIn.countOfItem)//无相同的匹配
						{
							//不符合命名规则的情况
							if(szBuffer[0]=='\0')
							{
								EnableWindow(hConfirmButton_AddSignIn,FALSE);
								colorOfEditText = RGB(150,0,0);
								break;
							}

							//符合规则，改变字体颜色为暗绿色
							EnableWindow(hConfirmButton_AddSignIn,TRUE);
							colorOfEditText = RGB(0,150,0);
						}
					}
					break;
				case 3://	hConfirmButton_AddSignIn
					GetWindowText(hEditAddNewItem,szBuffer,256);

					//在allSignIn 结构中新分配一个内存项，并更新相关数据
					allSignIn.countOfItem += 1;
					allSignIn.countInList = (int*)realloc(allSignIn.countInList,sizeof(int)*allSignIn.countOfItem);
					allSignIn.countInList[allSignIn.countOfItem-1]=0;
					allSignIn.SignList = (pSignIn*)realloc(allSignIn.SignList,sizeof(pSignIn)*allSignIn.countOfItem);
					allSignIn.SignList[allSignIn.countOfItem-1] = (pSignIn)malloc(sizeof(SignIn)*1);
					lstrcpy(allSignIn.SignList[allSignIn.countOfItem-1][0].name,szBuffer);
					allSignIn.SignList[allSignIn.countOfItem-1][0].year =0;
					allSignIn.SignList[allSignIn.countOfItem-1][0].month =0;
					allSignIn.SignList[allSignIn.countOfItem-1][0].day =0;

					//而不去更改countOfSignIn_All 和 pSignInListTemp ，只有签过到之后以备结束时写入文件

					//更新显示签到项的ListBox
					SendMessage (hSignInListBox, LB_ADDSTRING, 0, (LPARAM) &allSignIn.SignList[allSignIn.countOfItem-1][0].name) ;
					
					//给父窗口发送关闭消息
					SendMessage(hDlgAddItem,WM_CLOSE,0,0);
/*/////////////////////////////////////////////////////////////////////////////
				printf("countOfSignIn:%d\n",countOfSignIn_All);
				for(i=0;i<allSignIn.countOfItem;i++)
				{
					for(j=0;j<allSignIn.countInList[i];j++)
					{
						printf("N:%s %d %d %d\n",allSignIn.SignList[i][j].name,allSignIn.SignList[i][j].year,allSignIn.SignList[i][j].month,
							allSignIn.SignList[i][j].day);
					}
					printf("--\n");
				}
				*/
					break;
				case 4://	hCancelButton_AddSignIn
					//给父窗口发送关闭消息
					SendMessage(hDlgAddItem,WM_CLOSE,0,0);

					break;
			}
		}
		return 0;
	case  WM_CTLCOLOREDIT:
		if((HWND)lParam==hEditAddNewItem){
			//绘制colorOfEditText颜色的字体
			SetTextColor((HDC)wParam,colorOfEditText);
        
           return (LRESULT)GetStockObject(WHITE_BRUSH);
		}
		break;
	case WM_CLOSE:
		if(hwnd==hDlgAddItem)
			break; //如果是 新添签到 对话框，交给DefWindowProc (hwnd, message, wParam, lParam) ;默认处理（销毁！）

		ShowWindow(hwnd,SW_HIDE);
		
		return 0;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}


/*****************************************************************
*          第一个模块中所有出现的Edit控件的新的消息处理函数
******************************************************************/

LRESULT CALLBACK newEditProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WNDPROC oldEditProcChosen;
//	HDC hdc;

	switch(message)
	{
	case WM_KEYDOWN:
		if(wParam==VK_RETURN){//捕捉Edit控件的回车事件

			//使对应的Edit窗口失去焦点，从而在父窗口得到其EN_KILLFOCUS事件
			if(GetFocus()==hTimesEdit){
				SetFocus(GetParent(hwnd));
			}
			if(GetFocus()==hEditAddNewItem){
				SetFocus(GetParent(hwnd));
			}
		}
		break;

	}

	//根据调用消息的Edit窗口选择其相对应的回调函数
	if(hwnd==hTimesEdit)	
		oldEditProcChosen = oldEditProc[0];
	else if(hwnd==hEditAddNewItem)
		oldEditProcChosen = oldEditProc[1];

	return  CallWindowProc (oldEditProcChosen, hwnd, message, wParam,lParam) ;
}

