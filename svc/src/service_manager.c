/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service_manager.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 13:31:13 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/12 17:29:40 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "svc.h"

#pragma endregion

#pragma region "Variables"

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
            Sleep(100000);

            // Detener el servicio cuando termine su trabajo
            ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
        }

    #pragma endregion

#pragma endregion
