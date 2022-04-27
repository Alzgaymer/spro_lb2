#include "headers.h"
#include "resource.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;
	//fill window class
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	//register main window 
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
		throw;
	}
	HWND hwnd;
	hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		_T("WinProject"),
		WS_OVERLAPPEDWINDOW,
		//centerized window
		MaxPixelsFSX / 2 - MaxPixelsFSX / 4, MaxPixelsFSY / 2 - MaxPixelsFSY / 4,
		MaxPixelsFSX / 2, MaxPixelsFSY / 2,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hwnd)
	{
		MessageBox(NULL,
			_T("Call to project::Create_Window failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
		throw;
	}
	globalhIst = hInstance;
	ShowWindow(hwnd,
		nCmdShow);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	switch (message)
	{
	case WM_COMMAND:

		switch (wParam)
		{
		case ID_CREATE_FILE:	
			DialogBoxParam(
				globalhIst,
				MAKEINTRESOURCE(IDD_DIALOG1),
				hWnd,
				DlgProc_forCreate,
				0
			);

			break;
		case ID_DELETE_FILE:
			DialogBoxParam(
				globalhIst,
				MAKEINTRESOURCE(IDD_DIALOG2),
				hWnd,
				DlgProc_forDelete,
				0
			);
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(69);
		
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

}
INT_PTR CALLBACK DlgProc_forCreate(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	
	switch (message)
	{
	case WM_INITDIALOG:
		::hwnd_textpox = CreateWindow(
			_T("edit"),
			_T(".txt"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			90, 50,
			110, 20,
			hWnd, (HMENU)ID_TEXTBOX, 0, 0);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			//clear the buff
			wFileName.clear();
			//get text from textbox
			GetWindowText(hwnd_textpox, FileName, 32);

			wFileName = FileName;
			//check if file name correct
			if (CheckFileName(wFileName) == FALSE)
			{
				MessageBox(
					hWnd,
					_T("Wrong file name!"),
					_T("Create file error"),
					MB_OK | MB_ICONWARNING 
				);
			}
			//creating file
			wFileName = _T("C:\\Users\\Notebook\\OneDrive\\Рабочий стол\\") + wFileName;
			//finding file, if it`s exist - end the dilog
			hFileFind = FindFirstFile(&wFileName[0], &FileData);
			if (hFileFind != INVALID_HANDLE_VALUE)
			{
				MessageBox(
					hWnd,
					_T("File is already exist!"),
					_T("Create file error"),
					MB_OK | MB_ICONWARNING
				);
				CloseHandle(hFileFind);
				//end dialog
				SendMessage(hWnd, WM_CLOSE, wParam, lParam);
				break;
			}
			hFileCreate = CreateFile(
				&wFileName[0],	
				GENERIC_WRITE, 0, NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == hFileCreate) 
			{
				MessageBox(
					hWnd,
					_T("File isn`t created!"),
					_T("Create file error"),
					MB_OK | MB_ICONWARNING
				);
				
				//end dialog
				SendMessage(hWnd, WM_CLOSE, wParam, lParam);
			}
			CloseHandle(hFileCreate);
			//end dialog
			SendMessage(hWnd, WM_CLOSE, wParam, lParam);
			break;
		case IDCANCEL:
			//end dialog
			SendMessage(hWnd, WM_CLOSE, wParam, lParam);
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd,0);
		return FALSE;
		break;
	}
	return FALSE;
}
INT_PTR CALLBACK DlgProc_forDelete(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	switch (message)
	{
	case WM_INITDIALOG:
		//initializing a textbox
		::hwnd_textpox = CreateWindow(
			_T("edit"),
			_T(".txt"),
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
			90,50,
			110, 20,
			hWnd, (HMENU)ID_TEXTBOX, 0, 0);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			//clear the buff
			wFileName.clear();
			//get text from textbox
			GetWindowText(hwnd_textpox, FileName, 32);

			wFileName = FileName;
			//check if file name correct
			if (CheckFileName(wFileName) == FALSE)
			{
				MessageBox(
					hWnd,
					_T("Wrong file name!"),
					_T("Delete file error"),
					MB_OK | MB_ICONWARNING
				);
			}
			//setting up the path to the desktop
			wFileName = _T("C:\\Users\\Notebook\\OneDrive\\Рабочий стол\\") + wFileName;
			hFileFind = FindFirstFile(&wFileName[0], &FileData);
			if (hFileFind == INVALID_HANDLE_VALUE)
			{
				MessageBox(
					hWnd,
					_T("File isn`t exist!"),
					_T("Delete file error"),
					MB_OK | MB_ICONWARNING
				);
				CloseHandle(hFileFind);
				//end dialog
				SendMessage(hWnd, WM_CLOSE, wParam, lParam);
				break;
			}
			DeleteFile(&wFileName[0]);
			MessageBox(
				hWnd,
				_T("File succesfully deleted!"),
				_T("Delete file info"),
				MB_OK | MB_ICONINFORMATION
			);
			CloseHandle(hFileCreate);
			//end dialog
			SendMessage(hWnd, WM_CLOSE, wParam, lParam);
			break;
		case IDCANCEL:
			//end dialog
			SendMessage(hWnd, WM_CLOSE, wParam, lParam);
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return FALSE;
		break;
	}
	return FALSE;
}
bool CheckFileName(const wstring& str)
//checks if file name (.) and if it has file extension after (.)
{
	if (str.empty() || str.find(_T(".")) == -1 || str.substr(str.find(_T(".")), str.size() - str.find(_T("."))) == _T(""))
	{
		return FALSE;
	}
	return TRUE;
}