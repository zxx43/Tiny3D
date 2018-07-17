#include <windows.h>
#include <windowsx.h>
#include "simpleApplication.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HWND hWnd;
HDC hdc;
HGLRC hrc;
HINSTANCE hInstance;
const TCHAR szName[]=TEXT("win");

HANDLE actThread = NULL, frameThread = NULL;
bool thread1End, thread2End;
DWORD WINAPI ActThreadRun(LPVOID param);
DWORD WINAPI FrameThreadRun(LPVOID param);
void CreateThreads();
void ReleaseThreads();
bool dataPrepared = false;
HANDLE mutex = NULL;
void InitMutex();
void DeleteMutex();
DWORD currentTime = 0, startTime = 0;
DWORD screenLeft, screenTop;
int screenHalfX, screenHalfY;
POINT mPoint;

SimpleApplication* app = NULL;
void CreateApplication();
void ReleaseApplication();

float fullscreen = 0.0;

void KillWindow() {
	DeleteMutex();
	ReleaseThreads();
	ReleaseApplication();
	ShowCursor(true);
	if (fullscreen)
		ChangeDisplaySettings(NULL, 0);
	wglMakeCurrent(NULL,NULL);
	wglDeleteContext(hrc);
	ReleaseDC(hWnd,hdc);
	DestroyWindow(hWnd);
	UnregisterClass(szName,hInstance);
}

void ResizeWindow(int width,int height) {
	app->resize(width, height);
}

void DrawWindow() {
	WaitForSingleObject(mutex, INFINITE);
	dataPrepared = true;
	ReleaseMutex(mutex);
	app->draw();
	WaitForSingleObject(mutex, INFINITE);
	dataPrepared = false;
	ReleaseMutex(mutex);
	/*
	GetCursorPos(&mPoint);
	app->moveMouse(mPoint.x, mPoint.y, screenHalfX, screenHalfY);
	SetCursorPos(screenHalfX, screenHalfY);
	app->moveKey();
	//*/
}

DWORD WINAPI ActThreadRun(LPVOID param) {
	///*
	DWORD last = 0;
	while (!app->willExit) {
		if (currentTime - last > 1) {
			GetCursorPos(&mPoint);
			app->moveMouse(mPoint.x, mPoint.y, screenHalfX, screenHalfY);
			SetCursorPos(screenHalfX, screenHalfY);
			app->moveKey();

			last = currentTime;
		}
		Sleep(0);
	}
	//*/
	thread1End = true;
	return 1;
}

DWORD WINAPI FrameThreadRun(LPVOID param) {
	while (!app->willExit) {
		currentTime = timeGetTime();
		if(!dataPrepared) {
			app->act(startTime, currentTime);
			app->prepare();
			app->animate(startTime, currentTime);

			WaitForSingleObject(mutex, INFINITE);
			dataPrepared = true;
			ReleaseMutex(mutex);
		}
	}
	thread2End = true;
	return 1;
}

void InitGLWin() {
	const PIXELFORMATDESCRIPTOR pfd={
			sizeof(PIXELFORMATDESCRIPTOR),1,
			PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,32,
			0,0,0,0,0,0,
			0,0,0,
			0,0,0,0,
			24,0,0,
			PFD_MAIN_PLANE,0,0,0,0
	};
	hdc=GetDC(hWnd);
	int pixelFormat=ChoosePixelFormat(hdc,&pfd);
	SetPixelFormat(hdc,pixelFormat,&pfd);
	hrc=wglCreateContext(hdc);
	wglMakeCurrent(hdc,hrc);
}

void InitGL() {
	ShowCursor(false);
	app->init();
	InitMutex();
	CreateThreads();
}

void CreateThreads() {
	actThread = CreateThread(NULL, 0, ActThreadRun, NULL, 0, NULL);
	frameThread = CreateThread(NULL, 0, FrameThreadRun, NULL, 0, NULL);
	thread1End = false;
	thread2End = false;
}

