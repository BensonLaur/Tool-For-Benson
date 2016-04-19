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

/*********   模块的名称，需要时可以直接在下面添加名称从而 直接添加模块  ********/
static TCHAR *ModuleName[] = {
TEXT("签到"),
TEXT("收藏"),
TEXT("收藏"),
TEXT("收藏收藏收藏收藏收藏收藏"),
TEXT("收藏"),
TEXT("收藏")
};
static TCHAR *CollectionModuleName[] = {
TEXT("网络"),
TEXT("本地"),
TEXT("本地")
};

//主模块及其管理器句柄 和 第二个模块“收藏”及管理器句柄 
static MODULEMANAGER ModuleManeger,ModuleManeger2;	 
static HMODULEMANAGER hMM,hMM2;
//记录当前模块的ID
static int idOfCurrentAppModule;
static int idOfCurrentSubModule;


static ModuleProc moduleProc[COUNT_OF_MODULE_MANAGER];

/*********   在此定义各个模块的“内容窗口”的所有子窗口的句柄以备使用  ********/

//第一个模块 “签到” 的所有句柄
 
static HWND hSignInListBox , hAddItemButton , hDeleteItemButton ,  
	   hStaticCurrent , hStaticCurrentItem ,
	   hSignInButton , hStatisticsGroupBox , hBaseDataButton , hHistogramButton ;
static HWND hDlgBaseData , hDlgHistogram;
static HWND hStaticSignRange1,hRangeListBox1 ,hStaticWord1, hStaticTimes , hStaticDaysOnce ,
       hRangeListBox2 , hLongOrShotListBox ,hTimesEdit, hStaticWord2,hStaticIntervalDays;
static HWND hStaticSignRange2,hStaticTime, hStaticMonth , *hStaticTimesNumber, *hStaticMonthNumber,
	   hDefaultYearButton,hStaticYearLabel, hNextYearButton, hLastYearButton;

//第二个模块 “收藏” 的所有句柄  

static HWND hStaticCurrentFloder,hFloderListBox, hLinkListBox,
	   hAddFloderButton, hDeleteFloderButton,
	   hAddLinkButton,  hDeleteLinkButton;
static HWND hStaticCurrentFloder2,hFloderListBox2, hLinkListBox2,
	   hAddFloderButton2, hDeleteFloderButton2,
	   hAddLinkButton2,  hDeleteLinkButton2;
static HWND hDlgAddNetLink, hDlgAddLocalLink,
       hStaticName, hStaticLink, hNameEdit1, hLinkEdit1, hConformButton1 , hCancelbutton1,
       hStaticName2, hStaticLink2, hNameEdit2, hLinkEdit2, hConformButton2 , hCancelbutton2;

//第三个模块 



//签到模块数据
static AllSignIn allSignIn;
static FILE *pSignInFile;
static pSignIn pSignInListTemp;  //程序启动后文件读取后关闭，签到数据的更改缓冲于此，软件关闭时才更新写入
static int countOfSignIn;
static SignIn SignInTemp;
static TCHAR filePath[256];
static int dlgAnswer;

static int currentSignInSel;
static struct tm *pTime;
static time_t secondTime;

