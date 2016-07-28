#include <Windows.h>
#include <WinUser.h>
#include <stdio.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#define hookButton 12

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI HookThreadFunc(LPVOID);
HHOOK hhk;
HWND targetHandle, hHexHandlerLabel, hIntHandlerLabel, hCaptureButton;
int interceptIsOn = 0;
_declspec(dllexport) LRESULT CALLBACK MouseEvent(int Code, WPARAM wParam, LPARAM lParam) {
	
	if (Code == HC_ACTION) {
		switch (wParam)
		{
		case WM_MOUSEMOVE:
		{
			if (interceptIsOn == 1) {
				MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);
				HWND targetHandle = WindowFromPoint(mouse.pt);
				char strHexHandle[64], strIntHandle[64];
				sprintf_s(strHexHandle, "%p", targetHandle);
				sprintf_s(strIntHandle, "%d", targetHandle);
				SetWindowTextA(hHexHandlerLabel, strHexHandle);
				SetWindowTextA(hIntHandlerLabel, strIntHandle);
			}			
		}
			break;
		case WM_LBUTTONUP:
			EnableWindow(hCaptureButton, true);
			interceptIsOn = 0;
			break;
		case WM_LBUTTONDOWN:
			if (WindowFromPoint((*((MSLLHOOKSTRUCT*)lParam)).pt) == hCaptureButton){
				interceptIsOn = 1;
				EnableWindow(hCaptureButton,false);
			}
			break;
		default:
			break;
		}
		
	}
	return CallNextHookEx(hhk, Code, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow) {

	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = "FirstWnd";
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Error registering", "Error", MB_ICONEXCLAMATION | MB_OK);
	}
	hwnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_TOPMOST, "FirstWnd", "Window Controller", WS_OVERLAPPEDWINDOW^WS_THICKFRAME^WS_MAXIMIZEBOX, 500, 100, 345, 420, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	

	while (GetMessage(&msg,NULL,0,0)>0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
	case WM_CREATE: {
		HANDLE hookThread;
		DWORD dwThread;
		hookThread = CreateThread(0, 0, HookThreadFunc, NULL, NULL,&dwThread);


		hHexHandlerLabel = CreateWindowEx(WS_EX_STATICEDGE, "STATIC", "Hex Handle", WS_CHILD | WS_VISIBLE |SS_CENTER, 7, 80, 80, 25, hwnd, (HMENU)123, NULL, NULL);
		hIntHandlerLabel = CreateWindowEx(WS_EX_STATICEDGE, "STATIC", "Int Handle", WS_CHILD | WS_VISIBLE | SS_CENTER, 241, 80, 80, 25, hwnd, (HMENU)124, NULL, NULL);
		hCaptureButton = CreateWindowEx(WS_EX_STATICEDGE, "BUTTON", "Capture window", WS_CHILD | WS_VISIBLE, 87, 80, 154, 25, hwnd, (HMENU)hookButton, NULL, NULL);
		break;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}



DWORD WINAPI HookThreadFunc(LPVOID Param) {
	HINSTANCE exeHandle = GetModuleHandle(NULL);
	hhk = SetWindowsHookEx(WH_MOUSE_LL, MouseEvent, exeHandle, NULL);
	MSG msg;
	while (GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnhookWindowsHookEx(hhk);
	return 1;
}