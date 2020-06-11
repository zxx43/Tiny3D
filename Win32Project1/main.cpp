#include <windows.h>
#include <windowsx.h>
#include "simpleApplication.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HWND hWnd;
HDC hdc;
HGLRC hrc;
HINSTANCE hInstance;
const TCHAR szName[]=TEXT("win");

HANDLE frameThread = NULL;
bool threadEnd;
DWORD WINAPI FrameThreadRun(LPVOID param);
void CreateThreads();
void ReleaseThreads();
bool dataPrepared = false;
HANDLE mutex = NULL;
void InitMutex();
void DeleteMutex();
DWORD currentTime = 0, lastTime = 0, startTime = 0;
CirQueue<float>* dTimes = NULL;
float velocity = 0.0;
DWORD screenLeft, screenTop;
int screenHalfX, screenHalfY;
int centerX, centerY;
RECT winRect;
POINT mPoint;
bool mouseShow = false;

SimpleApplication* app = NULL;
void CreateApplication();
void ReleaseApplication();

bool fullscreen = false;
bool inited = false;

void SwitchMouse() {
	if (app->isMouseShow() && !mouseShow) {
		ShowCursor(true);
		mouseShow = true;
	} else if (!app->isMouseShow() && mouseShow) {
		ShowCursor(false);
		mouseShow = false;
	}
}

void KillWindow() {
	if (dTimes) delete dTimes;
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

void ActRun() {
	currentTime = timeGetTime();
	float dTime = (float)(currentTime - lastTime);
	lastTime = currentTime;
	if (app->cfgs->dualthread) {
		dTimes->push(dTime);
		float dSum = 0.0;
		for (int i = 0; i < dTimes->size; ++i)
			dSum += dTimes->data[i];
		dTime = dSum / dTimes->size;
	}
	app->setFps(1000.0f / dTime);
	velocity = D_DISTANCE * dTime;

	if (app->pressed || app->input->getControl() >= 0) {
		GetCursorPos(&mPoint);
		app->moveMouse(mPoint.x, mPoint.y, centerX, centerY);
		SetCursorPos(centerX, centerY);
		app->hideMouse();
	}
	else app->showMouse();
	app->wheelAct();
	app->keyAct(velocity);
	app->act(startTime, currentTime, velocity);
}

void DrawWindow() {
	if (!app->cfgs->dualthread) {
		ActRun();
		app->prepare(false);
	} else {
		WaitForSingleObject(mutex, INFINITE);
		dataPrepared = true;
		ReleaseMutex(mutex);
	}

	app->draw();

	if (app->cfgs->dualthread) {
		WaitForSingleObject(mutex, INFINITE);
		dataPrepared = false;
		ReleaseMutex(mutex);
	}

	SwitchMouse();
}

DWORD WINAPI FrameThreadRun(LPVOID param) {
	while (!app->willExit && app->cfgs->dualthread) {
		if(!dataPrepared && inited) {
			ActRun();
			app->prepare(true);

			WaitForSingleObject(mutex, INFINITE);
			dataPrepared = true;
			ReleaseMutex(mutex);
		}
	}
	threadEnd = true;
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
	mouseShow = false;
	printf("Init GL\n");
	app->init();
	dTimes = new CirQueue<float>(app->cfgs->smoothframe);
	InitMutex();
	CreateThreads();
	inited = true;
}

void CreateThreads() {
	frameThread = CreateThread(NULL, 0, FrameThreadRun, NULL, 0, NULL);
	threadEnd = false;
}

void ReleaseThreads() {
	CloseHandle(frameThread);
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
	fullscreen = app->cfgs->fullscreen;
	startTime = timeGetTime();
	currentTime = startTime;
	lastTime = startTime;
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
			fullscreen = false;
		} else {
			style = WS_POPUP;
			styleEX = WS_EX_APPWINDOW;
		}
	}

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

	//LoadKeyboardLayout(L"00000409", KLF_ACTIVATE);
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

	while (!threadEnd) Sleep(0);
	KillWindow();
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	static PAINTSTRUCT ps;
	switch(msg) {
		case WM_PAINT:
			BeginPaint(hWnd, &ps);
			if (!app->willExit && inited)
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
		case WM_LBUTTONDOWN:
			app->mouseKey(true, true);
			break;
		case WM_LBUTTONUP:
			app->mouseKey(false, true);
			break;
		case WM_RBUTTONDOWN:
			app->mouseKey(true, false);
			break;
		case WM_RBUTTONUP:
			app->mouseKey(false, false);
			break;
		case WM_SIZE:
			ResizeWindow(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			GetWindowRect(hWnd, &winRect);
			centerX = winRect.left + (LONG)(app->windowWidth >> 1);
			centerY = winRect.top + (LONG)(app->windowHeight >> 1);
			break;
		case WM_MOUSEWHEEL:
			{	
				short dWheel = GET_WHEEL_DELTA_WPARAM(wParam);
				if (dWheel > 0)
					app->setWheelDir(MNEAR);
				else if (dWheel < 0)
					app->setWheelDir(MFAR);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd,msg,wParam,lParam);
	}

	return 0;
}