/*********************************************************************
*		以下为主函数、各个窗口的消息处理函数、和辅助函数
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
*	              主窗口用的消息处理函数
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
			//初始化自定义的句柄映射队列
			InitializeMapList();

			//创建一个模块管理器 ModuleManager 以及其句柄 hMM
			
			moduleProc[0].ManagerProc = ModuleManagerProc;
			moduleProc[0].ButtonWindowProc = ModuleButtonWindowProc;
			moduleProc[0].ContentWindowProc = ModuleContentWindowProc;

			// ModuleManager 里面有 其相应的 hStaticBoard; hButtonBoard; hContentBoard;
			CreateStaticModuleManergerBoard(hMM,ModuleManeger,moduleProc[0],
				         hwnd,(HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE));

			for(i=0; i <COUNT_OF_MODULE ; i++ )
			   AddNewModule(hMM,moduleProc[0],ModuleName[i]);

			//为第二个模块创建一个模块管理器 ModuleManager2 以及其句柄 hMM2
			
			moduleProc[1].ManagerProc = ModuleManagerProc2;
			moduleProc[1].ButtonWindowProc = ModuleButtonWindowProc2;
			moduleProc[1].ContentWindowProc = ModuleContentWindowProc2;
			
			CreateStaticModuleManergerBoard(hMM2,ModuleManeger2,moduleProc[1],
				         ModuleManeger.pToolModule[1].hContentBoard,(HMENU)1, //新的模块管理器的父窗口为第二个模块的子窗口
						 (HINSTANCE)GetWindowLong(ModuleManeger.pToolModule[1].hContentBoard,GWL_HINSTANCE));
			
			for(i=0; i <COUNT_OF_SUB_MODULE ; i++ )
			   AddNewModule(hMM2,moduleProc[1],CollectionModuleName[i]);

			/**注意:第二个模块管理器的布局 交由【主模块管理器】静态ContentBoard 的消息处理函数 来绘制  */
			/**    而第二个模块管理器里的各个模块的布局 则由【第二个模块管理器】ModuleContentWindowProc2负责*/
			
			//获得系统默认字体的高度
			cyChar = HIWORD (GetDialogBaseUnits ()) ;
			cyChar*=2;

			idOfCurrentAppModule=0;
			idOfCurrentSubModule=0;

          return 0 ;

	 case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			
			//设置ModuleManeger各个子窗口的位置，hButtonBoard 和hContentBoard 是hStaticBoard的子窗口
			MoveWindow(ModuleManeger.hStaticBoard,5,5,cxClient-10,cyClient-10,TRUE);
			MoveWindow(ModuleManeger.hButtonBoard,3,3,cxClient-16,cyChar+5,TRUE);

			for(i=0 ; i<ModuleManeger.countOfModule ; i++)
				MoveWindow(ModuleManeger.pToolModule[i].hContentBoard,3,3+cyChar+6,cxClient-16,cyClient-10-3-cyChar-6-3,TRUE);
			
			//显示当前所选定的“内容窗口”
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
		  //释放句柄的内存
		  FreeMapList();

		  //在关闭文件之前，先将数据数据重新写入文件
		  if((pSignInFile = fopen(filePath,"w+b"))==NULL)
		  {
			  wsprintf(szBuffer,TEXT("签到数据无法更新！\n\n原因：\n无法打开文件:%s \n\n请检查其目录是否缺失！"),filePath);
			  MessageBox(hwnd,szBuffer,TEXT("提示"),MB_ICONERROR);
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
*	两个子窗口 hDlgBaseData 和 hDlgHistogram 的消息处理函数
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

			//记录当前有签到记录的年数
			cYearInRecord= defaultYearToShow - allSignIn.SignList[currentSignInSel][0].year+1;

			//更新显示年份的static标签
			wsprintf(szBuffer,TEXT("%d"),yearToShow);
			SetWindowText(hStaticYearLabel,szBuffer);

			//根据情况决定按钮的启用状态
			EnableWindow(hNextYearButton,FALSE);
			if(cYearInRecord==1)
				EnableWindow(hLastYearButton,FALSE);
			
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
					countOfSignIn=0;
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
					//遍历当前所有所选项的的签到项
						for(i=0;i<allSignIn.countInList[currentSignInSel] ;i++)
						{
							//如果签到项在时间范围内，进行记录
							if(DaysBetween(allSignIn.SignList[currentSignInSel][i],SignInTemp)<=range)
							{
								countOfSignIn+=1;
								if(countOfSignIn==1)//记录该范围内第一个签到项
									signInTemp1=allSignIn.SignList[currentSignInSel][i];
							}
						}
						if(countOfSignIn!=0)//如果有记录签到项记录最后一个
							signInTemp2 = allSignIn.SignList[currentSignInSel][allSignIn.countInList[currentSignInSel]-1];

					//直接更新显示数据
					if(countOfSignIn==0){

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
					countOfSignIn=0;
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
						//遍历当前所有所选项的的签到项
						for(i=0;i<allSignIn.countInList[currentSignInSel] ;i++)
						{
							//如果签到项在时间范围内，进行记录
							if(DaysBetween(allSignIn.SignList[currentSignInSel][i],SignInTemp)<=range)
							{
								countOfSignIn+=1;
								if(countOfSignIn==1)//记录该范围内第一个签到项
									signInTemp1=allSignIn.SignList[currentSignInSel][i];
							}
						}
						if(countOfSignIn!=0)//如果有记录签到项记录最后一个
							signInTemp2 = allSignIn.SignList[currentSignInSel][allSignIn.countInList[currentSignInSel]-1];

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
							for(i=0;i<allSignIn.countInList[currentSignInSel] ;i++)
							{
								//如果签到项在时间范围内
								if(DaysBetween(allSignIn.SignList[currentSignInSel][i],SignInTemp)<=range)
								{
									//如果当前项i 与i+timeInputted-1 项都存在的话，求间隔并比较
									if(i+timeInputted-1<allSignIn.countInList[currentSignInSel])
									{
										//当sel_LongOrShotListBox =0 求最长N次间隔
										if(sel_LongOrShotListBox==0)
										{
											maxIntervalDay =max(maxIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentSignInSel][i],
												allSignIn.SignList[currentSignInSel][i+timeInputted-1]));
										}
										else 	//当sel_LongOrShotListBox =1 求最短N次间隔
										{
											minIntervalDay =min(minIntervalDay,DaysBetween(
										    	allSignIn.SignList[currentSignInSel][i],
												allSignIn.SignList[currentSignInSel][i+timeInputted-1]));
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
					if(countOfSignIn==0)//sel_RangeListBox2 还未选择
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
		return 0;

	case WM_CLOSE:
		ShowWindow(hwnd,SW_HIDE);	

		return 0;

	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

/*****************************************************************
*	模块管理器 的 静态“按钮窗口”使用的消息处理函数 (主模块管理器)
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
	static int blockLeft,blockRight,newLeft,newRight,oldLeft,oldRight; //用于绘制高亮区域
	static int firstTimeFlag = 1;
	static bool toRight = true;
	static int currentId = 0;

	//"按钮窗口"的两个左右方向按钮
	static HWND hLeftButton,hRightButton;
	//记录“按钮窗口”中按钮的开始与结束位置
	static int xButtonBegin,xButtonEnd;

	switch(message)
	{
	case WM_CREATE:
		//设置定时器，用于绘画可以移动的高亮区域
		SetTimer (hwnd, 1, 10, NULL) ;
		blockLeft = blockRight = 0;
		xButtonBegin = 5;

		hLeftButton = CreateWindow(TEXT("button"),TEXT("<<"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // 使用 MAX_COUNT_OF_MODULE+1 作为向左移动按钮的ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+1), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		hRightButton = CreateWindow(TEXT("button"),TEXT(">>"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // 使用 MAX_COUNT_OF_MODULE+2 作为向右移动按钮的ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+2), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

		break;

	case WM_SIZE:
		//获得“按钮窗口”的大小消息，和设备字体宽度
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cxChar = LOWORD (GetDialogBaseUnits ()) ;

		//仅调用一次
		if(firstTimeFlag)
		{
			//将每个按钮的字体宽度信息保存在列表中
			intList = (int*)malloc(sizeof(int)*ModuleManeger.countOfModule);
			CheckNullErrorAndQuit(intList,13,TEXT("Memory for [intList] alloc failed in ModuleButtonWindowProc()!"));

			for(i=0 ; i< ModuleManeger.countOfModule ; i++)
			{
				intList[i] = GetWindowTextLength (ModuleManeger.pToolModule[i].hButton) ;
			}
	
			//第一次调用时将高亮的区域指定为第一个按钮
			newLeft = xButtonBegin;
			newRight = xButtonBegin + intList[0]*cxChar*2+30;
			
			firstTimeFlag = 0;
		}

		//根据xButtonBegin和字体宽度等设置按钮的 位置和大小
		lengthToLeft =0;
		for(i=0 ; i< ModuleManeger.countOfModule ; i++)
		{
			MoveWindow(ModuleManeger.pToolModule[i].hButton,
				xButtonBegin+lengthToLeft*cxChar*2+i*30,10, intList[i]*cxChar*2+30,cyWindow-10-2,TRUE);
			lengthToLeft += intList[i];
		}

		//根据xButtonBegin 和 按钮总宽度获得 xButtonEnd
		totalWidth = lengthToLeft*cxChar*2 + i*30;
		xButtonEnd = xButtonBegin + totalWidth;

		//由 xButtonBegin 、xButtonEnd 与 windows size 的关系决定“左右移动”按钮是否出现
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

		//改变大小变宽时，若左侧部分隐藏，右侧又有空余，则全部按钮拉着右边框跟着移动
		if(xButtonBegin < 5 && xButtonEnd < cxWindow-5)
		{
			xButtonBegin += cxWindow- 5 - xButtonEnd;
			newLeft += cxWindow- 5 - xButtonEnd;
			newRight += cxWindow - 5-xButtonEnd;
			toRight = true;
		}


		break;

	case WM_COMMAND:
		//由按钮的句柄获得其Id 即得到所在模块pToolModle[]的下标
		idGot = GetWindowLong ((HWND)lParam, GWL_ID) ;
		idOfCurrentAppModule = idGot < MAX_COUNT_OF_MODULE ? idGot : idOfCurrentAppModule;

		if(idGot < MAX_COUNT_OF_MODULE) //为模块按钮的消息
		{
			for(i=0; i<pMM->countOfModule ;i++){
				if(i==idOfCurrentAppModule)
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_SHOW);
				else 
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_HIDE);
			}

			//计算点击按钮的位置，储存在newLeft 和 newRight中
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
		
			//确定高亮移动方向
			if(idOfCurrentAppModule > currentId) toRight =true;
			else 
				if(idOfCurrentAppModule < currentId) toRight = false;
			currentId = idOfCurrentAppModule;
		}
		else //为“左右移动”按钮的消息
		{
			if(idGot == MAX_COUNT_OF_MODULE + 1) //为“向左”按钮
			{
				if(xButtonEnd > cxWindow-5){
					buttonShift=xButtonEnd;
					//改变Button位置
					xButtonEnd = max( cxWindow-5, xButtonEnd-30);
					xButtonBegin = xButtonEnd - totalWidth;
					
					//改变高亮位置
					buttonShift = xButtonEnd -buttonShift;
					newLeft += buttonShift;
					newRight += buttonShift;
					toRight = false;
				}
			}
			else  //为“向右”按钮
			{
				if(xButtonBegin < 5){
					buttonShift=xButtonBegin;
					//改变Button位置
					xButtonBegin = min( 5, xButtonBegin + 30);
					xButtonEnd = xButtonBegin + totalWidth;

					//改变高亮位置
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

		//更新绘画高亮区域的范围
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
		//绘制高亮区域
		//擦除旧的区域
		SetRect(&rect,oldLeft,1,oldRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		//绘制性的区域
		SetRect(&rect,blockLeft,1,blockRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));

         EndPaint (hwnd, &ps) ;
		break;

	}
	return CallWindowProc (pMM->OldModuleButton, hwnd, message, wParam,lParam);
}

/*****************************************************************
*	每个新模块的 静态“内容窗口”使用的消息处理函数 (主模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleContentWindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//得到原来的消息函数
	WNDPROC oldModuleContent = GetOldContentWndProcByContentHWND(hwnd);
	CheckNullErrorAndQuit(oldModuleContent,8,TEXT("Can't Get oldModuleContent By HWND in ModuleButtonWindowProc()!"));
	
	//得到模块的ID
	int id = GetIdInModuleManagerByContentHWND(hwnd);
	if(id == -1){
		MessageBox(hwnd,TEXT("ErrorCode: 0X 0011\nCant't Get [idModule] in ModuleContentWindowProc()"),TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
	//得到模块管理器指针
	PMODULEMANAGER pMM = GetPModuleManagerByContentHWND(hwnd);
	CheckNullErrorAndQuit(pMM,12,TEXT("Can't Get ModuleManager By ContentHWND in ModuleContentWindowProc()!"));

	static int cxWindow,cyWindow,cxChar,cyChar,i,j;


	//可以在此消息处理函数中初始化各个模块的内容
	switch(message)
	{
	case WM_CREATE:
		/** 注意：在子窗口类别化时，次新的回调函数的WM_CREATE 不会被调用，进入该函数的该消息是在
			AddNewModule()中使用 SendMessage(hContentBoard,WM_CREATE,0,0); 发送的	*/
		//初始化第一个模块内容
		if( id == 0 )
		{
				// 签到ListBox，增加、删除签到项按钮
				 hSignInListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									5,5,80,80,
									hwnd, (HMENU)1,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hAddItemButton = CreateWindow(TEXT("button"),TEXT("添加签到项"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									5,90,40,20,
									hwnd, (HMENU)2, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hDeleteItemButton = CreateWindow(TEXT("button"),TEXT("删除签到项"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									100,90,40,20,
									hwnd, (HMENU)3, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				 //两个静态标签，显示当前签到项
				 hStaticCurrent = CreateWindow(TEXT("static"),TEXT("当前签到项："),WS_CHILD|WS_VISIBLE|SS_LEFT,
									200,20,100,30,
									hwnd, (HMENU)4, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				 hStaticCurrentItem =CreateWindow(TEXT("static"),TEXT("***当前签到项***"),WS_CHILD|WS_VISIBLE|SS_LEFT,
									200,60,200,30,
									hwnd, (HMENU)5, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
					
				 //签到按钮
				hSignInButton = CreateWindow(TEXT("button"),TEXT("今日签到"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									200,100,40,20,
									hwnd, (HMENU)6, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				 //签到统计模块
				hStatisticsGroupBox =CreateWindow(TEXT("button"),TEXT("签到统计"),WS_CHILD|WS_VISIBLE|BS_GROUPBOX,
									250,100,80,100,
									hwnd, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
				
				hBaseDataButton =CreateWindow(TEXT("button"),TEXT("基本数据展示"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									255,130,70,30,
									hwnd, (HMENU)9, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

				hHistogramButton =CreateWindow(TEXT("button"),TEXT("柱状图"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									255,155,70,30,
									hwnd, (HMENU)10, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

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
				oldEditProc = (WNDPROC)SetWindowLong(hTimesEdit,GWL_WNDPROC,(LONG)newEditProc);

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
	
				/** 业务数据逻辑处理部分  ***/

				GetCurrentDirectory(256,filePath);
				wsprintf(filePath,TEXT("%s\\Data"),filePath);
				printf(TEXT("mkdir:%d"),mkdir(filePath));
				wsprintf(filePath,TEXT("%s\\SignIn.dat"),filePath);


				//初始化具体的ListBox项
				
				//打开签到记录文件
				if((pSignInFile = fopen(filePath,"r+b"))==NULL)
				{
					if((pSignInFile = fopen(filePath,"w+b"))==NULL){
						wsprintf(szBuffer,TEXT("Can't Create File [%s]!"),filePath);
						CheckNullErrorAndQuit(NULL,20,szBuffer);
						exit(0);
					}
					//新建文件之后，初始化数据
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
				//从头开始读取文件数据
				fseek(pSignInFile,0,SEEK_SET);
				fread(&countOfSignIn,sizeof(int),1,pSignInFile);

				//将文件里的数据读出，按存放于结构体AllSignIn中
				pSignInListTemp = (pSignIn)malloc(sizeof(SignIn)*countOfSignIn);
				fread(pSignInListTemp,sizeof(SignIn),countOfSignIn,pSignInFile);
				fclose(pSignInFile);

				for(i=0;i<countOfSignIn;i++)
				{
					if(i==0){
						
						//初始第一个项目到结构体里
						allSignIn.countOfItem =1;
						allSignIn.countInList = (int*)malloc(sizeof(int)*allSignIn.countOfItem);
						allSignIn.countInList[0]=1;
						allSignIn.SignList = (pSignIn*)malloc(sizeof(pSignIn)*allSignIn.countOfItem);
						allSignIn.SignList[0] = (pSignIn)malloc(sizeof(SignIn)*allSignIn.countInList[0]);
						allSignIn.SignList[0][0]= pSignInListTemp[i];
					}
					else
					{

						//在二维数组里查找签到项是否出现，若出现，则加入到对应的签到队列里
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
						if(j==allSignIn.countOfItem) //若无出现，则新分配一个内存项，并更新相关数据
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


				//根据数据更新ListBox的数据
				for(i=0;i<allSignIn.countOfItem;i++)
					SendMessage (hSignInListBox, LB_ADDSTRING, 0, (LPARAM) &allSignIn.SignList[i][0].name) ;
				//当前签到项的静态标签设为“未选择签到项”
				SetWindowText(hStaticCurrentItem,TEXT("——未选！请先选择或添加 ——"));

				currentSignInSel=-1;

		//printf("%s",filePath);
/*
typedef struct _AllSignIn{
int countOfItem;
SignIn ** SignInList;  //二维的签到记录，第一维的个数为countOfItem ，第二维的个数分别存放于countList
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
		 else if( id == 1 )//初始化第二个模块内容
		{
			/**注意:第二个模块管理器的布局 交由此【主模块管理器】静态ContentBoard 的消息处理函数 来绘制  */
			/**    而第二个模块管理器里的各个模块的布局 则由【第二个模块管理器】ModuleContentWindowProc2负责*/
		}

		break;

	case WM_SIZE:
		
		//获得“按钮窗口”的大小消息，和设备字体宽度
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cxChar = LOWORD (GetDialogBaseUnits ()) ;
		cyChar = HIWORD (GetDialogBaseUnits ()) ;

		if( id == 0 )
		{
			//调整控件位置
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
			//设置ModuleManeger2各个子窗口的位置，hButtonBoard 和hContentBoard 是hStaticBoard的子窗口
			MoveWindow(ModuleManeger2.hStaticBoard,5,5,cxWindow-10,cyWindow-10,TRUE);
			MoveWindow(ModuleManeger2.hButtonBoard,3,3,cxWindow-16,cyChar*2,TRUE);

			for(i=0 ; i<ModuleManeger2.countOfModule ; i++)
				MoveWindow(ModuleManeger2.pToolModule[i].hContentBoard,3,3+cyChar*2+3,cxWindow-16,cyWindow-3*2-3-cyChar*2,TRUE);
			
			//显示当前所选定的“内容窗口”
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
					MessageBox(hwnd,TEXT("未选择签到项！\n\n请先选择或创建签到项！ :)"),TEXT("签到提示"),MB_ICONINFORMATION);
					break;
				}
				
				//获得当前系统的时间信息
				secondTime = time(NULL);
				pTime = localtime(&secondTime);
				//生成签到内容
				SendMessage(hSignInListBox,LB_GETTEXT,currentSignInSel,(LPARAM)szBuffer);
				wsprintf(SignInTemp.name,"%s",szBuffer);
				SignInTemp.year = 1900 + pTime->tm_year;
				SignInTemp.month = pTime->tm_mon+1;
				SignInTemp.day = pTime->tm_mday;

				printf("------> szBuffer:SignInItemName:%s\n",szBuffer);
				
				//进行签到处理
				for(i=0;i<allSignIn.countOfItem;i++)
				{
					//循环查找所有的签到项，看是否出现过
					if(lstrcmp(szBuffer,allSignIn.SignList[i][0].name)==0)
					{
						//如果出现过，进一步循环看具体项目是否已经签到过了
						for(j=0;j<allSignIn.countInList[i];j++)
						{
							//如果已签到，提示
							if(allSignIn.SignList[i][j].year == SignInTemp.year &&
							   allSignIn.SignList[i][j].month == SignInTemp.month &&
							   allSignIn.SignList[i][j].day == SignInTemp.day )
							{
								wsprintf(szBuffer2,TEXT("签到项：%s  %d-%d-%d 已经签过到了！"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
								MessageBox(hwnd,szBuffer2,TEXT("签到提示"),MB_ICONINFORMATION);
								break;
							}
						}
						//如果签到项未出现过
						if(j==allSignIn.countInList[i])
						{
							wsprintf(szBuffer2,TEXT("是否确认对签到项：%s  于%d-%d-%d 的签到？"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
							dlgAnswer=MessageBox(hwnd,szBuffer2,TEXT("签到提示"),MB_ICONINFORMATION|MB_YESNO);
							
							//当用户确认时，分配新的足够的内存，将签到信息写入缓冲结构allSignIn
							if(dlgAnswer==IDYES){
								allSignIn.countInList[i] += 1;
								allSignIn.SignList[i] = (pSignIn)realloc(allSignIn.SignList[i],sizeof(SignIn)*allSignIn.countInList[i]);
								allSignIn.SignList[i][allSignIn.countInList[i]-1] = SignInTemp;

								//同时更改countOfSignIn 和 pSignInListTemp 以备结束时写入文件
								countOfSignIn += 1;
								pSignInListTemp = (pSignIn)realloc(pSignInListTemp,sizeof(SignIn)*countOfSignIn);
								pSignInListTemp[countOfSignIn-1] = SignInTemp;
								
								//签到成功提示
								wsprintf(szBuffer2,TEXT("签到项：%s  %d-%d-%d  签到成功！"),szBuffer,SignInTemp.year,
									SignInTemp.month,SignInTemp.day);
								MessageBox(hwnd,szBuffer2,TEXT("签到提示"),MB_ICONINFORMATION);
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
					MessageBox(hwnd,TEXT("未选择签到项！\n\n请先选择或创建签到项！ :)"),TEXT("统计提示"),MB_ICONINFORMATION);
					break;
				}

				//打开前发送自定义消息，初始化窗口
				SendMessage(hDlgBaseData,WM_USER+2,0,0);

				ShowWindow(hDlgBaseData ,SW_SHOW);

				break;
			case 10: //hHistogramButton:
				
				if(currentSignInSel==-1)
				{
					MessageBox(hwnd,TEXT("未选择签到项！\n\n请先选择或创建签到项！ :)"),TEXT("统计提示"),MB_ICONINFORMATION);
					break;
				}
				
				//打开前发送自定义消息，初始化窗口
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
*  第二个模块下的子模块管理器的消息处理函数
******************************************************************/

/*****************************************************************
*  模块管理器 的 底层静态“静态窗口” 使用的消息处理函数  (第二个模块管理器)
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
*  模块管理器 的 静态“按钮窗口”使用的消息处理函数 (第二个模块管理器)
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
	static int blockLeft,blockRight,newLeft,newRight,oldLeft,oldRight; //用于绘制高亮区域
	static int firstTimeFlag = 1;
	static bool toRight = true;
	static int currentId = 0;

	//"按钮窗口"的两个左右方向按钮
	static HWND hLeftButton,hRightButton;
	//记录“按钮窗口”中按钮的开始与结束位置
	static int xButtonBegin,xButtonEnd;
	
	switch(message)
	{
	case WM_CREATE:
			/** 注意：在子窗口类别化时，次新的回调函数的WM_CREATE 不会被调用，进入该函数的该消息是在
			CreateStaticModuleManergerBoard()中使用 SendMessage(pMM.hButtonBoard,WM_CREATE,0,0); 发送的	*/

		//设置定时器，用于绘画可以移动的高亮区域
		SetTimer (hwnd, 2, 10, NULL) ;
		blockLeft = blockRight = 0;
		xButtonBegin = 5;

		hLeftButton = CreateWindow(TEXT("button"),TEXT("<<"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // 使用 MAX_COUNT_OF_MODULE+1 作为向左移动按钮的ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+1), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		hRightButton = CreateWindow(TEXT("button"),TEXT(">>"),WS_CHILD|BS_PUSHBUTTON,
									0,0,0,0,  // 使用 MAX_COUNT_OF_MODULE+2 作为向右移动按钮的ID
									hwnd, (HMENU)(MAX_COUNT_OF_MODULE+2), (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

		break;

	case WM_SIZE:
		//获得“按钮窗口”的大小消息，和设备字体宽度
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cxChar = LOWORD (GetDialogBaseUnits ()) ;

		//仅调用一次
		if(firstTimeFlag)
		{
			//将每个按钮的字体宽度信息保存在列表中
			intList = (int*)malloc(sizeof(int)*ModuleManeger2.countOfModule);
			CheckNullErrorAndQuit(intList,19,TEXT("Memory for [intList] alloc failed in ModuleButtonWindowProc2()!"));

			for(i=0 ; i< ModuleManeger2.countOfModule ; i++)
			{
				intList[i] = GetWindowTextLength (ModuleManeger2.pToolModule[i].hButton) ;
			}
	
			//第一次调用时将高亮的区域指定为第一个按钮
			newLeft = xButtonBegin;
			newRight = xButtonBegin + intList[0]*cxChar*2+30;
			
			firstTimeFlag = 0;
		}

		//根据xButtonBegin和字体宽度等设置按钮的 位置和大小
		lengthToLeft =0;
		for(i=0 ; i< ModuleManeger2.countOfModule ; i++)
		{
			MoveWindow(ModuleManeger2.pToolModule[i].hButton,
				xButtonBegin+lengthToLeft*cxChar*2+i*30,10, intList[i]*cxChar*2+30,cyWindow-10-2,TRUE);
			lengthToLeft += intList[i];
		}

		//根据xButtonBegin 和 按钮总宽度获得 xButtonEnd
		totalWidth = lengthToLeft*cxChar*2 + i*30;
		xButtonEnd = xButtonBegin + totalWidth;

		//由 xButtonBegin 、xButtonEnd 与 windows size 的关系决定“左右移动”按钮是否出现
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

		//改变大小变宽时，若左侧部分隐藏，右侧又有空余，则全部按钮拉着右边框跟着移动
		if(xButtonBegin < 5 && xButtonEnd < cxWindow-5)
		{
			xButtonBegin += cxWindow- 5 - xButtonEnd;
			newLeft += cxWindow- 5 - xButtonEnd;
			newRight += cxWindow - 5-xButtonEnd;
			toRight = true;
		}


		break;

	case WM_COMMAND:
		//由按钮的句柄获得其Id 即得到所在模块pToolModle[]的下标
		idGot = GetWindowLong ((HWND)lParam, GWL_ID) ;
		idOfCurrentSubModule = idGot < MAX_COUNT_OF_MODULE ? idGot : idOfCurrentSubModule;

		if(idGot < MAX_COUNT_OF_MODULE) //为模块按钮的消息
		{
			for(i=0; i<pMM->countOfModule ;i++){
				if(i==idOfCurrentSubModule)
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_SHOW);
				else 
					ShowWindow(pMM->pToolModule[i].hContentBoard,SW_HIDE);
			}

			//计算点击按钮的位置，储存在newLeft 和 newRight中
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
		
			//确定高亮移动方向
			if(idOfCurrentSubModule > currentId) toRight =true;
			else 
				if(idOfCurrentSubModule < currentId) toRight = false;
			currentId = idOfCurrentSubModule;
		}
		else //为“左右移动”按钮的消息
		{
			if(idGot == MAX_COUNT_OF_MODULE + 1) //为“向左”按钮
			{
				if(xButtonEnd > cxWindow-5){
					buttonShift=xButtonEnd;
					//改变Button位置
					xButtonEnd = max( cxWindow-5, xButtonEnd-30);
					xButtonBegin = xButtonEnd - totalWidth;
					
					//改变高亮位置
					buttonShift = xButtonEnd -buttonShift;
					newLeft += buttonShift;
					newRight += buttonShift;
					toRight = false;
				}
			}
			else  //为“向右”按钮
			{
				if(xButtonBegin < 5){
					buttonShift=xButtonBegin;
					//改变Button位置
					xButtonBegin = min( 5, xButtonBegin + 30);
					xButtonEnd = xButtonBegin + totalWidth;

					//改变高亮位置
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

		//更新绘画高亮区域的范围
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
		//绘制高亮区域
		//擦除旧的区域
		SetRect(&rect,oldLeft,1,oldRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		//绘制性的区域
		SetRect(&rect,blockLeft,1,blockRight,8);
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));

         EndPaint (hwnd, &ps) ;
		break;

	}


	return CallWindowProc (pMM->OldModuleButton, hwnd, message, wParam,lParam);
}

/*****************************************************************
*  每个新模块的 静态“内容窗口”使用的消息处理函数  (第二个模块管理器)
******************************************************************/

LRESULT CALLBACK ModuleContentWindowProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	//得到原来的消息函数
	WNDPROC oldModuleContent = GetOldContentWndProcByContentHWND(hwnd);
	CheckNullErrorAndQuit(oldModuleContent,16,TEXT("Can't Get oldModuleContent By HWND in ModuleButtonWindowProc()!"));
	
	//得到模块的ID
	int id = GetIdInModuleManagerByContentHWND(hwnd);
	if(id == -1){
		MessageBox(hwnd,TEXT("ErrorCode: 0X 0017\nCant't Get [idModule] in ModuleContentWindowProc2()"),TEXT("ERROR"),MB_ICONERROR);
		PostQuitMessage (0) ;
	}
	//得到模块管理器指针
	PMODULEMANAGER pMM = GetPModuleManagerByContentHWND(hwnd);
	CheckNullErrorAndQuit(pMM,18,TEXT("Can't Get ModuleManager By ContentHWND in ModuleContentWindowProc2()!"));


	static int cxWindow,cyWindow,cxChar,cyChar,i;

	switch(message)
	{
		
	case WM_CREATE:
		/** 注意：在子窗口类别化时，次新的回调函数的WM_CREATE 不会被调用，进入该函数的该消息是在
			AddNewModule()中使用 SendMessage(hContentBoard,WM_CREATE,0,0); 发送的	*/

		//初始化第一个子模块的控件
		if(id==0)
		{
			//显示当前文件夹的静态标签
			hStaticCurrentFloder = CreateWindow(TEXT("static"),TEXT("当前文件夹"),WS_CHILD|WS_VISIBLE|SS_CENTER,
									0,0,0,0,
									hwnd, (HMENU)1, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//两个显示当前文件夹和链接的ListBox
			hFloderListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)2,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hLinkListBox = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)3,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//四个添加或删除按钮
			hAddFloderButton = CreateWindow(TEXT("button"),TEXT("新增文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)4, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteFloderButton = CreateWindow(TEXT("button"),TEXT("删除文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)5, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
			
			hAddLinkButton = CreateWindow(TEXT("button"),TEXT("添加链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)6, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteLinkButton = CreateWindow(TEXT("button"),TEXT("删除链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)7, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		}
		else if(id==1)//初始化第二个子模块的控件
		{
						//显示当前文件夹的静态标签
			hStaticCurrentFloder2 = CreateWindow(TEXT("static"),TEXT("当前文件夹"),WS_CHILD|WS_VISIBLE|SS_CENTER,
									0,0,0,0,
									hwnd, (HMENU)8, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//两个显示当前文件夹和链接的ListBox
			hFloderListBox2 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)9,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hLinkListBox2 = CreateWindow(TEXT("listbox"),NULL,WS_CHILD|WS_VISIBLE|LBS_STANDARD,
									0,0,0,0,
									hwnd, (HMENU)10,(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			//四个添加或删除按钮
			hAddFloderButton2 = CreateWindow(TEXT("button"),TEXT("新增文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)11, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteFloderButton2 = CreateWindow(TEXT("button"),TEXT("删除文件夹"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)12, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
			
			hAddLinkButton2 = CreateWindow(TEXT("button"),TEXT("添加链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)13, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);

			hDeleteLinkButton2 = CreateWindow(TEXT("button"),TEXT("删除链接"),WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON,
									0,0,0,0,
									hwnd, (HMENU)14, (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		}
		else
		{

		}

		break;

	case WM_SIZE:
		
		//获得“内容窗口”的大小消息，和设备字体宽度
		cxWindow = LOWORD(lParam);
		cyWindow = HIWORD(lParam);
		cyChar = LOWORD (GetDialogBaseUnits ()) ;

		//调整第一个子模块的控件的位置和大小
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
		else if(id==1)//调整第二个子模块的控件的位置和大小
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
*          本程序所有出现的Edit控件的新的消息处理函数
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

// 当前错误提示，用到20
