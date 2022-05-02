#include "headers.h"
#include "resource.h"


// for correct work need c++20 or newer
// UNICODE character set
// and Linker\system\subsystem\windows
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
	string initialLocation, newLocation;
	wstring wCutLocation, wNewLocation;
	int backslashPos;
	const wchar_t* wcNewLocation = nullptr;
	const wchar_t* wcOldLocation = nullptr;
	switch (message)
	{
	case WM_COMMAND:

		switch (wParam)
		{
		case ID_CREATE_FILE:
			DialogBoxParam(
				::globalhIst,
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
		case ID_FILE_MOVE:
			ChoseFile(L".txt", hWnd);
			initialLocation = std::string(wFileName.begin(), wFileName.end());
			backslashPos = initialLocation.find_last_of("\\", wFileName.size() - 1);
			wCutLocation = wFileName.substr(backslashPos + 1, wFileName.size() - backslashPos);
			newLocation = "D:\\win32 files\\" + std::string(wCutLocation.begin(), wCutLocation.end());
			wNewLocation = std::wstring(newLocation.begin(), newLocation.end());
			wcNewLocation = wNewLocation.c_str();
			wcOldLocation = wFileName.c_str();
			MoveFile(wcOldLocation, wcNewLocation);
			break;
		case ID_FILE_FILEINFO:
			ChoseFile(L".txt", hWnd);
			hFileFind = CreateFileW(&wFileName[0], 0,
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL);
			ZeroMemory(&FileInfo, sizeof(BY_HANDLE_FILE_INFORMATION));

			if (!GetFileInformationByHandle(hFileFind, &FileInfo))
			{
				if (hFileFind == INVALID_HANDLE_VALUE)
				{
					MessageBox(hWnd, L"Handle file error", L"FILE INFO ERROR", MB_OK | MB_ICONERROR);

					return 0;
				}
				MessageBox(hWnd, L"Cann`t get information from file", L"FILE INFO ERROR", MB_OK | MB_ICONERROR);
				return 0;
			}
			else
				ExtractData(FileInfo, hWnd);

			break;
		default:
			break;
		}
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rt);
		break;
	case WM_DESTROY:
		PostQuitMessage(69);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

}
//dialog message receiver for (Create file)`s menu
INT_PTR CALLBACK DlgProc_forCreate(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	
	switch (message)
	{
	//initializing the dialog window
	case WM_INITDIALOG:
		::hwnd_textpox = CreateWindow(
			_T("edit"),										//textbox
			_T(".txt"),										//value which shows by start
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
			//copying TCHAR to std::wstring
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
			//adding to the file path the desktop path (for correct work if need change `Notebook` to your username)
			wFileName = _T("C:\\Users\\Notebook\\OneDrive\\Рабочий стол\\") + wFileName;
			//finding file, if it`s exist - end the dilog
			hFileFind = FindFirstFile(&wFileName[0], &FileData);
			//checks if file is already exist
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
			//creating file
			hFileCreate = CreateFile(
				&wFileName[0],	
				GENERIC_WRITE, 0, NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL, NULL);
			//checks if file wasn`t created
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
	//initializing a dialog win 
	case WM_INITDIALOG:
		//initializing a textbox
		::hwnd_textpox = CreateWindow(
			_T("edit"),										//textbox								
			_T(".txt"),										//value which shows by start
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
			//copying TCHAR to std::wstring
			wFileName = FileName;
			//setting up the path to the desktop
			wFileName = _T("C:\\Users\\Notebook\\OneDrive\\Рабочий стол\\") + wFileName;
			//finding file
			hFileFind = FindFirstFile(&wFileName[0], &FileData);
			//if NOT found repeat dialog delete box
			if (hFileFind == INVALID_HANDLE_VALUE)
			{
				MessageBox(
					hWnd,
					_T("File doesn`t exist!"),
					_T("Delete file error"),
					MB_OK | MB_ICONWARNING
				);
				CloseHandle(hFileFind);
				//end dialog
				return FALSE;
				break;
			}
			//deletes file (&wFileName[0] is a converting a wstring to LPCWSTR)
			DeleteFile(& wFileName[0]);
			//throws a message box about succesfull deleting
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
//checks if file name correct
//if successful returns TRUE(1)
{	//(if it is empty) (if the dot was NOT found) (if the file extension after (.) was NOT declared)
 	if (str.empty() || str.find(_T(".")) == -1 || str.substr(str.find(_T("."))+1, str.size() - str.find(_T("."))) == _T("\0"))
	{
		return FALSE;
	}
	return TRUE;
}
bool ChoseFile(const std::wstring& str1, HWND hWnd)
{
	std::wstring str(str1);

	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[256];       // buffer for file name
	HANDLE hf; // open file handle
	if (str == _T(".txt"))
	{
		str = _T("Text files\0*") + str;
	}
	else if (str == _T(".exe"))
	{
		str = _T("Execution files\0*") + str;
	}
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = str.c_str();
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn) == TRUE)
	{
		hf = CreateFile(ofn.lpstrFile, GENERIC_READ,
			0, (LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
			(HANDLE)NULL);

		DWORD dwBytesToRead = 1023;
		DWORD bytesRead = 0;
		byte byteBuffer[1024];
		memset(&byteBuffer, 0, sizeof(byteBuffer));
		ReadFile(hf, byteBuffer, dwBytesToRead, &bytesRead, NULL);

		wchar_t buffer[1024];
		memset(&buffer, 0, sizeof(byteBuffer));
		for (int position = 0; position < 1023; position++)
			buffer[position] = (wchar_t)byteBuffer[position];
		SetWindowText(main_text_box, (LPCWSTR)buffer);

		wFileName = ofn.lpstrFile;
		
		CloseHandle(hf);
		return true; //succsesfull 
	}
	else return false;//unsuccsesfull
}
void ExtractData(const BY_HANDLE_FILE_INFORMATION& Info, HWND hWnd) 
{
	HDC WindowDC = GetDC(hWnd);
	//HDC TemporaryDC = CreateCompatibleDC(WindowDC);
	//
	//GetWindowRect(hWnd, &rt);

	//auto BitmapDC = CreateCompatibleBitmap(WindowDC, rt.right - rt.left, rt.bottom - rt.top);
	//
	////take bitmap as a dc
	//SelectObject(TemporaryDC, BitmapDC);
	wstring str{ L"LAST TIME WRITE: " };
	str += Info.ftLastWriteTime.dwLowDateTime;
	
	//DrawText(WindowDC, L"1234567890", 11, &rt, 0);
	TextOut(WindowDC, 0, 0, &str[0], str.size());
	//SetWindowText(hWnd, &str[0]);
	/*BitBlt(WindowDC, 0, 0, rt.right - rt.left, rt.bottom - rt.top, TemporaryDC, 0, 0, SRCCOPY);
	
	DeleteDC(TemporaryDC);
	DeleteObject(BitmapDC);*/
}