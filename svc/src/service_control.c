/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service_control.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 13:31:13 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/12 18:02:35 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "svc.h"

	#include <stdio.h>
	#include <string.h>

#pragma endregion

#pragma region "Actions"

	#pragma region "Help"

		int help(char *exe) {		
			printf("Usage: %s [command] [options]\n\n", exe);
			printf("Available commands:\n\n");
			printf("  install                    - Install the service\n");
			printf("  delete                     - Delete the service\n");
			printf("\n");
			printf("  start                      - Start the service\n");
			printf("  stop                       - Stop the service\n");
			printf("\n");
			printf("  config start auto          - Configure automatic startup\n");
			printf("  config start delayed-auto  - Configure delayed automatic startup\n");
			printf("  config start manual        - Configure manual startup\n");
			printf("  config start disabled      - Disable the service\n");
			printf("\n");
			printf("  status                     - Show service status\n");
			printf("  version                    - Show service version\n");
			printf("  help                       - Show this help\n");

			return (0);
		}

	#pragma endregion

	#pragma region "Install"

		int install(void) {
			SC_HANDLE hSCManager = NULL;
			SC_HANDLE hService = NULL;
		    char szPath[MAX_PATH];

			// Abrir el Service Control Manager
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (hSCManager == NULL) {
				printf("[!] Error opening Service Control Manager: %ld\n", GetLastError());
				return (1);
			}

			// Verificar si el servicio ya existe
			hService = OpenService(hSCManager, Name, SERVICE_ALL_ACCESS);
			if (hService != NULL) {
				printf("[!] Service '%s' already exists\n", Name);
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return (1);
			}

		    // Obtener la ruta completa del ejecutable actual
			if (GetModuleFileName(NULL, szPath, MAX_PATH) == 0) {
				printf("[!] Error obtaining executable path: %ld\n", GetLastError());
				return (1);
			}

			// Crear el servicio
			hService = CreateService(
				hSCManager,						// SCM database
				Name,							// Name of service
				Name,							// Service name to display
				SERVICE_ALL_ACCESS,				// Desired access
				SERVICE_WIN32_OWN_PROCESS,		// Service type
				SERVICE_DEMAND_START,			// Start type
				SERVICE_ERROR_NORMAL,			// Error control type
				szPath,							// Path to service's binary
				NULL,							// No load ordering group
				NULL,							// No tag identifier
				NULL,							// No dependencies
				NULL,							// LocalSystem account
				NULL							// No password
			);

			if (hService == NULL) {
				printf("[!] Failed to create service: %ld\n", GetLastError());
				CloseServiceHandle(hSCManager);
				return (1);
			}

		    // Descripción del servicio
			SERVICE_DESCRIPTION sd;
			sd.lpDescription = (LPSTR)"42 Tinky-Winkey Service";
			ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);

			printf("[+] Service '%s' installed successfully\n", Name);

			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return (0);
		}

	#pragma endregion

	#pragma region "Delete"

		int delete(void) {
			SC_HANDLE hSCManager = NULL;
			SC_HANDLE hService = NULL;
			SERVICE_STATUS serviceStatus;
			
			// Abrir el Service Control Manager
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (hSCManager == NULL) { printf("[!] Error opening Service Control Manager\n"); return (1); }

			// Verificar si el servicio ya existe
			hService = OpenService(hSCManager, Name, SERVICE_ALL_ACCESS);
			if (hService == NULL) {
				printf("[!] Service '%s' doesn't exist\n", Name);
				CloseServiceHandle(hSCManager);
				return (1);
			}

			// Verificar el estado del servicio
			if (QueryServiceStatus(hService, &serviceStatus) == FALSE) {
				printf("[!] Failed to query service status: %ld\n", GetLastError());
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return (1);
			}

			// Si el servicio está en ejecución, intentar detenerlo
			if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
				printf("[*] Stopping service...\n");
				if (ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus) == FALSE) {
					printf("[!] Failed to stop service: %ld\n", GetLastError());
					CloseServiceHandle(hService);
					CloseServiceHandle(hSCManager);
					return (1);
				}

				// Esperar a que el servicio se detenga (con timeout)
				DWORD startTime = GetTickCount();
				while (serviceStatus.dwCurrentState != SERVICE_STOPPED) { Sleep(500);
					if (QueryServiceStatus(hService, &serviceStatus) == FALSE)	{ printf("[!] Failed to query service status\n");		break; }
					if (GetTickCount() - startTime > 10000)						{ printf("[!] Timeout waiting for service to stop\n");	break; }
				}
			}

			// Eliminar el servicio
			if (DeleteService(hService) == FALSE) {
				switch (GetLastError()) {
					case ERROR_SERVICE_MARKED_FOR_DELETE:	printf("[!] Service is already marked for deletion\n");						break;
					case ERROR_ACCESS_DENIED:				printf("[!] Access denied. Ensure no processes are using the service.\n");	break;
					default:								printf("[!] Failed to delete service\n");
				}
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return (1);
			}

			printf("[+] Service '%s' deleted successfully\n", Name);

			// Limpiar handles
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return (0);
		}

	#pragma endregion

	#pragma region "Start"

		int start(void) {
			SC_HANDLE hSCManager = NULL;
			SC_HANDLE hService = NULL;
			
			// Abrir Service Control Manager
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (hSCManager == NULL) { printf("[!] Error opening Service Control Manager\n"); return (1); }

			// Abrir el servicio
			hService = OpenService(hSCManager, Name, SERVICE_START);
			if (hService == NULL) {
				switch (GetLastError()) {
					case ERROR_SERVICE_DOES_NOT_EXIST:		printf("[!] Service does not exist\n");										break;
					case ERROR_ACCESS_DENIED:				printf("[!] Access denied. Check administrator privileges\n");				break;
					default:								printf("[!] Failed to open service\n");
				}
				CloseServiceHandle(hSCManager);
				return (1);
			}

			// Intentar iniciar el servicio
			if (!StartService(hService, 0, NULL)) {
				switch (GetLastError()) {
					case ERROR_SERVICE_ALREADY_RUNNING:		printf("[*] Service is already running\n");									break;
					case ERROR_SERVICE_DISABLED:			printf("[!] Service is disabled\n");										break;
					case ERROR_SERVICE_DOES_NOT_EXIST:		printf("[!] Service does not exist\n");										break;
					case ERROR_ACCESS_DENIED:				printf("[!] Access denied. Check administrator privileges\n");				break;
					default:								printf("[!] Failed to start service\n");
				}
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return (1);
			}
			
			printf("[+] Service '%s' started successfully\n", Name);

			// Limpiar handles
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return (0);
		}

	#pragma endregion

	#pragma region "Stop"

		int stop(void) {
			SC_HANDLE hSCManager = NULL;
			SC_HANDLE hService = NULL;
			SERVICE_STATUS serviceStatus;
			
			// Abrir Service Control Manager
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (hSCManager == NULL) { printf("[!] Error opening Service Control Manager\n"); return (1); }

			// Abrir el servicio
			hService = OpenService(hSCManager, Name, SERVICE_STOP);
			if (hService == NULL) {
				switch (GetLastError()) {
					case ERROR_SERVICE_DOES_NOT_EXIST:		printf("[!] Service does not exist\n");										break;
					case ERROR_ACCESS_DENIED:				printf("[!] Access denied. Check administrator privileges\n");				break;
					default:								printf("[!] Failed to open service\n");
				}
				CloseServiceHandle(hSCManager);
				return (1);
			}

			// Intentar detener el servicio
			if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
				switch (GetLastError()) {
					case ERROR_SERVICE_NOT_ACTIVE:			printf("[*] Service is already stopped\n");									break;
					case ERROR_SERVICE_DOES_NOT_EXIST:		printf("[!] Service does not exist\n");										break;
					default:								printf("[!] Failed to stop service\n");
				}
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return (1);
			}

			// Esperar a que el servicio se detenga (con timeout)
			DWORD startTime = GetTickCount();
			while (serviceStatus.dwCurrentState != SERVICE_STOPPED) { Sleep(500);
				if (QueryServiceStatus(hService, &serviceStatus) == FALSE)	{ printf("[!] Failed to query service status\n");			break; }
				if (GetTickCount() - startTime > 10000)						{ printf("[!] Timeout waiting for service to stop\n");		break; }
			}

			printf("[+] Service '%s' stopped successfully\n", Name);

			// Limpiar handles
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return (0);
		}

	#pragma endregion

	#pragma region "Config"

		int config(char *exe, char *arg) {
			SC_HANDLE hSCManager = NULL;
			SC_HANDLE hService = NULL;
			BOOL success = FALSE;
			DWORD startType;
    		BOOL isDelayedAuto = FALSE;
    		BOOL isRegularAuto = FALSE;

			// Determinar el tipo de inicio según el argumento
			if (!strcmp(arg, "manual") || !strcmp(arg, "demand"))			  startType = SERVICE_DEMAND_START;
			else if (!strcmp(arg, "auto"))									{ startType = SERVICE_AUTO_START; isRegularAuto = TRUE; }
			else if (!strcmp(arg, "delayed-auto"))							{ startType = SERVICE_AUTO_START; isDelayedAuto = TRUE; }
			else if (!strcmp(arg, "disabled"))								  startType = SERVICE_DISABLED;
			else															return (help(exe), 1);

			// Abrir el Service Control Manager
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (hSCManager == NULL) { printf("[!] Error opening Service Control Manager\n"); return (1); }

			// Abrir el servicio
			hService = OpenService(hSCManager, Name, SERVICE_CHANGE_CONFIG);
			if (hService == NULL) {
				printf("[!] Failed to open service\n");
				CloseServiceHandle(hSCManager);
				return (1);
			}

			// Cambiar la configuración de inicio del servicio
			success = ChangeServiceConfig(
				hService,						// handle to the service
				SERVICE_NO_CHANGE,				// service type (no changes)
				startType,						// start type
				SERVICE_NO_CHANGE,				// error control (no changes)
				NULL,							// binary path (no changes)
				NULL,							// load order group (no changes)
				NULL,							// tag ID (no changes)
				NULL,							// dependencies (no changes)
				NULL,							// service account (no changes)
				NULL,							// password (no changes)
				NULL							// display name (no changes)
			);

			if (!success) {
				printf("[!] Failed to configure service\n");
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return (1);
			}

			// Configurar o desactivar el inicio retrasado según corresponda
			SERVICE_DELAYED_AUTO_START_INFO delayedInfo;
			delayedInfo.fDelayedAutostart = isDelayedAuto ? TRUE : FALSE;
			
			if (isDelayedAuto || isRegularAuto) {
				success = ChangeServiceConfig2(hService, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &delayedInfo);
				if (!success) printf("[!] Failed to %s delayed auto-start\n", isDelayedAuto ? "set" : "clear");
			}

			printf("[+] Service start type successfully set to '%s'\n", arg);

			// Limpiar handles
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			
			return (0);
		}

	#pragma endregion

	#pragma region "Status"

		int status(void) {
			SC_HANDLE hSCManager = NULL;
			SC_HANDLE hService = NULL;
			SERVICE_STATUS_PROCESS statusProcess;
			DWORD bytesNeeded = 0;
			LPQUERY_SERVICE_CONFIG lpServiceConfig = NULL;
			SERVICE_DELAYED_AUTO_START_INFO delayedInfo;
			BOOL isDelayedStart = FALSE;
			DWORD cbBytesNeeded = 0;
			
			printf("Status of %s:\n", Name);
			
			// Open the Service Control Manager
			hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
			if (hSCManager == NULL) {
				printf("[!] Error opening Service Control Manager\n");
				return 1;
			}
			
			// Open the service
			hService = OpenService(hSCManager, Name, SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG);
			if (hService == NULL) {
				printf("[!] Service not found or insufficient permissions\n");
				CloseServiceHandle(hSCManager);
				return 1;
			}
			
			// Get service configuration
			// First call to get required buffer size
			if (!QueryServiceConfig(hService, NULL, 0, &cbBytesNeeded) && 
				GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				
				lpServiceConfig = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED, cbBytesNeeded);
				if (lpServiceConfig != NULL) {
					if (!QueryServiceConfig(hService, lpServiceConfig, cbBytesNeeded, &cbBytesNeeded)) {
						printf("[!] Failed to query service configuration\n");
						LocalFree(lpServiceConfig);
						CloseServiceHandle(hService);
						CloseServiceHandle(hSCManager);
						return 1;
					}
				}
			}
			
			// Get current service status
			if (!QueryServiceStatusEx(
				hService,
				SC_STATUS_PROCESS_INFO,
				(LPBYTE)&statusProcess,
				sizeof(SERVICE_STATUS_PROCESS),
				&bytesNeeded)) {
				
				printf("[!] Failed to query service status\n");
				if (lpServiceConfig != NULL) LocalFree(lpServiceConfig);
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return 1;
			}
			
			// Check if service is delayed start (only applies to auto-start services)
			if (lpServiceConfig != NULL && lpServiceConfig->dwStartType == SERVICE_AUTO_START) {
				if (QueryServiceConfig2(
					hService,
					SERVICE_CONFIG_DELAYED_AUTO_START_INFO,
					(LPBYTE)&delayedInfo,
					sizeof(SERVICE_DELAYED_AUTO_START_INFO),
					&bytesNeeded)) {
					
					isDelayedStart = delayedInfo.fDelayedAutostart;
				}
			}
			
			// Display service information
			printf("\nSERVICE DETAILS\n");
			printf("---------------\n");
			
			// Display service status
			printf("Status:         ");
			switch (statusProcess.dwCurrentState) {
				case SERVICE_STOPPED:
					printf("STOPPED\n");
					break;
				case SERVICE_START_PENDING:
					printf("STARTING\n");
					break;
				case SERVICE_STOP_PENDING:
					printf("STOPPING\n");
					break;
				case SERVICE_RUNNING:
					printf("RUNNING\n");
					break;
				case SERVICE_CONTINUE_PENDING:
					printf("CONTINUE PENDING\n");
					break;
				case SERVICE_PAUSE_PENDING:
					printf("PAUSE PENDING\n");
					break;
				case SERVICE_PAUSED:
					printf("PAUSED\n");
					break;
				default:
					printf("UNKNOWN\n");
					break;
			}
			
			// Display start type if available
			if (lpServiceConfig != NULL) {
				printf("Start Type:     ");
				switch (lpServiceConfig->dwStartType) {
					case SERVICE_AUTO_START:
						if (isDelayedStart)
							printf("AUTO (DELAYED)\n");
						else
							printf("AUTO\n");
						break;
					case SERVICE_DEMAND_START:
						printf("MANUAL\n");
						break;
					case SERVICE_DISABLED:
						printf("DISABLED\n");
						break;
					case SERVICE_BOOT_START:
						printf("BOOT\n");
						break;
					case SERVICE_SYSTEM_START:
						printf("SYSTEM\n");
						break;
					default:
						printf("UNKNOWN\n");
						break;
				}
				
				// Display binary path
				printf("Binary Path:    %s\n", lpServiceConfig->lpBinaryPathName);
				
				// Display service type
				printf("Service Type:   ");
				if (lpServiceConfig->dwServiceType & SERVICE_WIN32_OWN_PROCESS)
					printf("WIN32_OWN_PROCESS");
				if (lpServiceConfig->dwServiceType & SERVICE_WIN32_SHARE_PROCESS)
					printf("WIN32_SHARE_PROCESS");
				if (lpServiceConfig->dwServiceType & SERVICE_KERNEL_DRIVER)
					printf("KERNEL_DRIVER");
				if (lpServiceConfig->dwServiceType & SERVICE_FILE_SYSTEM_DRIVER)
					printf("FILE_SYSTEM_DRIVER");
				if (lpServiceConfig->dwServiceType & SERVICE_INTERACTIVE_PROCESS)
					printf(" (INTERACTIVE)");
				printf("\n");
				
				// Display account name
				printf("Account:        %s\n", lpServiceConfig->lpServiceStartName);
				
				// Display display name
				printf("Display Name:   %s\n", lpServiceConfig->lpDisplayName);
			}
			
			// Display process ID if running
			if (statusProcess.dwCurrentState == SERVICE_RUNNING || 
				statusProcess.dwCurrentState == SERVICE_PAUSED) {
				printf("Process ID:     %lu\n", statusProcess.dwProcessId);
			}
			
			// Clean up
			if (lpServiceConfig != NULL) {
				LocalFree(lpServiceConfig);
			}
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			
			return 0;
		}

	#pragma endregion

	#pragma region "Update ??"

		// svc.exe update [path]
		int update(char *dst, char *src) {
			printf("Updating file %s with %s\n", dst, src);
			// cmd.exe /c timeout 2 & move /Y update.exe svc.exe & start svc.exe start

			return (0);
		}

	#pragma endregion

