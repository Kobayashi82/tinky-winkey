/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/14 17:04:01 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "winkey.h"

	#include <stdio.h>
	#include <windows.h>

#pragma endregion

#pragma region "Variables"

	char g_LogPath[MAX_PATH];

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



		// Abrir un archivo para escribir el resultado
		FILE *file = NULL;
		if (!fopen_s(&file, "D:\\winkey_status.txt", "w")) {
			// Obtener la hora actual
			SYSTEMTIME st; GetLocalTime(&st);
			fprintf(file, "Fecha: %02d/%02d/%04d %02d:%02d:%02d\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);

			// Información del sistema
			TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
			DWORD size = sizeof(computerName) / sizeof(computerName[0]);
			if (GetComputerName(computerName, &size)) fprintf(file, "Equipo: %s\n", computerName);
			TCHAR userName[MAX_PATH];
			size = sizeof(userName) / sizeof(userName[0]);
			if (GetUserName(userName, &size)) fprintf(file, "Usuario: %s\n", userName);
			fclose(file);
		}

		// Esperar al evento de cierre
		HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, "Global\\WinkeyTerminateEvent");
		while(TRUE) {
			if (WaitForSingleObject(hEvent, 100) == WAIT_OBJECT_0) {
				if (!fopen_s(&file, "D:\\winkey_exit.txt", "w")) {
					fprintf(file, "Estado: saliendo\n");
					fclose(file);
				} break;
			}
		}



		return (0);
	}

#pragma endregion
