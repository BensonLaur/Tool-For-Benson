/******************************************************************
	�ļ�����SecondModuleManagerWndProc.cpp

	�ļ��������ڶ���ģ���������������Ϣ������

*********************************************************************/
#include <windows.h>
#include "MyDefine.h"

//ʹ��ȫ�ֱ��� ModuleManeger2 (��GlobleManager.h�ж��壬��Entry.cpp�г�ʼ��)
extern MODULEMANAGER ModuleManeger2;	

//������ ���ڶ���ģ��������� ��ָ����ǰ ��ťѡ�����Ǹ���ģ��
static idOfCurrentSubModule;

//���ڶ���ģ�顿 ���ղء��е� �����о��  
static HWND hStaticCurrentFloder,hFloderListBox, hLinkListBox,
	   hAddFloderButton, hDeleteFloderButton,
	   hAddLinkButton,  hDeleteLinkButton;
static HWND hStaticCurrentFloder2,hFloderListBox2, hLinkListBox2,
	   hAddFloderButton2, hDeleteFloderButton2,
	   hAddLinkButton2,  hDeleteLinkButton2;
static HWND hDlgAddNetLink, hDlgAddLocalLink,
       hStaticName, hStaticLink, hNameEdit1, hLinkEdit1, hConformButton1 , hCancelbutton1,
       hStaticName2, hStaticLink2, hNameEdit2, hLinkEdit2, hConformButton2 , hCancelbutton2;

/*****************************************************************
*  �ڶ���ģ���µ���ģ�����������Ϣ��������������
*
*	ModuleManagerProc2			������ ���ײ㡱��̬ static Board �ؼ�����Ϣ������
*   ModuleButtonWindowProc2		������ ����ť����̬ static button board �ؼ�����Ϣ������
*   ModuleContentWindowProc2	������ �����ݡ���̬ static content boead �ؼ����ǣ�����Ϣ������
******************************************************************/

/*****************************************************************
*  ģ������� �� �ײ㾲̬����̬���ڡ� ʹ�õ���Ϣ������  (�ڶ���ģ�������)
******************************************************************/

LRESULT CALLBACK ModuleManagerProc2 (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	PMODULEMANAGER pMM = GetPModuleManagerByStaticHWND(hwnd);
	CheckNullErrorAndQuit(pMM,14,TEXT("Can't Get ModuleManager By HWND in ModuleManagerProc2()!"));
//	RECT rect;
	int cxWindow,cyWindow,cyChar,i;		

	switch(message)
	{
		case WM_SIZE:
				//���ϵͳĬ������ĸ߶�
			cyChar = HIWORD (GetDialogBaseUnits ()) ;

			cxWindow = LOWORD(lParam);
			cyWindow = HIWORD(lParam);
			//����hStaticBoard(�ڶ���ģ��)���Ӵ���hButtonBoard ��hContentBoard��λ��
			MoveWindow(ModuleManeger2.hButtonBoard,3,3,cxWindow-2*3,cyChar*2,TRUE);

			for(i=0 ; i<ModuleManeger2.countOfModule ; i++)
				MoveWindow(ModuleManeger2.pToolModule[i].hContentBoard,3,3+cyChar*2+3,cxWindow-2*3,cyWindow-3*2-3-cyChar*2,TRUE);
			
			//��ʾ��ǰ��ѡ���ġ����ݴ��ڡ�
			for(i=0; i<ModuleManeger2.countOfModule ;i++){
				
				if(i==idOfCurrentSubModule){
					ShowWindow(ModuleManeger2.pToolModule[i].hContentBoard,SW_SHOW);
				}
				else 
					ShowWindow(ModuleManeger2.pToolModule[i].hContentBoard,SW_HIDE);
			}

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