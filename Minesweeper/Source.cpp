#include <windows.h>
#include <string>
#include "game_board.h"

#define SAPER_LOGO 1000
#define horizontal_coordinates (horizontal / 2 -  width / 2)
#define vertical_coordinates (vertical / 2 - height / 2)

HBITMAP hbit_BMPs[16];

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewSafeBtnProc(HWND hButton, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void GetDesktopResolution(int &horizontal, int &vertical);
HWND **get_hwnd_matrix(HWND hwnd, HINSTANCE hInstance);
void load_BITMAP();

//Source of Subclassing Example: https://code.msdn.microsoft.com/windowsapps/CppWindowsSubclassing-2ef7ee53
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")
//
//  FUNCTION: NewSafeBtnProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR)
//
//  PURPOSE:  The new procedure that processes messages for the button control. 
//  Every time a message is received by the new window procedure, a subclass 
//  ID and reference data are included.
//

char b[32];
HWND hwnd_smile;

game_board *g_b_gameboard;
int g_b_X = 0;
int g_b_Y = 0;

HWND **hwnd_matrix;
bool GAME_OVER = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	g_b_gameboard = new game_board(10,10,9); //Custom setting, Game_board 10x10 and 9 mines


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
	wc.hbrBackground = (HBRUSH)(1);
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

	hwnd_smile = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
		width / 2 - 22, 5, 26, 26, hwnd, (HMENU)100, hInstance, 0);

	//HBITMAP hbit = (HBITMAP)LoadImage(NULL, "BMPs\\smile.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	load_BITMAP();
	SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);

	//HWND matrix
	hwnd_matrix = get_hwnd_matrix(hwnd, hInstance);

	// Get the handle of the control to be subclassed, and subclass it.
	for (int i = 0; i < 100; i++)
	{
		if (!SetWindowSubclass(GetDlgItem(hwnd, i), NewSafeBtnProc, 0, i))
		{
			DestroyWindow(hwnd);
		}
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Communique, NULL, 0, 0))
	{
		TranslateMessage(&Communique);
		DispatchMessage(&Communique);
	}
	return Communique.wParam;

	delete g_b_gameboard;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[14]);
		break;

	case WM_LBUTTONUP:
		SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
		break;

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
LRESULT CALLBACK NewSafeBtnProc(HWND hButton, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
		SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[14]);
		return TRUE;

	case WM_LBUTTONUP:

		SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); //change smile

		g_b_X = dwRefData / 10;
		g_b_Y = dwRefData%10;

		/*_itoa_s(g_b_X, b, 10);
		MessageBox(hButton, b, "X", MB_OK);

		_itoa_s(g_b_Y, b, 10);
		MessageBox(hButton, b, "Y", MB_OK);*/


		if (g_b_gameboard->fields[g_b_X][g_b_Y].discovered == false && g_b_gameboard->fields[g_b_X][g_b_Y].flagged == false) //if button wasn't discovered
		{
			g_b_gameboard->fields[g_b_X][g_b_Y].discovered = true;
			//dwRefData == 1 &&
			switch (g_b_gameboard->fields[g_b_X][g_b_Y].value)
			{
			case -1:
			{
				if (GAME_OVER == false){
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[11]);

					for (int i = 0; i < 10; i++)
					{
						for (int j = 0; j < 10; j++)
						{
							if (g_b_gameboard->fields[i][j].value == -1 && g_b_gameboard->fields[i][j].discovered == false)
							{
								SendMessage(hwnd_matrix[i][j], BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[9]);
								g_b_gameboard->fields[i][j].discovered = true;
								g_b_gameboard->fields[i][j].flagged = true;
							}
							else
							{
								g_b_gameboard->fields[i][j].discovered = true;
								g_b_gameboard->fields[i][j].flagged = true;
							}
						}
					}
					GAME_OVER = true;
					MessageBox(hButton, "You have discovered mine!", "GAME OVER", MB_OK);
				}
			}
				break;
			case 1:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[1]);
				break;
			case 2:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[2]);
				break;
			case 3:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[3]);
				break;
			case 4:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[4]);
				break;
			case 5:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[5]);
				break;
			case 6:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[6]);
				break;
			case 7:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[7]);
				break;
			case 8:
				SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[8]);
				break;

			default:
				break;
			}
			
		}

			return TRUE;

		//When Right Button is released
	case WM_RBUTTONUP:

		// Mouse button right-click event handler
		//char b[32];
		/*_itoa_s(dwRefData, b, 10);
		MessageBox(hButton, b, "Subclass Example - WM_RBUTTONUP", MB_OK);*/
		g_b_X = dwRefData / 10;
		g_b_Y = dwRefData % 10;

		if (g_b_gameboard->fields[g_b_X][g_b_Y].flagged == false && g_b_gameboard->fields[g_b_X][g_b_Y].discovered == false)
		{
			SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[0]); // set the flag Bitmap when first click
			g_b_gameboard->fields[g_b_X][g_b_Y].flagged = true;
		}
		else if (g_b_gameboard->fields[g_b_X][g_b_Y].flagged == true && GAME_OVER==false)
		{
			SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, NULL);
			g_b_gameboard->fields[g_b_X][g_b_Y].flagged = false;
		}
		return TRUE;

		// Stop the default message handler.
		return TRUE;

	case WM_NCDESTROY:

		// You must remove your window subclass before the window being 
		// subclassed is destroyed. This is typically done either by removing 
		// the subclass once your temporary need has passed, or if you are 
		// installing a permanent subclass, by inserting a call to 
		// RemoveWindowSubclass inside the subclass procedure itself:
		RemoveWindowSubclass(hButton, NewSafeBtnProc, uIdSubclass);
		return DefSubclassProc(hButton, message, wParam, lParam);

	default:
		return DefSubclassProc(hButton, message, wParam, lParam);
	}
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
			hwnd_matrix[i][j] = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_NOTIFY | BS_PUSHBUTTON,
				12 + 16 * j, 36 + 16 * i, 16, 16, hwnd, (HMENU)(10 * i + j), hInstance, 0);
		}
	}
	return hwnd_matrix;
}
void load_BITMAP()
{
	int n;
	std::string name = "";
	char help_bufor[3];
	for (int i = 0; i < 16; i++)
	{
		_itoa_s(i, help_bufor, 10);
		name = "BMPs\\" + std::string(help_bufor) + ".bmp";
		hbit_BMPs[i] = (HBITMAP)LoadImage(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	}
}