void ReleaseThreads() {
	CloseHandle(frameThread);
	CloseHandle(actThread);
}

void InitMutex() {
	mutex = CreateMutex(NULL, FALSE, NULL);
}

void DeleteMutex() {
	ReleaseMutex(mutex);
	CloseHandle(mutex);
}

void CreateApplication() {
	app = new SimpleApplication();
	app->config->get("fullscreen", fullscreen);
	startTime = timeGetTime();
}

void ReleaseApplication() {
	if (app) delete app;
	app = NULL;
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInstance,PSTR szCmdLine,int iCmdShow) {
	MSG msg;
	WNDCLASS wndClass;
	hInstance=hInst;

	wndClass.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wndClass.lpfnWndProc=WndProc;
	wndClass.cbClsExtra=0;
	wndClass.cbWndExtra=0;
	wndClass.hInstance=hInstance;
	wndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName=NULL;
	wndClass.lpszClassName=szName;

	if(!RegisterClass(&wndClass)) {
		MessageBox(NULL,TEXT("Can not create!"),szName,MB_ICONERROR);
		return 0;
	}

	CreateApplication();

	DWORD style=WS_OVERLAPPEDWINDOW;
	DWORD styleEX=WS_EX_APPWINDOW|WS_EX_WINDOWEDGE;

	if (fullscreen) {
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(DEVMODE));
		dmScreenSettings.dmSize = sizeof(DEVMODE);
		dmScreenSettings.dmPelsWidth = app->windowWidth;
		dmScreenSettings.dmPelsHeight = app->windowHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			MessageBox(NULL, TEXT("Can not full screen!"), szName, MB_ICONERROR);
			fullscreen = 0.0;
		} else {
			style = WS_POPUP;
			styleEX = WS_EX_APPWINDOW;
		}
	}

	RECT winRect;
	winRect.left=(LONG)0;
	winRect.right=(LONG)app->windowWidth;
	winRect.top=(LONG)0;
	winRect.bottom=(LONG)app->windowHeight;
	screenLeft=(GetSystemMetrics(SM_CXSCREEN)>>1)-(app->windowWidth>>1);
	screenTop=(GetSystemMetrics(SM_CYSCREEN)>>1)-(app->windowHeight>>1);
	screenHalfX = (int)GetSystemMetrics(SM_CXSCREEN) >> 1;
	screenHalfY = (int)GetSystemMetrics(SM_CYSCREEN) >> 1;

	AdjustWindowRectEx(&winRect,style,false,styleEX);
	hWnd=CreateWindowEx(styleEX,szName,TEXT("Tiny"),WS_CLIPSIBLINGS|WS_CLIPCHILDREN|style,
			screenLeft,screenTop,(winRect.right-winRect.left),(winRect.bottom-winRect.top),
			NULL,NULL,hInstance,NULL);

	InitGLWin();
	ShowWindow(hWnd,iCmdShow);
	InitGL();
	ResizeWindow(app->windowWidth,app->windowHeight);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	UpdateWindow(hWnd);

	//SendMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	while(!app->willExit) {
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			if(msg.message==WM_QUIT) {
				app->willExit=true;
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} 
	}
	while (!thread1End || !thread2End)
		Sleep(0);
	KillWindow();
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	static PAINTSTRUCT ps;
	switch(msg) {
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			if (!app->willExit)
				DrawWindow();
			SwapBuffers(hdc);
			EndPaint(hWnd, &ps);
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYDOWN:
			app->keyDown(wParam);
			break;
		case WM_KEYUP:
			app->keyUp(wParam);
			break;
		case WM_MOUSEMOVE:
			/*
			if (app) {
				GetCursorPos(&mPoint);
				app->moveMouse(mPoint.x, mPoint.y, screenHalfX, screenHalfY);
				SetCursorPos(screenHalfX, screenHalfY);
			}
			//*/
			break;
		case WM_SIZE:
			ResizeWindow(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
	}

	return 0;
}