#pragma endregion

#pragma region "Control"

	int control(int argc, char **argv) {
		printf("\n");

		if (argc <= 1) ;
		else if (argc == 2 && !strcmp(argv[1], "install"))   													return (install());
		else if (argc == 2 && !strcmp(argv[1], "delete"))    													return (delete());

		else if (argc == 2 && !strcmp(argv[1], "start"))     													return (start());
		else if (argc == 2 && !strcmp(argv[1], "stop"))      													return (stop());

		else if (argc == 2 && (!strcmp(argv[1], "help") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "/?")))	return (help(argv[0]));
		else if (argc == 2 && (!strcmp(argv[1], "version") || !strcmp(argv[1], "-v")))							return (printf("%s version: %s\n", Name, Version), 0);

		else if (argc == 2 && !strcmp(argv[1], "status"))														return (status());
		else if (argc == 3 && !strcmp(argv[1], "update"))														return (update(argv[0], argv[2]));
		else if (argc == 4 && !strcmp(argv[1], "config") && !strcmp(argv[2], "start"))							return (config(argv[0], argv[3]));

		return (help(argv[0]), 1);
	}

#pragma endregion

			// SERVICE_BOOT_START				Dispositivos del sistema operativo
			// SERVICE_SYSTEM_START				Dispositivos de inicio del sistema
			// SERVICE_AUTO_START				Inicio automático al arrancar Windows
			// SERVICE_DEMAND_START				Inicio manual
			// SERVICE_DISABLED					Servicio deshabilitado