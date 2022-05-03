#include "Utility.cpp"
#include <Windows.h>

const wchar_t CLASS_NAME[] = L"Game Window Class";
const wchar_t WINDOW_NAME[] = L"Pong Ultimate";

GLOBAL bool running = true;

struct RenderState {
	int width;
	int height;
	void* memory;
	BITMAPINFO bitmapInfo;
};
RenderState renderState;

#include "Renderer.cpp"
#include "PlatformCommon.cpp"
#include "Game.cpp"

LRESULT CALLBACK WindowCallback(HWND hwnd, UINT uMsg, WPARAM wPara, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		running = false;
		break;
	case WM_SIZE:
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		renderState.width = rect.right - rect.left;
		renderState.height = rect.bottom - rect.top;

		int bufferSize = renderState.width * renderState.height * sizeof(u32);

		if (renderState.memory) VirtualFree(renderState.memory, 0, MEM_RELEASE);
		renderState.memory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		renderState.bitmapInfo.bmiHeader.biSize = sizeof(renderState.bitmapInfo.bmiHeader);
		renderState.bitmapInfo.bmiHeader.biWidth = renderState.width;
		renderState.bitmapInfo.bmiHeader.biHeight = renderState.height;
		renderState.bitmapInfo.bmiHeader.biPlanes = 1;
		renderState.bitmapInfo.bmiHeader.biBitCount = 32;
		renderState.bitmapInfo.bmiHeader.biCompression = BI_RGB;
	}	break;
	default:
		result = DefWindowProc(hwnd, uMsg, wPara, lParam);
	}

	return result;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	ShowCursor(FALSE);
	//Create a window class
	WNDCLASS windowClass = {};
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = CLASS_NAME;
	windowClass.lpfnWndProc = WindowCallback;

	//Register class
	RegisterClass(&windowClass);

	//Create window
	HWND window = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		WINDOW_NAME,						// Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	{
		//Fullscreen
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}

	if (window == NULL) return 0;

	ShowWindow(window, nCmdShow);

	HDC hdc = GetDC(window);

	Input input = {};

	float deltaTime = 0.016666f;
	LARGE_INTEGER frameBeginTime;
	QueryPerformanceCounter(&frameBeginTime);

	LARGE_INTEGER performance;
	QueryPerformanceFrequency(&performance);
	float performanceFrequency = (float)performance.QuadPart;

	//Run Program
	while (running)
	{
		//Input
		MSG message;

		for (int i = 0; i < BUTTON_COUNT; i++)
		{
			input.buttonsState[i].changed = false;
		}

		while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
		{
			switch (message.message)
			{
				case WM_KEYUP:
				case WM_KEYDOWN:
				{
					u32 virtualKey = (u32)message.wParam;
					bool isDown = ((message.lParam & (1 << 31)) == 0);

#define PROCESS_BUTTON_STATE(b, vk)\
case vk: {\
input.buttonsState[b].changed = isDown != input.buttonsState[b].isDown;\
input.buttonsState[b].isDown = isDown;\
} break;

					switch (virtualKey)
					{
						PROCESS_BUTTON_STATE(BUTTON_UP, VK_UP);
						PROCESS_BUTTON_STATE(BUTTON_DOWN, VK_DOWN);
						PROCESS_BUTTON_STATE(BUTTON_W, 'W');
						PROCESS_BUTTON_STATE(BUTTON_S, 'S');
						PROCESS_BUTTON_STATE(BUTTON_LEFT, VK_LEFT);
						PROCESS_BUTTON_STATE(BUTTON_RIGHT, VK_RIGHT);
						PROCESS_BUTTON_STATE(BUTTON_ENTER, VK_RETURN);
					}
				} break;

				default:
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
		}

		//Simulate
		SimulateGame(&input, deltaTime);

		//Render
		StretchDIBits(hdc, NULL, NULL, renderState.width, renderState.height, NULL, NULL, renderState.width, renderState.height, renderState.memory, &renderState.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

		LARGE_INTEGER frameEndTime;
		QueryPerformanceCounter(&frameEndTime);

		deltaTime = (float)(frameEndTime.QuadPart - frameBeginTime.QuadPart) / performanceFrequency;
		frameBeginTime = frameEndTime;
	}
}