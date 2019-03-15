// Win32Thread.cpp : ����Ӧ�ó������ڵ㡣
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
// ȫ�ֱ���: 

HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

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
};  // ��Ҫ����ͨѶ��IP��ַ







int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	CCheckReplicate  cr =  CCheckReplicate(L"SERVERCHECK");
	if (cr.CheckValue == 0)  cr.CreateM(L"SERVERCHECK");
	else
	{
		MessageBox(0, _T("���������У�"), _T("Warnning"), MB_OK);
		return 0;
	}
	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_Accelerator));

	// ����Ϣѭ��: 
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
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
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
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   0, 0, 600, 400, 0, 0, hInstance, 0);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);
   // ����֪ͨ��ͼ��
   notifyico.cbSize = sizeof(NOTIFYICONDATA);
   notifyico.hWnd = hWnd;
   notifyico.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
   wcscpy_s(notifyico.szTip, _T("��������Ӧ����"));
   notifyico.uCallbackMessage = MYWM_NOTIFYICON;
   notifyico.uID = IDI_ICON;
   pnotifyico = &notifyico;
   pnotifyico->hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
   Shell_NotifyIcon(NIM_ADD, pnotifyico);


   hmenu = CreatePopupMenu();//���ɲ˵�  
   AppendMenu(hmenu, MF_STRING, IDR_START, _T("Start"));
   AppendMenu(hmenu, MF_SEPARATOR, IDR_START, _T(""));  //�˵��ָ���
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
			 wcscat_s(aa, _T(" �˵�ַ�Ѵ���"));
			 MessageBox(0, aa, _T(""), MB_OK);
			 //_itow_s(i, aa, 10);
			 //	Sleep(500);
		 }
		else*/
		 if (!IP_STATUS)
		{
			flag = true;
			wcscat_s(aa, _T(" ����Ӧ��"));
			MessageBox(0, aa,_T("Msg"), MB_OK);
		}
	}
	if (flag==false) 
		MessageBox(0, _T("���������쳣"), _T("Msg"), MB_OK);

}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
	//�����Ļ�ߴ�
	int scrWidth, scrHeight;

	switch (message)
	{
	case WM_CREATE:
		
		

		//���ô���λ��
	//	SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
		break;
	case MYWM_NOTIFYICON:
		if (lParam == WM_RBUTTONDOWN)
		{
			GetCursorPos(&pt);//ȡ�������  
			::SetForegroundWindow(hWnd);//����ڲ˵��ⵥ������˵�����ʧ������  
		    result = TrackPopupMenu(hmenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);//��ʾ�˵�����ȡѡ��ID  
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
		//	EnableMenuItem(hmenu, IDR_PAUSE, MF_GRAYED);//�ò˵��е�ĳһ����  
		}
		if (lParam == WM_LBUTTONDBLCLK)
		{
			
					
			scrWidth = GetSystemMetrics(SM_CXSCREEN);
			scrHeight = GetSystemMetrics(SM_CYSCREEN);
			//��ȡ����ߴ�
	  	//	GetWindowRect(hWnd, &rect);
			//���� ��ʾ������
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
		//ϵͳ�˵���Ϣ
	case WM_SYSCOMMAND:

	
		switch (wParam)
		{
		case SC_CLOSE:
			MessageBox(hWnd, L"ʹ�ò˵��е��˳���", L"", MB_OK);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
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
		// TODO:  �ڴ���������ͼ����...
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



// �����ڡ������Ϣ�������
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