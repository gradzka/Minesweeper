#include <windows.h>

#define SAPER_LOGO 1000

#define horizontal_coordinates (horizontal / 2 -  width / 2)
#define vertical_coordinates (vertical / 2 - height / 2)

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void GetDesktopResolution(int &horizontal, int &vertical);
HWND **get_hwnd_matrix(HWND hwnd, HINSTANCE hInstance);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPSTR ClassName = "Minesweeper";
	MSG Communique;

	//screen sizes, GetDesktopResolution will update them
	int horizontal = 0;
	int vertical = 0;

	//window sizes
	int width = 200;
	int height = 270;

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(SAPER_LOGO));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(SAPER_LOGO));

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "wc structure variable wasn't registered properly!", "ERROR",
			MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	GetDesktopResolution(horizontal, vertical);
	
	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(200));

	// Creating window
	HWND hwnd = CreateWindowEx(
		WS_EX_WINDOWEDGE,
		ClassName,
		ClassName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		horizontal_coordinates,
		vertical_coordinates,
		width,
		height,
		NULL,
		hMenu,
		hInstance,
		NULL);

	if (hwnd == NULL)
	{
		MessageBox(NULL, "Error", "Window Error", MB_ICONEXCLAMATION);
		return 1;
	}

	HWND hwnd_smile = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
		width/2-22, 2, 25, 25, hwnd, (HMENU)0, hInstance, 0);

	HBITMAP hbit = (HBITMAP)LoadImage(NULL, "facesmile.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);

	SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit);

	//HWND matrix
	HWND **hwnd_matrix = get_hwnd_matrix(hwnd, hInstance);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Communique, NULL, 0, 0))
	{
		TranslateMessage(&Communique);
		DispatchMessage(&Communique);
	}
	return Communique.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}
// width and height in pix:
void GetDesktopResolution(int &horizontal, int &vertical)
{
	RECT desktop;

	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
HWND **get_hwnd_matrix(HWND hwnd, HINSTANCE hInstance)
{
	//HWND matrix
	HWND **hwnd_matrix = new HWND *[10];

	for (int i = 0; i < 10; i++)
	{
		hwnd_matrix[i] = new HWND[10];

		for (int j = 0; j < 10; j++)
		{
			hwnd_matrix[i][j] = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
				7 + 17 * j, 30 + 17 * i, 17, 17, hwnd, (HMENU)(10 * i + j + 1), hInstance, 0);
		}
	}
	return hwnd_matrix;
}
