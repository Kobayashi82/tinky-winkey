/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   impersonation.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 13:31:21 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/13 13:06:42 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "svc.h"

#pragma endregion

#include <tlhelp32.h>

DWORD GetWinlogonPID() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return (0);

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return (0);
    }

    DWORD winlogonPID = 0;
    do { if (_stricmp(pe32.szExeFile, "winlogon.exe") == 0) { winlogonPID = pe32.th32ProcessID; break; }}
    while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return (winlogonPID);
}

BOOL ImpersonateSystemToken() {
    HANDLE hProcess = NULL;
    HANDLE hToken = NULL;
    HANDLE hDupToken = NULL;
    BOOL result = FALSE;
    DWORD winlogonPID = GetWinlogonPID();

    if (winlogonPID == 0) return (FALSE);

    // Abrir el proceso winlogon.exe
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, winlogonPID);
    if (!hProcess) return (FALSE);

    // Obtener el token del proceso
    if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken)) {
        CloseHandle(hProcess);
        return (FALSE);
    }

    // Duplicar el token con los permisos necesarios usando DuplicateTokenEx (función requerida)
    if (!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityImpersonation, TokenPrimary, &hDupToken)) {
        CloseHandle(hToken);
        CloseHandle(hProcess);
        return (FALSE);
    }

    result = SetThreadToken(NULL, hDupToken);

    // Limpieza
    if (hDupToken) CloseHandle(hDupToken);
    if (hToken) CloseHandle(hToken);
    if (hProcess) CloseHandle(hProcess);

    return (result);
}
