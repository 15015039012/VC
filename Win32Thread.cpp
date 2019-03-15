// Win32Thread.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "Win32Thread.h"

#include "windows.h"
#include "stdio.h"
#include "winsock.h"
#include  "iphlpapi.h"
#include  "shellapi.h"
#include "resource.h"
#include "CheckReplicate.h"

#define MYWM_NOTIFYICON WM_USER+1
#define MAX_LOADSTRING 100
#define IDR_START  1
#define IDR_EXIT 2
// 全局变量: 

HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

int m_count = 0;
HMENU hmenu;
NOTIFYICONDATA notifyico, *pnotifyico;
HICON notifyicon;
wchar_t strIPAddress[][50] = { _T("10.10.0.1"),
_T("10.10.0.2"),
_T("10.10.0.3"),
_T("10.10.0.8"),
_T("10.10.0.17"),
_T("10.10.0.6"),
_T("10.10.0.7")
};  // 需要测试通讯的IP地址







int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	CCheckReplicate  cr =  CCheckReplicate(L"SERVERCHECK");
	if (cr.CheckValue == 0)  cr.CreateM(L"SERVERCHECK");
	else
	{
		MessageBox(0, _T("程序已运行！"), _T("Warnning"), MB_OK);
		return 0;
	}
	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_Accelerator));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MENU);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm =  LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   0, 0, 600, 400, 0, 0, hInstance, 0);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);
   // 创建通知栏图标
   notifyico.cbSize = sizeof(NOTIFYICONDATA);
   notifyico.hWnd = hWnd;
   notifyico.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
   wcscpy_s(notifyico.szTip, _T("服务器响应测试"));
   notifyico.uCallbackMessage = MYWM_NOTIFYICON;
   notifyico.uID = IDI_ICON;
   pnotifyico = &notifyico;
   pnotifyico->hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
   Shell_NotifyIcon(NIM_ADD, pnotifyico);


   hmenu = CreatePopupMenu();//生成菜单  
   AppendMenu(hmenu, MF_STRING, IDR_START, _T("Start"));
   AppendMenu(hmenu, MF_SEPARATOR, IDR_START, _T(""));  //菜单分隔线
   AppendMenu(hmenu, MF_STRING, IDR_EXIT, _T("End"));
   
   return TRUE;
}

bool CheckIPReachable(LPCTSTR strIPAddress)

{
	char strIP[100];

#ifdef UNICODE  
	WideCharToMultiByte(CP_ACP, 0, strIPAddress, (int)_tcslen(strIPAddress) + 1, strIP, sizeof(strIP) / sizeof(char), NULL, NULL);
#else  
	strcpy_s(strIP, strIPAddress);
#endif  

	IPAddr ipaddr = inet_addr(strIP);
	ULONG ulHopCount, ulRTT;
	return (BOOL)GetRTTAndHopCount(ipaddr, &ulHopCount, 3, &ulRTT);
}


void ThreadFunc(int i)
{

	wchar_t aa[50];
	BOOL IP_STATUS = FALSE;
	//LPCWSTR p;
	int j ;
	bool flag = false;
	//	ZeroMemory(aa, 8);
	//	_itow_s(sizeof(ThreadID), aa, 10);
	for (j = 0; j <7; j++)
	{
		
		 IP_STATUS = CheckIPReachable(strIPAddress[j]);
		 lstrcpyW(aa,strIPAddress[j]);
		 /*
		 if (IP_STATUS)
		 {
			 wcscat_s(aa, _T(" 此地址已存在"));
			 MessageBox(0, aa, _T(""), MB_OK);
			 //_itow_s(i, aa, 10);
			 //	Sleep(500);
		 }
		else*/
		 if (!IP_STATUS)
		{
			flag = true;
			wcscat_s(aa, _T(" 无响应！"));
			MessageBox(0, aa,_T("Msg"), MB_OK);
		}
	}
	if (flag==false) 
		MessageBox(0, _T("服务器无异常"), _T("Msg"), MB_OK);

}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HANDLE handle;
	DWORD ThreadID;
	POINT pt;
	int result;
	RECT rect;
//	TCHAR aa[20];
	//获得屏幕尺寸
	int scrWidth, scrHeight;

	switch (message)
	{
	case WM_CREATE:
		
		

		//设置窗体位置
	//	SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
		break;
	case MYWM_NOTIFYICON:
		if (lParam == WM_RBUTTONDOWN)
		{
			GetCursorPos(&pt);//取鼠标坐标  
			::SetForegroundWindow(hWnd);//解决在菜单外单击左键菜单不消失的问题  
		    result = TrackPopupMenu(hmenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);//显示菜单并获取选项ID  
			if (result == IDR_START)
			{
				HANDLE handle = CreateThread(NULL,
					0,
					(LPTHREAD_START_ROUTINE)ThreadFunc,
					(void *)m_count,
					0,
					&ThreadID);
			}
			if (result == IDR_EXIT)
			{
				PostMessage(hWnd, WM_DESTROY, wParam, lParam);
			}
		//	EnableMenuItem(hmenu, IDR_PAUSE, MF_GRAYED);//让菜单中的某一项变灰  
		}
		if (lParam == WM_LBUTTONDBLCLK)
		{
			
					
			scrWidth = GetSystemMetrics(SM_CXSCREEN);
			scrHeight = GetSystemMetrics(SM_CYSCREEN);
			//获取窗体尺寸
	  	//	GetWindowRect(hWnd, &rect);
			//窗口 显示器居中
			rect.left = (scrWidth - 600) / 2;
	     	rect.top = (scrHeight - 400) / 2;
			rect.bottom = 400;
			rect.right = 600;
			SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
			ShowWindow(hWnd, SW_SHOWNORMAL);
            UpdateWindow(hWnd);	
		//	MessageBox(0, _T("TEST"), _T(""), MB_OK);
		}

			//MessageBox(hWnd, _T(""), _T("MSG"), MB_OK);
		break;
		//系统菜单消息
	case WM_SYSCOMMAND:

	
		switch (wParam)
		{
		case SC_CLOSE:
			MessageBox(hWnd, L"使用菜单中的退出！", L"", MB_OK);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD__DIALOG), hWnd, About);
			break;
		case IDM_EXIT:

			DestroyWindow(hWnd);
			break;
		case IDM_THREAD:
			
			handle = CreateThread(NULL,
				0,
				(LPTHREAD_START_ROUTINE)ThreadFunc,
				(void *) m_count,
				0,
				&ThreadID);
			
			CloseHandle(handle);
            break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, pnotifyico);
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		if ((wParam & 0xff) == 1)
		{
			ShowWindow(hWnd, SW_HIDE);
			UpdateWindow(hWnd);
		};
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}