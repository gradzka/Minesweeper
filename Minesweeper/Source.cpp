#include <windows.h>
#include <string>
#include "game_board.h"
#include "Resource.h"
#include <fstream>
#include <cmath>
#include "HighScore.h"

#define horizontal_coordinates (horizontal / 2 -  get_window_width() / 2)
#define vertical_coordinates (vertical / 2 - get_window_height() / 2)

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HighScoresProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CustomProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewScoreProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NewSafeBtnProc(HWND hButton, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

int get_window_width();
int get_window_height();
void GetDesktopResolution(int &horizontal, int &vertical);
HWND **get_hwnd_matrix(HWND hwnd, HINSTANCE hInstance);
void load_BITMAPS_and_ICON(std::string skin);
void check_neighbours(DWORD_PTR dwRefData);
void neighbour_value(DWORD_PTR dwRefData, int g_b_X, int g_b_Y);
void uncheck_menu(UINT menu_arg_1, UINT menu_arg_2, UINT menu_arg_3, std::string menu_level_1, std::string menu_level_2, std::string menu_level_3);
void HWND_matrix_and_subclassing();
void delete_buttons(int old_rows, int old_buttons);
void clear_old_window_change_its_pos_and_dim(int old_rows, int old_buttons);
void unpressed_clear_button_normal_face();
void play_again_or_change_level(std::string again_or_level, std::string level = "", int rows = 0, int columns = 0, int mines = 0);
void check_if_win();
void check_and_save_HighScores(int your_time, std::string level);
void change_skins(std::string skin);

#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

HBITMAP hbit_BMPs[18];

game_board *g_b_gameboard;
HighScore *high_score;

HWND hwnd;

HWND **hwnd_matrix;
HINSTANCE hInstance;

HBITMAP BitMap; //handle to BitMap
BITMAP Bitmap_Info; //Structure with informaton about bitmap

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));
	g_b_gameboard = new game_board("Beginner");
	high_score = new HighScore();

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
	wc.lpszClassName = "Minesweeper";
	wc.hIconSm = 0;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "wc structure variable wasn't registered properly!", "ERROR",
			MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(1000));
	MSG Communique;

	//screen sizes, GetDesktopResolution will update them
	int horizontal = 0;
	int vertical = 0;

	GetDesktopResolution(horizontal, vertical);

	// Creating window
	hwnd = CreateWindowEx(WS_EX_WINDOWEDGE,
		"Minesweeper", "Minesweeper",
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

	HWND hwnd_smile = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP,
		get_window_width() / 2 - 22, 5, 26, 26, hwnd, (HMENU)-1, hInstance, 0);

	load_BITMAPS_and_ICON("Classic");
	SendMessage(hwnd_smile, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);

	HWND_matrix_and_subclassing();
	unpressed_clear_button_normal_face();

	high_score->load_HighScores();

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
	HDC hdc;
	HDC hdcBitmap; //zmienne na 2 konteksty
	PAINTSTRUCT ps;
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

	case WM_TIMER:
	{
		if (high_score->get_TIMER() < 999)
		{
			high_score->add_one_to_TIMER();
			InvalidateRect(hwnd, NULL, FALSE);
		}
	}
	break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		hdcBitmap = CreateCompatibleDC(hdc);

		BitMap = (HBITMAP)SelectObject(hdcBitmap, BitMap);
		BitBlt(hdc, get_window_width() - 69, 6, 15, 23, hdcBitmap, 17 * ((high_score->get_TIMER() / 100) % 10) + 1, 0, SRCCOPY);//Bitmap_Info.bmWidth, Bitmap_Info.bmHeight
		BitBlt(hdc, get_window_width() - 54, 6, 15, 23, hdcBitmap, 17 * ((high_score->get_TIMER() / 10) % 10) + 1, 0, SRCCOPY);
		BitBlt(hdc, get_window_width() - 39, 6, 15, 23, hdcBitmap, 17 * (high_score->get_TIMER() % 10) + 1, 0, SRCCOPY);

		//if (GAME_STARTED == true)
		//{
		if (g_b_gameboard->no_flagged_mines_number >= 0)
		{
			BitBlt(hdc, 7, 6, 15, 23, hdcBitmap, 1, 0, SRCCOPY);
		}
		else
		{
			BitBlt(hdc, 7, 6, 15, 23, hdcBitmap, 17 * 10 + 1, 0, SRCCOPY);
		}
		BitBlt(hdc, 22, 6, 15, 23, hdcBitmap, abs(17 * ((g_b_gameboard->no_flagged_mines_number / 10) % 10)) + 1, 0, SRCCOPY);
		BitBlt(hdc, 37, 6, 15, 23, hdcBitmap, abs(17 * (g_b_gameboard->no_flagged_mines_number % 10)) + 1, 0, SRCCOPY);
		//GAME_STARTED = false;
		//}

		//if (FLAG_CLICKED == true){
		//if (g_b_gameboard->no_flagged_mines_number < 0)
		//{
		//BitBlt(hdc, 0, 8, 20, 20, hdcBitmap, 22 * 10 + 2, 0, SRCCOPY);
		//}
		//BitBlt(hdc, 18, 8, 20, 20, hdcBitmap, abs(22 * ((g_b_gameboard->no_flagged_mines_number / 10) % 10)) + 2, 0, SRCCOPY);
		//BitBlt(hdc, 36, 8, 20, 20, hdcBitmap, abs(22 * (g_b_gameboard->no_flagged_mines_number % 10)) + 2, 0, SRCCOPY);
		//FLAG_CLICKED = false;
		//}

		BitMap = (HBITMAP)SelectObject(hdcBitmap, BitMap);
		DeleteDC(hdcBitmap);
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
		if (g_b_gameboard->get_END_OF_GAME() == false)
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[14]);
		}
		break;

	case WM_LBUTTONUP:
		if (g_b_gameboard->get_END_OF_GAME() == false)
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		delete g_b_gameboard;
		delete high_score;
		KillTimer(hwnd, ID_TIMER);
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
			play_again_or_change_level("level", "Beginner");
			break;
		}
		case 1003: //Intermediate
		{
			uncheck_menu(1002, 1004, 1005, "Beginner", "Expert", "Custom");
			CheckMenuItem(GetMenu(hwnd), 1003, MF_BYCOMMAND | MF_CHECKED);
			play_again_or_change_level("level", "Intermediate");
			break;
		}
		case 1004: //Expert
		{
			uncheck_menu(1002, 1003, 1005, "Beginner", "Intermediate", "Custom");
			CheckMenuItem(GetMenu(hwnd), 1004, MF_BYCOMMAND | MF_CHECKED);
			play_again_or_change_level("level", "Expert");
			break;
		}
		case 1005: //Custom
		{
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_CUSTOM),
				hwnd, reinterpret_cast<DLGPROC>(CustomProc));
			break;
		}
		case 1006: //High Scores
		{
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_HIGHSCORES),
				hwnd, reinterpret_cast<DLGPROC>(HighScoresProc));
			break;
		}
		case 1007: //Exit
		{
			DestroyWindow(hwnd);
			break;
		}
		case 1008: //About
		{
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_ABOUT),
				hwnd, reinterpret_cast<DLGPROC>(AboutProc));
			break;
		}
		//Skins
		case 1050: //Classic Theme
		{
			CheckMenuItem(GetMenu(hwnd), 1051, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetMenu(hwnd), 1052, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetMenu(hwnd), 1050, MF_BYCOMMAND | MF_CHECKED);
			//InvalidateRect(hwnd, NULL, FALSE);
			change_skins("Classic");
			break;
		}
		case 1051: //Crash Bandicoot
		{
			CheckMenuItem(GetMenu(hwnd), 1050, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetMenu(hwnd), 1052, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetMenu(hwnd), 1051, MF_BYCOMMAND | MF_CHECKED);
			//load_BITMAPS_and_ICON("Crash");
			change_skins("Crash");
			break;
		}
		case 1052: //Super Mario
		{
			CheckMenuItem(GetMenu(hwnd), 1050, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetMenu(hwnd), 1051, MF_BYCOMMAND | MF_UNCHECKED);
			CheckMenuItem(GetMenu(hwnd), 1052, MF_BYCOMMAND | MF_CHECKED);
			change_skins("Mario");
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
LRESULT CALLBACK HighScoresProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	int ID_CTRL = 1016;

	switch (Msg)
	{
	case WM_INITDIALOG:
		char buffer_for_highscores[6];
		for (int i = 0; i < 9; i++)
		{
			SetDlgItemText(hWndDlg, ID_CTRL, high_score->get_i_HighScores_name(i));
			ID_CTRL += 1;
			_itoa_s(high_score->get_i_HighScores_time(i), buffer_for_highscores, 10);
			if (high_score->get_i_HighScores_time(i) < 10)
			{
				buffer_for_highscores[1] = ' ';
				buffer_for_highscores[2] = 's';
				buffer_for_highscores[3] = '\0';
			}
			else if (high_score->get_i_HighScores_time(i) < 100)
			{
				buffer_for_highscores[2] = ' ';
				buffer_for_highscores[3] = 's';
				buffer_for_highscores[4] = '\0';
			}
			else
			{
				buffer_for_highscores[3] = ' ';
				buffer_for_highscores[4] = 's';
				buffer_for_highscores[5] = '\0';
			}
			SetDlgItemText(hWndDlg, ID_CTRL, buffer_for_highscores);
			ID_CTRL += 1;
		}

		return TRUE;

	case WM_COMMAND:
	{
		// reakcja na przyciski
		switch (LOWORD(wParam))
		{
		case ID_OK:
		{
			EndDialog(hWndDlg, NULL);
			break;
		}
		case ID_RESET:
		{
			high_score->reset_HighScores();
			char buffer_for_highscores[6];
			for (int i = 0; i < 9; i++)
			{
				SetDlgItemText(hWndDlg, ID_CTRL, high_score->get_i_HighScores_name(i));
				ID_CTRL += 1;
				_itoa_s(high_score->get_i_HighScores_time(i), buffer_for_highscores, 10);
				buffer_for_highscores[3] = ' ';
				buffer_for_highscores[4] = 's';
				buffer_for_highscores[5] = '\0';
				SetDlgItemText(hWndDlg, ID_CTRL, buffer_for_highscores);
				ID_CTRL += 1;
			}
			break;
		}
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(hWndDlg, NULL); //NULL - value to return
		break;
	}
	break;

	default: return FALSE;
	}

	return FALSE;
}
LRESULT CALLBACK AboutProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
	{
		// reakcja na przyciski
		switch (LOWORD(wParam))
		{
		case ID_OK:
		{
			EndDialog(hWndDlg, NULL);
			break;
		}
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(hWndDlg, NULL); //NULL - value to return
		break;
	}
	break;

	default: return FALSE;
	}

	return FALSE;
}
LRESULT CALLBACK CustomProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hWndDlg, IDC_ROWS_T), "9");
		SetWindowText(GetDlgItem(hWndDlg, IDC_COLUMNS_T), "9");
		SetWindowText(GetDlgItem(hWndDlg, IDC_MINES_T), "10");
		return TRUE;

	case WM_COMMAND:
	{
		// reakcja na przyciski
		switch (LOWORD(wParam))
		{
		case ID_OK:
		{
			uncheck_menu(1002, 1003, 1004, "Beginner", "Intermediate", "Expert");
			CheckMenuItem(GetMenu(hwnd), 1005, MF_BYCOMMAND | MF_CHECKED);
			TCHAR rows_buffer[7];
			TCHAR columns_buffer[7];
			TCHAR mines_buffer[7];
			GetWindowText(GetDlgItem(hWndDlg, IDC_ROWS_T), rows_buffer, 7);
			GetWindowText(GetDlgItem(hWndDlg, IDC_COLUMNS_T), columns_buffer, 7);
			GetWindowText(GetDlgItem(hWndDlg, IDC_MINES_T), mines_buffer, 7);
			play_again_or_change_level("level", "Custom", atoi(rows_buffer), atoi(columns_buffer), atoi(mines_buffer));
			EndDialog(hWndDlg, NULL);
			break;
		}
		case ID_CANCEL:
		{
			EndDialog(hWndDlg, NULL);
			break;
		}
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(hWndDlg, NULL); //NULL - value to return
		break;
	}
	break;

	default: return FALSE;
	}

	return FALSE;
}
LRESULT CALLBACK NewScoreProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	std::string congratulations = g_b_gameboard->get_beg_int_exp_cus();
	congratulations[0] = congratulations[0] + 32;

	switch (Msg)
	{
	case WM_INITDIALOG:

		congratulations = "Congratulations!\n\nYou have one of the fastest times for " + congratulations + " level.\nPlease enter your name.";
		SetDlgItemText(hWndDlg, ID_CONGRATULATIONS, congratulations.c_str());
		SetWindowText(GetDlgItem(hWndDlg, IDC_NAME), "Player");
		return TRUE;

	case WM_COMMAND:
	{
		// reakcja na przyciski
		switch (LOWORD(wParam))
		{
		case ID_OK:
		{
			std::string player_name = high_score->get_PlayerName();
			GetWindowText(GetDlgItem(hWndDlg, IDC_NAME), high_score->get_PlayerName(), 17);
			EndDialog(hWndDlg, NULL);
			break;
		}
		}
		break;
	}
	case WM_CLOSE:
	{
		EndDialog(hWndDlg, NULL); //NULL - value to return
		break;
	}
	break;

	default: return FALSE;
	}

	return FALSE;
}
LRESULT CALLBACK NewSafeBtnProc(HWND hButton, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	int g_b_X = 0;
	int g_b_Y = 0;

	switch (message)
	{
	case WM_LBUTTONDOWN:
		if (g_b_gameboard->get_END_OF_GAME() == false)
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[14]);
		}
		return TRUE;

	case WM_LBUTTONUP:
		
		if (g_b_gameboard->get_END_OF_GAME() == true)
		{
			break;
		}
		else
		{
			if (high_score->get_Started_TIMER() == false)
			{
				SetTimer(hwnd, ID_TIMER, 1000, NULL);
				high_score->change_Started_TIMER_true();
			}
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); //change smile

			g_b_X = dwRefData / g_b_gameboard->get_columns();
			g_b_Y = dwRefData % g_b_gameboard->get_columns();

			if (g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == false)
				SendMessage(hButton, BM_SETSTATE, TRUE, NULL);

			/*_itoa_s(dwRefData, b, 10);
			MessageBox(hButton, b, "X", MB_OK);*/
			/*
			_itoa_s(gl_g_b_Y, b, 10);
			MessageBox(hButton, b, "Y", MB_OK);*/


			if (g_b_gameboard->get_fields(g_b_X, g_b_Y).discovered == false && g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == false) //if button wasn't discovered
			{
				g_b_gameboard->get_fields(g_b_X, g_b_Y).discovered = true;
				switch (g_b_gameboard->get_fields(g_b_X, g_b_Y).value)
				{
				case -1:
				{
					if (g_b_gameboard->get_END_OF_GAME() == false)
					{
						//Started_TIMER = false;
						//GAME_STARTED = true;
						//FLAG_CLICKED = false;
						KillTimer(hwnd, ID_TIMER);
						SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[11]);
						g_b_gameboard->get_fields(g_b_X, g_b_Y).last_clicked = true;
						SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[13]);

						for (int i = 0; i < g_b_gameboard->get_rows(); i++)
						{
							for (int j = 0; j < g_b_gameboard->get_columns(); j++)
							{
								if (g_b_gameboard->get_fields(i, j).value == -1 && g_b_gameboard->get_fields(i, j).discovered == false)
								{
									if (g_b_gameboard->get_fields(i, j).flagged == false)
									{
										SendMessage(GetDlgItem(hwnd, g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[9]);
										g_b_gameboard->get_fields(i, j).discovered = true;
									}
								}
								else if (g_b_gameboard->get_fields(i, j).value != -1 && g_b_gameboard->get_fields(i, j).flagged == true)
								{
									SendMessage(GetDlgItem(hwnd, g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[10]);
									g_b_gameboard->get_fields(i, j).discovered = true;
								}

							}
						}
						g_b_gameboard->change_END_OF_GAME(); //END_OF_GAME=truel;
					}
				}
				break;
				case 0:
					g_b_gameboard->get_fields(dwRefData / g_b_gameboard->get_columns(), dwRefData % g_b_gameboard->get_columns()).discovered = true;
					SendMessage(GetDlgItem(hwnd, dwRefData), BM_SETSTATE, TRUE, NULL);
					SendMessage(GetDlgItem(hwnd, dwRefData), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[0]);
					check_neighbours(dwRefData);
					check_if_win();
					break;
				case 1:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[1]);
					check_if_win();
					break;
				case 2:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[2]);
					check_if_win();
					break;
				case 3:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[3]);
					check_if_win();
					break;
				case 4:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[4]);
					check_if_win();
					break;
				case 5:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[5]);
					check_if_win();
					break;
				case 6:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[6]);
					check_if_win();
					break;
				case 7:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[7]);
					check_if_win();
					break;
				case 8:
					SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[8]);
					check_if_win();
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

		//FLAG_CLICKED = true; //flag has been discovered;
		if (g_b_gameboard->get_END_OF_GAME() == true)
		{
			break;
		}

		g_b_X = dwRefData / g_b_gameboard->get_columns();
		g_b_Y = dwRefData % g_b_gameboard->get_columns();


		if (g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == false && g_b_gameboard->get_fields(g_b_X, g_b_Y).discovered == false)
		{
			SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[17]); // set the flag Bitmap when first click
			g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged = true;

			g_b_gameboard->no_flagged_mines_number--;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		else if (g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged == true && g_b_gameboard->get_END_OF_GAME() == false)
		{
			SendMessage(hButton, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[16]);
			g_b_gameboard->get_fields(g_b_X, g_b_Y).flagged = false;

			g_b_gameboard->no_flagged_mines_number++;
			InvalidateRect(hwnd, NULL, FALSE);
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
	width = 18 + 16 * g_b_gameboard->get_columns();
	return width;
}
int get_window_height()
{
	int height = 0;
	height = 96 + 16 * g_b_gameboard->get_rows();
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
void load_BITMAPS_and_ICON(std::string skin)
{
	std::string name = "";
	char help_bufor[3];
	for (int i = 0; i < 18; i++)
	{
		_itoa_s(i, help_bufor, 10);
		name = "BMPs\\" + skin + "\\" + std::string(help_bufor) + ".bmp";
		hbit_BMPs[i] = (HBITMAP)LoadImage(NULL, name.c_str(), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	}
	name = "BMPs\\" + skin + "\\icon.ico";

	HANDLE icon = LoadImage(hInstance, name.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);

	name = "BMPs\\" + skin + "\\Digits.bmp";
	BitMap = (HBITMAP)LoadImage(0, name.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (BitMap == NULL) // if LoadImage fails, it returns a NULL handle
	{
		MessageBox(0, "There's problem with open \"Digits.bmp\"", "No File", MB_ICONERROR);
	}
	GetObject(BitMap, sizeof(BITMAP), &Bitmap_Info);
}
void check_neighbours(DWORD_PTR dwRefData)
{
	int g_b_X = dwRefData / g_b_gameboard->get_columns();
	int g_b_Y = dwRefData % g_b_gameboard->get_columns();

	if ((g_b_X - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y).discovered == false) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y).flagged == false))//up
	{
		g_b_gameboard->get_fields(g_b_X - 1, g_b_Y).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y, g_b_X - 1, g_b_Y);

	}
	if ((g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->get_fields(g_b_X, g_b_Y + 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X, g_b_Y + 1).flagged == false))//right
	{
		g_b_gameboard->get_fields(g_b_X, g_b_Y + 1).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X)*g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X)*g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X, g_b_Y + 1);
	}
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y).discovered == false) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y).flagged == false))//down
	{
		g_b_gameboard->get_fields(g_b_X + 1, g_b_Y).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y, g_b_X + 1, g_b_Y);
	}
	if ((g_b_Y - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X, g_b_Y - 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X, g_b_Y - 1).flagged == false))//left
	{
		g_b_gameboard->get_fields(g_b_X, g_b_Y - 1).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X)*g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X)*g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X, g_b_Y - 1);
	}
	if ((g_b_X - 1 >= 0) && (g_b_Y - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y - 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y - 1).flagged == false)) //upper left diagonally
	{
		g_b_gameboard->get_fields(g_b_X - 1, g_b_Y - 1).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X - 1, g_b_Y - 1);
	}
	if ((g_b_X - 1 >= 0) && (g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y + 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X - 1, g_b_Y + 1).flagged == false)) //upper right diagonally
	{
		g_b_gameboard->get_fields(g_b_X - 1, g_b_Y + 1).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X - 1)*g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X - 1, g_b_Y + 1);
	}
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_Y - 1 >= 0) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y - 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y - 1).flagged == false)) //bottom left diagonally
	{
		g_b_gameboard->get_fields(g_b_X + 1, g_b_Y - 1).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y - 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y - 1, g_b_X + 1, g_b_Y - 1);
	}
	if ((g_b_X + 1 < g_b_gameboard->get_rows()) && (g_b_Y + 1 < g_b_gameboard->get_columns()) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y + 1).discovered == false) && (g_b_gameboard->get_fields(g_b_X + 1, g_b_Y + 1).flagged == false)) //bottom right diagonally
	{
		g_b_gameboard->get_fields(g_b_X + 1, g_b_Y + 1).discovered = true;
		SendMessage(GetDlgItem(hwnd, (g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y + 1), BM_SETSTATE, TRUE, NULL);
		neighbour_value((g_b_X + 1)*g_b_gameboard->get_columns() + g_b_Y + 1, g_b_X + 1, g_b_Y + 1);
	}
}
void neighbour_value(DWORD_PTR dwRefData, int g_b_X, int g_b_Y)
{
	switch (g_b_gameboard->get_fields(g_b_X, g_b_Y).value)
	{
	case 0:
		SendMessage(GetDlgItem(hwnd, dwRefData), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[0]);
		check_neighbours(dwRefData);
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
	int i = 0;
	int j = 0;
	for (i = 0; i < old_rows; i++)
	{
		for (j = 0; j < old_columns; j++)
		{
			DestroyWindow(hwnd_matrix[i][j]);
		}
		delete[] hwnd_matrix[i];
	}
	delete[]hwnd_matrix;
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
		SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[16]);
	}
	SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
}
void play_again_or_change_level(std::string again_or_level, std::string level, int rows, int columns, int mines)
{
	//TIMER = 0;
	//Started_TIMER = false;
	//high_score = new HighScore();
	high_score->change_Started_TIMER_false();
	high_score->change_TIMER_false();
	KillTimer(hwnd, ID_TIMER);
	//GAME_STARTED = true;
	//FLAG_CLICKED = false;

	if (again_or_level.compare("again") == 0)
	{
		InvalidateRect(hwnd, NULL, FALSE);

		unpressed_clear_button_normal_face();
		g_b_gameboard = g_b_gameboard->get_beg_int_exp_cus().compare("Custom") == 0 ?
			new game_board(g_b_gameboard->get_rows(), g_b_gameboard->get_columns(), g_b_gameboard->get_mines_number()) :
			new game_board(g_b_gameboard->get_beg_int_exp_cus());
	}
	else if (again_or_level.compare("level") == 0)
	{
		RECT *help_RECT_struct = new RECT;

		help_RECT_struct->left = get_window_width() - 73;
		help_RECT_struct->right = get_window_width() - 17;
		help_RECT_struct->top = 8;
		help_RECT_struct->bottom = 28;

		int old_rows = 0;
		int old_columns = 0;
		old_rows = g_b_gameboard->get_rows();
		old_columns = g_b_gameboard->get_columns();

		g_b_gameboard = level.compare("Custom") == 0 ?
			new game_board(rows, columns, mines) :
			new game_board(level);

		InvalidateRect(hwnd, help_RECT_struct, TRUE);
		InvalidateRect(hwnd, NULL, FALSE);

		clear_old_window_change_its_pos_and_dim(old_rows, old_columns);

		int i = 0;
		for (i = 0; i < g_b_gameboard->get_rows()*g_b_gameboard->get_columns(); i++)
		{
			SendMessage(GetDlgItem(hwnd, i), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[16]); //box
		}

		SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]); //normal face
	}
	//END_OF_GAME = false;
}
void check_if_win()
{
	int i = 0;
	int j = 0;

	for (i = 0; i < g_b_gameboard->get_rows(); i++)
	{
		for (j = 0; j < g_b_gameboard->get_columns(); j++)
		{
			if (g_b_gameboard->get_fields(i, j).value != -1 && g_b_gameboard->get_fields(i, j).discovered != true)
			{
				return;
			}
		}
	}
	//Started_TIMER = false;
	//GAME_STARTED = true;
	//FLAG_CLICKED = false;
	KillTimer(hwnd, ID_TIMER);
	g_b_gameboard->change_END_OF_GAME();

	for (i = 0; i < g_b_gameboard->get_rows(); i++)
	{
		for (j = 0; j < g_b_gameboard->get_columns(); j++)
		{
			if (g_b_gameboard->get_fields(i, j).value == -1 && g_b_gameboard->get_fields(i, j).discovered == false && g_b_gameboard->get_fields(i, j).flagged == false)
			{
				SendMessage(GetDlgItem(hwnd, g_b_gameboard->get_columns() * i + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[17]);
			}
		}
	}
	SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[15]);
	g_b_gameboard->change_victory();
	check_and_save_HighScores(high_score->get_TIMER(), g_b_gameboard->get_beg_int_exp_cus());
}
void check_and_save_HighScores(int your_time, std::string level)
{
	if (level.compare("Custom") == 0)
	{
		return;
	}
	int index;
	index = 0;
	if (level.compare("Beginner") == 0)
	{
		index = 0;
	}
	else if (level.compare("Intermediate") == 0)
	{
		index = 3;
	}
	else if (level.compare("Expert") == 0)
	{
		index = 6;
	}

	if (your_time < high_score->get_i_HighScores_time(index))
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_NEW_SCORE),
			hwnd, reinterpret_cast<DLGPROC>(NewScoreProc));
		high_score->change_i_HighScores_name(index + 2, index + 1);
		high_score->change_i_HighScores_time(index + 2, index + 1);

		high_score->change_i_HighScores_name(index + 1, index);
		high_score->change_i_HighScores_time(index + 1, index);

		high_score->change_i_HighScores_name(index, high_score->get_PlayerName());
		high_score->change_i_HighScores_time_2(index, your_time);
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_HIGHSCORES),
			hwnd, reinterpret_cast<DLGPROC>(HighScoresProc));
	}
	else if (your_time < high_score->get_i_HighScores_time(index + 1))
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_NEW_SCORE),
			hwnd, reinterpret_cast<DLGPROC>(NewScoreProc));
		high_score->change_i_HighScores_name(index + 2, index + 1);
		high_score->change_i_HighScores_time(index + 2, index + 1);

		high_score->change_i_HighScores_name(index+1, high_score->get_PlayerName());
		high_score->change_i_HighScores_time_2(index+1, your_time);
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_HIGHSCORES),
			hwnd, reinterpret_cast<DLGPROC>(HighScoresProc));
	}
	else if (your_time < high_score->get_i_HighScores_time(index + 2))
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_NEW_SCORE),
			hwnd, reinterpret_cast<DLGPROC>(NewScoreProc));
		high_score->change_i_HighScores_name(index+2, high_score->get_PlayerName());
		high_score->change_i_HighScores_time_2(index + 2, your_time);
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DLG_HIGHSCORES),
			hwnd, reinterpret_cast<DLGPROC>(HighScoresProc));
	}

	std::fstream file;
	file.open("HighScores.txt", std::ios::out | std::ios::trunc);
	for (int i = 0; i < 9; i++)
	{
		if (i == 0)
		{
			file << "Beginner\n";
		}
		if (i == 3)
		{
			file << "\nIntermediate\n";
		}
		if (i == 6)
		{
			file << "\nExpert\n";
		}
		file << high_score->get_i_HighScores_name(i);
		file << " ";
		file << high_score->get_i_HighScores_time(i);
		file << "\n";
	}
	file.close();
}
void change_skins(std::string skin)
{
	load_BITMAPS_and_ICON(skin);
	if (g_b_gameboard->get_END_OF_GAME() == false)
	{
		SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[12]);
	}
	else
	{
		if (g_b_gameboard->get_victory() == true)
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[15]);
		}
		else
		{
			SendMessage(GetDlgItem(hwnd, -1), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[13]);
		}
	}
	for (int i = 0; i < g_b_gameboard->get_rows(); i++)
	{
		for (int j = 0; j < g_b_gameboard->get_columns(); j++)
		{
			if (g_b_gameboard->get_fields(i, j).flagged == true)
			{
				if (g_b_gameboard->get_fields(i, j).value + 1 != 0)
				{
					SendMessage(GetDlgItem(hwnd, i*g_b_gameboard->get_columns() + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[10]);
				}
				else
				{
					SendMessage(GetDlgItem(hwnd, i*g_b_gameboard->get_columns() + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[17]);
				}
			}
			else if (g_b_gameboard->get_fields(i, j).discovered == false)
			{
				SendMessage(GetDlgItem(hwnd, i*g_b_gameboard->get_columns() + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[16]);
			}
			else //discovered
			{
				if (g_b_gameboard->get_fields(i, j).value + 1 == 0)
				{
					if (g_b_gameboard->get_fields(i, j).last_clicked == true)
					{
						SendMessage(GetDlgItem(hwnd, i*g_b_gameboard->get_columns() + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[11]);
					}
					else
					{
						SendMessage(GetDlgItem(hwnd, i*g_b_gameboard->get_columns() + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[9]);
					}
				}
				else
				{
					SendMessage(GetDlgItem(hwnd, i*g_b_gameboard->get_columns() + j), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbit_BMPs[g_b_gameboard->get_fields(i, j).value]);
				}
			}
		}
	}
	InvalidateRect(hwnd, NULL, FALSE);
}
