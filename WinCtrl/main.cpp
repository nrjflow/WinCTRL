#include <Windows.h>
#include <WinUser.h>
#include <stdio.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI HookThreadFunc(LPVOID);
HHOOK hhk;
HWND hLabel, hButton;
#define hookButton 12
_declspec(dllexport) LRESULT CALLBACK MouseEvent(int Code, WPARAM wParam, LPARAM lParam) {
	
	if (Code == HC_ACTION) {
		switch (wParam)
		{
		case WM_MOUSEMOVE:
		{
			MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);
			HWND test = WindowFromPoint(mouse.pt);
			char strHandle[64];
			sprintf_s(strHandle, "%p", test);
			SetWindowTextA(hLabel, strHandle);
		}
		break;
		case WM_LBUTTONUP:
			if (WindowFromPoint((*((MSLLHOOKSTRUCT*)lParam)).pt) == hButton){
				MessageBox(NULL, "Maaa nigga", "", MB_OK);
			}
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
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Error registering", "Error", MB_ICONEXCLAMATION | MB_OK);
	}
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "FirstWnd", "Window Controller", WS_OVERLAPPEDWINDOW, 490, 80, 340, 420, NULL, NULL, hInstance, NULL);

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


		hLabel = CreateWindowEx(WS_EX_CLIENTEDGE, "STATIC", "ggg", WS_CHILD | WS_VISIBLE, 2, 2, 100, 100, hwnd, (HMENU)123, NULL, NULL);
		hButton = CreateWindowEx(WS_EX_CLIENTEDGE, "BUTTON", "Click and drag", WS_CHILD | WS_VISIBLE , 100, 100, 100, 20, hwnd, (HMENU)hookButton, NULL, NULL);
		if (hLabel==NULL) {
			MessageBox(NULL, "Error registering", "Error", MB_ICONEXCLAMATION | MB_OK);
		}
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