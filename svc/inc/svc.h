/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   svc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   */
/*   Created: 2025/05/11 13:21:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/11 13:21:38 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <windows.h>

#pragma endregion

#pragma region "Variables"

	extern char						*Name;
	extern char						*Version;
	extern SERVICE_STATUS			g_ServiceStatus;
	extern SERVICE_STATUS_HANDLE	g_StatusHandle;

#pragma endregion

#pragma region "Methods"

	// Service Control
	int control(int argc, char **argv);

	// Funciones específicas del servicio
	void ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
	void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv);
	VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl);

#pragma endregion
