#include <windows.h>
#include <string>
#include "game_board.h"


#define horizontal_coordinates (horizontal / 2 -  get_window_width() / 2)
#define vertical_coordinates (vertical / 2 - get_window_height() / 2)

HBITMAP hbit_BMPs[16];

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewSafeBtnProc(HWND hButton, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

int get_window_width();
int get_window_height();
void GetDesktopResolution(int &horizontal, int &vertical);
HWND **get_hwnd_matrix(HWND hwnd, HINSTANCE hInstance);
void load_BITMAP();
void check_neigbours(DWORD_PTR dwRefData);
void neighbour_value(DWORD_PTR dwRefData, int g_b_X, int g_b_Y);
void uncheck_menu(UINT menu_arg_1, UINT menu_arg_2, UINT menu_arg_3, std::string menu_level_1, std::string menu_level_2, std::string menu_level_3);
void HWND_matrix_and_subclassing();
void delete_buttons(int old_rows, int old_buttons);
void clear_old_window_change_its_pos_and_dim(int old_rows, int old_buttons);
void unpressed_clear_button_normal_face();
void play_again_or_change_level(std::string again_or_level, std::string level = "", int rows = 0, int columns = 0, int mines = 0);


#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

char b[32];

game_board *g_b_gameboard;
int gl_g_b_X = 0;
int gl_g_b_Y = 0;

bool GAME_OVER = false;
HWND hwnd;
HMENU hMenu;
HWND **hwnd_matrix;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPSTR ClassName = "Minesweeper";
	g_b_gameboard = new game_board("Beginner");

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;
	wc.hIconSm = 0;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "wc structure variable wasn't registered properly!", "ERROR",
			MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(1000));

	MSG Communique;

	//screen sizes, GetDesktopResolution will update them
	int horizontal = 0;
	int vertical = 0;

	GetDesktopResolution(horizontal, vertical);

	// Creating window
	hwnd = CreateWindowEx(
		WS_EX_WINDOWEDGE,
		ClassName,
		ClassName,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX,
		horizontal_coordinates,
		vertical_coordinates,
		get_window_width(),
		get_window_height(),
		NULL,
		hMenu,
		hInstance,
		NULL);

	if (hwnd == NULL)
	{
		return 1;
	}

	HANDLE icon = LoadImage(hInstance, "BMPs\\Crash\\icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);

	HWND hwnd_smile = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
		get_window_width() / 2 - 22, 5, 26, 26, hwnd, (HMENU)-1, hInstance, 0);

	load_BITMAP();
	SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
	
	HWND_matrix_and_subclassing();

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
	case WM_KEYDOWN:
	{
		switch ((int)wParam)
		{
		case VK_F2:
			play_again_or_change_level("again");
			break;
		}
	}
	break;

	case WM_LBUTTONDOWN:
		if (GAME_OVER == false)
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[14]);
		}
		break;

	case WM_LBUTTONUP:
		if (GAME_OVER == false)
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		delete g_b_gameboard;
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		//ACTION MENU
		switch (LOWORD(wParam))
		{
		case 1001: //NEW GAME
		{
			play_again_or_change_level("again");
			break;
		}
		case 1002: //Beginner
		{
			uncheck_menu(1003, 1004, 1005, "Intermediate", "Expert", "Custom");
			CheckMenuItem(GetMenu(hwnd), 1002, MF_BYCOMMAND | MF_CHECKED);
			//new_game("Beginner");
			play_again_or_change_level("level", "Beginner");
			break; 
		}
		case 1003: //Intermediate
		{
			uncheck_menu(1002, 1004, 1005, "Beginner", "Expert", "Custom");
			CheckMenuItem(GetMenu(hwnd), 1003, MF_BYCOMMAND | MF_CHECKED);
			//new_game("Intermediate");
			play_again_or_change_level("level", "Intermediate");
			break;
		}
		case 1004: //Expert
		{
			uncheck_menu(1002, 1003, 1005, "Beginner", "Intermediate", "Custom");
			CheckMenuItem(GetMenu(hwnd), 1004, MF_BYCOMMAND | MF_CHECKED);
			//new_game("Expert");
			play_again_or_change_level("level", "Expert");
			break;
		}
		case 1005: //Custom
		{
			uncheck_menu(1002, 1003, 1004, "Beginner", "Intermediate", "Expert");
			CheckMenuItem(GetMenu(hwnd), 1005, MF_BYCOMMAND | MF_CHECKED);
			//new_game(10,17,10); // To improve
			play_again_or_change_level("level", "Custom",10,17,10);
			break;
		}
		case 1006: //High Scores
		{
			break;
		}
		case 1007: //Exit
		{
			PostQuitMessage(0);
			break;
		}
		case 1008: //About
			{
				break;
			}
		}

		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			// see which button was clicked
			if ((HWND)lParam == GetDlgItem(hwnd, -1))
			{
				play_again_or_change_level("again");
				SetFocus(hwnd);
			}
			break;
		}
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
		if (GAME_OVER == false)
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[14]);
		}
		return TRUE;

	case WM_LBUTTONUP:
		if (GAME_OVER == true)
		{
			break;
		}
		else
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); //change smile

			gl_g_b_X = dwRefData / g_b_gameboard->get_columns();
			gl_g_b_Y = dwRefData % g_b_gameboard->get_columns();

			if (g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].flagged == false)
				SendMessage(hButton, BM_SETSTATE, TRUE, NULL);

			/*_itoa_s(dwRefData, b, 10);
			MessageBox(hButton, b, "X", MB_OK);*/
			/*
			_itoa_s(gl_g_b_Y, b, 10);
			MessageBox(hButton, b, "Y", MB_OK);*/
			

			if (g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].discovered == false && g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].flagged == false) //if button wasn't discovered
			{
				g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].discovered = true;
				//dwRefData == 1 &&
				switch (g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].value)
				{
				case -1:
				{
					if (GAME_OVER == false){
						SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[11]);
						SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[13]);

						for (int i = 0; i < g_b_gameboard->get_rows(); i++)
						{
							for (int j = 0; j < g_b_gameboard->get_columns(); j++)
							{
								if (g_b_gameboard->fields[i][j].value == -1 && g_b_gameboard->fields[i][j].discovered == false)
								{
									if (g_b_gameboard->fields[i][j].flagged == false)
									{
										SendMessage(GetDlgItem(hwnd, g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[9]);
									}
								}
								else if (g_b_gameboard->fields[i][j].value != -1 && g_b_gameboard->fields[i][j].flagged == true)
								{
									SendMessage(GetDlgItem(hwnd, g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[10]);
								}

							}
						}
						GAME_OVER = true;
						//MessageBox(hButton, "You have discovered mine!", "GAME OVER", MB_OK);
					}
				}
				break;
				case 0:
					g_b_gameboard->fields[dwRefData / g_b_gameboard->get_columns()][dwRefData % g_b_gameboard->get_columns()].discovered = true;
					SendMessage(GetDlgItem(hwnd, dwRefData), BM_SETSTATE, TRUE, NULL);
					check_neigbours(dwRefData);
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

		}
		return TRUE;

		//When Right Button is released
	case WM_RBUTTONUP:

		// Mouse button right-click event handler
		/*_itoa_s(dwRefData, b, 10);
		MessageBox(hButton, b, "Subclass Example - WM_RBUTTONUP", MB_OK);*/
		gl_g_b_X = dwRefData / g_b_gameboard->get_columns();
		gl_g_b_Y = dwRefData % g_b_gameboard->get_columns();

		if (g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].flagged == false && g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].discovered == false)
		{
			SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[0]); // set the flag Bitmap when first click
			g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].flagged = true;
		}
		else if (g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].flagged == true && GAME_OVER == false)
		{
			SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, NULL);
			g_b_gameboard->fields[gl_g_b_X][gl_g_b_Y].flagged = false;
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
int get_window_width()
{
	int width = 0;
	width=18 + 16 * g_b_gameboard->get_columns();
	return width;
}
int get_window_height()
{
	int height = 0;
	height = 98 + 16 * g_b_gameboard->get_rows();
	return height;
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
	int button_number = 0;
	//HWND matrix
	HWND **hwnd_matrix = new HWND *[g_b_gameboard->get_rows()];

	for (int i = 0; i < g_b_gameboard->get_rows(); i++)
	{
		hwnd_matrix[i] = new HWND[g_b_gameboard->get_columns()];

		for (int j = 0; j < g_b_gameboard->get_columns(); j++)
		{
			hwnd_matrix[i][j] = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_NOTIFY | BS_PUSHBUTTON,
				1 + 16 * j, 35 + 16 * i, 16, 16, hwnd, (HMENU)button_number, hInstance, 0);
			button_number++;
		}
	}
	return hwnd_matrix;
}
void load_BITMAP()
{
	std::string name = "";
	char help_bufor[3];
	for (int i = 0; i < 16; i++)
	{
		_itoa_s(i, help_bufor, 10);
		name = "BMPs\\Crash\\" + std::string(help_bufor) + ".bmp";
		hbit_BMPs[i] = (HBITMAP)LoadImage(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	}
}
void check_neigbours(DWORD_PTR dwRefData)
{
	int g_b_X = dwRefData / g_b_gameboard->get_columns();
	int g_b_Y = dwRefData % g_b_gameboard->get_columns();
	//Do ifów
	if ((g_b_X - 1 >= 0) && (g_b_gameboard->fields[g_b_X - 1][g_b_Y].discovered == false) && (g_b_gameboard->fields[g_b_X - 1][g_b_Y].flagged == false))//up
	{
		g_b_gameboard->fields[g_b_X - 1][g_b_Y].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y, g_b_X - 1, g_b_Y);

	}
	if ((g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->fields[g_b_X][g_b_Y + 1].discovered == false) && (g_b_gameboard->fields[g_b_X][g_b_Y + 1].flagged == false))//right
	{
		g_b_gameboard->fields[g_b_X][g_b_Y + 1].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X)*g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X)*g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X, g_b_Y + 1);
	}
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_gameboard->fields[g_b_X + 1][g_b_Y].discovered == false) && (g_b_gameboard->fields[g_b_X + 1][g_b_Y].flagged == false))//down
	{
		g_b_gameboard->fields[g_b_X + 1][g_b_Y].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y, g_b_X + 1, g_b_Y);
	}
	if ((g_b_Y - 1 >= 0) && (g_b_gameboard->fields[g_b_X][g_b_Y - 1].discovered == false) && (g_b_gameboard->fields[g_b_X][g_b_Y - 1].flagged == false))//left
	{
		g_b_gameboard->fields[g_b_X][g_b_Y - 1].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X)*g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X)*g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X, g_b_Y - 1);
	}
	if ((g_b_X - 1 >= 0) && (g_b_Y - 1 >= 0) && (g_b_gameboard->fields[g_b_X - 1][g_b_Y - 1].discovered == false) && (g_b_gameboard->fields[g_b_X - 1][g_b_Y - 1].flagged == false)) //upper left diagonally
	{
		g_b_gameboard->fields[g_b_X - 1][g_b_Y - 1].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X - 1, g_b_Y - 1);
	}
	if ((g_b_X - 1 >= 0) && (g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->fields[g_b_X - 1][g_b_Y + 1].discovered == false) && (g_b_gameboard->fields[g_b_X - 1][g_b_Y + 1].flagged == false)) //upper right diagonally
	{
		g_b_gameboard->fields[g_b_X - 1][g_b_Y + 1].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X - 1, g_b_Y + 1);
	}
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_Y - 1 >= 0) && (g_b_gameboard->fields[g_b_X + 1][g_b_Y - 1].discovered == false) && (g_b_gameboard->fields[g_b_X + 1][g_b_Y - 1].flagged == false)) //bottom left diagonally
	{
		g_b_gameboard->fields[g_b_X + 1][g_b_Y - 1].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X + 1, g_b_Y - 1);
	}
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->fields[g_b_X + 1][g_b_Y + 1].discovered == false) && (g_b_gameboard->fields[g_b_X + 1][g_b_Y + 1].flagged == false)) //bottom right diagonally
	{
		g_b_gameboard->fields[g_b_X + 1][g_b_Y + 1].discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X + 1, g_b_Y + 1);
	}
}
void neighbour_value(DWORD_PTR dwRefData, int g_b_X, int g_b_Y)
{
	switch (g_b_gameboard->fields[g_b_X][g_b_Y].value)
	{
	case 0:
		check_neigbours(dwRefData);
		break;
	case 1:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[1]);
		break;
	case 2:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[2]);
		break;
	case 3:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[3]);
		break;
	case 4:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[4]);
		break;
	case 5:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[5]);
		break;
	case 6:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[6]);
		break;
	case 7:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[7]);
		break;
	case 8:
		SendMessage(GetDlgItem(hwnd, (dwRefData)), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[8]);
		break;

	default:
		break;
	}
}
void uncheck_menu(UINT menu_arg_1, UINT menu_arg_2, UINT menu_arg_3, std::string menu_level_1, std::string menu_level_2, std::string menu_level_3)
{
	if (g_b_gameboard->get_beg_int_exp_cus().compare(menu_level_1) == 0)
	{
		CheckMenuItem(GetMenu(hwnd), menu_arg_1, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if (g_b_gameboard->get_beg_int_exp_cus().compare(menu_level_2) == 0)
	{
		CheckMenuItem(GetMenu(hwnd), menu_arg_2, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else if (g_b_gameboard->get_beg_int_exp_cus().compare(menu_level_3) == 0)
	{
		CheckMenuItem(GetMenu(hwnd), menu_arg_3, MF_BYCOMMAND | MF_UNCHECKED);
	}
}
void HWND_matrix_and_subclassing()
{
	//HWND matrix
	hwnd_matrix = get_hwnd_matrix(hwnd, (HINSTANCE)GetModuleHandle(NULL));

	// Get the handle of the control to be subclassed, and subclass it.
	for (int i = 0; i < g_b_gameboard->get_rows()*g_b_gameboard->get_columns(); i++)
	{
		if (!SetWindowSubclass(GetDlgItem(hwnd, i), NewSafeBtnProc, 0, i))
		{
			DestroyWindow(hwnd);
		}
	}
}
void delete_buttons(int old_rows, int old_columns)
{
	for (int i = 0; i < old_rows; i++)
	{
		for (int j = 0; j < old_columns; j++)
		{
			DestroyWindow(hwnd_matrix[i][j]);
		}
		delete[] hwnd_matrix[i];
	}
	delete []hwnd_matrix;
}
void clear_old_window_change_its_pos_and_dim(int old_rows, int old_columns)
{
	int horizontal = 0;
	int vertical = 0;

	GetDesktopResolution(horizontal, vertical);

	delete_buttons(old_rows, old_columns);

	HWND_matrix_and_subclassing();

	MoveWindow(GetDlgItem(hwnd, -1), get_window_width() / 2 - 22, 5, 26, 26, TRUE);

	SetWindowPos(hwnd,
		HWND_TOP,
		horizontal_coordinates,
		vertical_coordinates,
		get_window_width(), get_window_height(),
		SWP_SHOWWINDOW);
}
void unpressed_clear_button_normal_face()
{
	for (int i = 0; i < g_b_gameboard->get_rows()*g_b_gameboard->get_columns(); i++)
	{
		SendMessage(GetDlgItem(hwnd, i), BM_SETSTATE, FALSE, NULL);
		SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, NULL);
	}
	SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
}
void play_again_or_change_level(std::string again_or_level,std::string level, int rows, int columns, int mines)
{
	if (again_or_level.compare("again")==0)
	{ 
		unpressed_clear_button_normal_face();
		g_b_gameboard = g_b_gameboard->get_beg_int_exp_cus().compare("Custom") == 0 ?
			new game_board(g_b_gameboard->get_rows(), g_b_gameboard->get_columns(), g_b_gameboard->get_mines_number()) :
			new game_board(g_b_gameboard->get_beg_int_exp_cus());
	}
	else if (again_or_level.compare("level")==0)
	{
		int old_rows = 0;
		int old_columns = 0;
		old_rows = g_b_gameboard->get_rows();
		old_columns = g_b_gameboard->get_columns();

		g_b_gameboard = level.compare("Custom") == 0 ?
			new game_board(rows, columns, mines) :
			new game_board(level);

		clear_old_window_change_its_pos_and_dim(old_rows, old_columns);

		SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
	}
	GAME_OVER = false;
}