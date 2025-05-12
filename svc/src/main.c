/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/12 14:57:46 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

    #include "svc.h"

    #include <stdio.h>
	#include <windows.h>

#pragma endregion

#pragma region "Variables"

    char                    *Name = "Tinky";
    char                    *Version = "1.0.0";
    SERVICE_STATUS          g_ServiceStatus;
    SERVICE_STATUS_HANDLE   g_StatusHandle;

#pragma endregion

#pragma region "Methods"

    #pragma region "ReportStatusToSCMgr"

        // Función que se ejecuta cuando se inicia el servicio
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
                    ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
                    
                    // Aquí iría la lógica para detener limpiamente el servicio
                    
                    ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
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

            // Mostrar los argumentos recibidos (para fines de depuración)
            printf("Servicio iniciado con %d argumentos:\n", (int)dwArgc);
            for (DWORD i = 0; i < dwArgc; i++) {
                printf("Arg %d: %s\n", (int)i, lpszArgv[i]);
            }

            // Reportar estado de "Arrancando"
            ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);

            // Aquí se podría iniciar el proceso winkey.exe o la funcionalidad principal del servicio
            // Por ahora, solo se duerme 10 segundos para demostración
            Sleep(10000);

            // Detener el servicio cuando termine su trabajo
            ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
        }

    #pragma endregion


	#pragma region "isService"

		int isService(void) {
			SERVICE_TABLE_ENTRY serviceTable[] = {
				{ Name, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
				{ NULL, NULL }
			};

			// Si tiene éxito, la función no regresará hasta que el servicio se detenga
			if (StartServiceCtrlDispatcher(serviceTable)) {
				return (1);
			}

			if (GetLastError() != ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
				return (printf("\nError al iniciar el servicio: %ld\n", GetLastError()), 2);
			return (0);
		}

	#pragma endregion

	#pragma region "isAdmin"

		int isAdmin(void) {
			BOOL isElevated = FALSE;
			HANDLE hToken = NULL;
			
			if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
				TOKEN_ELEVATION elevation;
				DWORD dwSize;

				if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) isElevated = elevation.TokenIsElevated;
				CloseHandle(hToken);
			}

			return (isElevated);
		}

	#pragma endregion

	#pragma region "Main"

		int main(int argc, char **argv) {
			int exit_code = 0;

			if ((exit_code = isService())) return (exit_code - 1);
			if (!isAdmin()) return (printf("\nSe requieren permisos de administrador\n"), 1);

			return (control(argc, argv));
		}

    #pragma endregion

#pragma endregion
