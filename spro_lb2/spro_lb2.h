#pragma once
#include "headers.h"
#pragma region global variables
static TCHAR szWindowClass[] = _T("Title");
static const auto MaxPixelsFSX = GetSystemMetrics(SM_CXFULLSCREEN);
static const auto MaxPixelsFSY = GetSystemMetrics(SM_CYFULLSCREEN);
using std::string, std::wstring;;

static HINSTANCE globalhIst;
static HWND hwnd_textpox;
static HWND main_text_box;
#define ID_TEXTBOX 001
static bool someFileIsOpen = false;
static TCHAR FileName[32];
static	wstring wFileName; 
static HANDLE hFileCreate;
static WIN32_FIND_DATA FileData;
static RECT rt;
static HANDLE hFileFind;
static BY_HANDLE_FILE_INFORMATION FileInfo{0};
#pragma endregion

LRESULT CALLBACK WndProc(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

INT_PTR CALLBACK DlgProc_forCreate(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
INT_PTR CALLBACK DlgProc_forDelete(
	_In_ HWND   hWnd,
	_In_ UINT   message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
bool CheckFileName(const wstring&);
bool ChoseFile(const std::wstring&, HWND hWnd);
void ExtractData(const BY_HANDLE_FILE_INFORMATION&, HWND);