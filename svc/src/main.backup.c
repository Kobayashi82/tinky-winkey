/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.backup                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/11 14:28:19 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// cmd.exe /c timeout 2 & move /Y update.exe svc.exe & start svc.exe /start

#pragma region "Includes"

	#include "svc.h"

#pragma endregion

#pragma region "Methods"

	// svc.exe install -s, --silent							Instala sin mostrar mensajes (modo silencioso)

int config(int argc, char **argv) {
	(void) argc;
	(void) argv;
	// svc.exe config start demand, manual					Configura el inicio manual
	// svc.exe config start auto							Configura el inicio automático
	// svc.exe config start delayed-auto					Configura el inicio automático con retraso
	// svc.exe config start disabled						Deshabilita el servicio
	// svc.exe config description "texto descriptivo"		Establece la descripción del servicio
	// svc.exe config displayname "nombre para mostrar"		Cambia el nombre visible del servicio
	// svc.exe config depend "servicio1/servicio2"			Establece dependencias de otros servicios
	return (0);
}

int install(int argc, char **argv) {
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	char servicePath[MAX_PATH];
	
	printf("[*] Instalando servicio...\n");
	
	// Obtener la ruta completa del ejecutable
	if (!GetModuleFileName(NULL, servicePath, MAX_PATH)) {
		printf("[!] Error: No se pudo obtener la ruta del ejecutable: %d\n", GetLastError());
		return 1;
	}
	
	// Conectar con el Service Control Manager
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (NULL == schSCManager) {
		printf("[!] Error: No se pudo conectar al Service Control Manager: %d\n", GetLastError());
		return 1;
	}

	// Argumentos del servicio (opcionales)
	// Estos argumentos estarían disponibles en ServiceMain(dwArgc, lpszArgv)
	// Por ejemplo: "-service" o cualquier otro parámetro
	char serviceArgs[MAX_PATH];
	strcpy(serviceArgs, servicePath);
	strcat(serviceArgs, " -service");  // Argumento opcional que podría usar el servicio
	
	// Crear el servicio
	schService = CreateService(
		schSCManager,              // SCM handle
		Name,                      // Nombre del servicio
		"Servicio Tinky Winkey",   // Nombre para mostrar
		SERVICE_ALL_ACCESS,        // Acceso deseado
		SERVICE_WIN32_OWN_PROCESS, // Tipo de servicio
		SERVICE_DEMAND_START,      // Tipo de inicio (manual)
		SERVICE_ERROR_NORMAL,      // Control de errores
		servicePath,               // Ruta al ejecutable
		NULL,                      // Sin grupo de carga
		NULL,                      // Sin tag de carga
		NULL,                      // Sin dependencias
		NULL,                      // Cuenta Local System
		NULL);                     // Sin contraseña
	
	if (schService == NULL) {
		DWORD err = GetLastError();
		if (err == ERROR_SERVICE_EXISTS) {
			printf("[!] Error: El servicio '%s' ya existe.\n", Name);
		} else {
			printf("[!] Error: No se pudo crear el servicio: %d\n", err);
		}
		CloseServiceHandle(schSCManager);
		return 1;
	}
	
	printf("[+] Servicio '%s' instalado correctamente.\n", Name);
	
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return 0;
}

int start(int argc, char **argv) {
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	BOOL stopping = FALSE;
	
	// Verificar si se trata de una solicitud de detener el servicio
	if (argc > 0 && strcmp(argv[0], "stop") == 0) {
		stopping = TRUE;
	}
	
	// Mensaje inicial
	if (stopping) {
		printf("[*] Deteniendo servicio...\n");
	} else {
		printf("[*] Iniciando servicio...\n");
	}
	
	// Conectar con el Service Control Manager
	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (NULL == schSCManager) {
		printf("[!] Error: No se pudo conectar al Service Control Manager: %d\n", GetLastError());
		return 1;
	}
	
	// Abrir el servicio
	schService = OpenService(schSCManager, Name, SERVICE_START | SERVICE_STOP | SERVICE_QUERY_STATUS);
	if (schService == NULL) {
		printf("[!] Error: No se pudo abrir el servicio '%s': %d\n", Name, GetLastError());
		CloseServiceHandle(schSCManager);
		return 1;
	}
	
	// Si estamos deteniendo el servicio
	if (stopping) {
		// Verificar estado actual
		if (!QueryServiceStatusEx(
			schService, SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded)) {
				printf("[!] Error: No se pudo consultar el estado del servicio: %d\n", GetLastError());
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return 1;
		}
		
		// Si el servicio no está en ejecución
		if (ssp.dwCurrentState == SERVICE_STOPPED) {
			printf("[*] El servicio '%s' ya está detenido.\n", Name);
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return 0;
		}
		
		// Intentar detener el servicio
		if (!ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
			printf("[!] Error: No se pudo detener el servicio '%s': %d\n", Name, GetLastError());
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return 1;
		}
		
		printf("[+] Servicio '%s' detenido correctamente.\n", Name);
	}
	// Si estamos iniciando el servicio
	else {
		// Verificar estado actual
		if (!QueryServiceStatusEx(
			schService, SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded)) {
				printf("[!] Error: No se pudo consultar el estado del servicio: %d\n", GetLastError());
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return 1;
		}
		
		// Si el servicio ya está en ejecución
		if (ssp.dwCurrentState != SERVICE_STOPPED) {
			printf("[*] El servicio '%s' ya está en ejecución o pendiente.\n", Name);
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return 0;
		}
		
		// Iniciar el servicio
		if (!StartService(schService, 0, NULL)) {
			printf("[!] Error: No se pudo iniciar el servicio '%s': %d\n", Name, GetLastError());
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return 1;
		}
		
		printf("[+] Servicio '%s' iniciado correctamente.\n", Name);
	}
	
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return 0;
}

int utils(int argc, char **argv) {
	(void) argc;
	(void) argv;

	// [*] Installing service...
	// [+] Service 'winkey' installed successfully.

	// svc.exe status, query								Muestra el estado actual del servicio
	// svc.exe help, -h, /?									Muestra información de ayuda sobre los comandos disponibles
	// svc.exe version, -v									Muestra información de versión
	return (0);
}

int update(char *arg) {
	// svc.exe service, -service							Se ejecuta como un servicio (ejecuta winkey.exe)
	return (0);
}

int service() {
	// Esta función se llamaría cuando el servicio se ejecuta en modo servicio
	// Aquí podríamos iniciar el proceso winkey.exe o cualquier otra funcionalidad
	
	printf("Ejecutando en modo servicio explícito\n");
	
	// Registrarnos como servicio
	SERVICE_TABLE_ENTRY serviceTable[] = {
		{ Name, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ NULL, NULL }
	};
	
	if (!StartServiceCtrlDispatcher(serviceTable)) {
		printf("Error al iniciar como servicio: %d\n", GetLastError());
		return 1;
	}
	
	return 0;
}

#pragma endregion

char *Name = "Tinky";
SERVICE_STATUS g_ServiceStatus;
SERVICE_STATUS_HANDLE g_StatusHandle;

// Función que se ejecuta cuando se inicia el servicio
void ReportStatusToSCMgr(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint) {
    // Reporta el estado del servicio al Service Control Manager (SCM)
    g_ServiceStatus.dwCurrentState = dwCurrentState;
    g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
    g_ServiceStatus.dwWaitHint = dwWaitHint;

    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

// Handler para eventos de control de servicio (cuando SCM envía un comando)
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
    switch (dwCtrl) {
        case SERVICE_CONTROL_STOP:
            ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
            
            // Aquí iría la lógica para detener limpiamente el servicio
            
            ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
            break;
            
        case SERVICE_CONTROL_INTERROGATE:
            // Simplemente devolver estado actual
            break;
            
        default:
            // No hacemos nada para otros comandos
            break;
    }
}





// Función principal del servicio
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
    FILE *logFile = fopen("C:\\Windows\\Temp\\tinky_service.log", "a");
    if (logFile) {
        fprintf(logFile, "Servicio iniciado con %d argumentos:\n", (int)dwArgc);
        for (DWORD i = 0; i < dwArgc; i++) {
            fprintf(logFile, "  Arg %d: %s\n", (int)i, lpszArgv[i]);
        }
        fclose(logFile);
    }

    // Reportar estado de "Arrancando"
    ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 0);

    // Aquí se podría iniciar el proceso winkey.exe o la funcionalidad principal del servicio
    // Por ahora, solo se duerme 10 segundos para demostración
    Sleep(10000);

    // Detener el servicio cuando termine su trabajo
    ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
}


int main(int argc, char **argv) {
    // Paso 1: Intentar iniciar como servicio
    // Si este programa ha sido iniciado por el SCM, esto tendrá éxito
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { Name, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    // Registrarnos como servicio
    // Si tiene éxito, la función no regresará hasta que el servicio se detenga
    if (StartServiceCtrlDispatcher(serviceTable)) {
        // Si llegamos aquí, el servicio se ha detenido correctamente
        return (0);
    }

    // Si StartServiceCtrlDispatcher falla con este error específico,
    // significa que estamos ejecutándonos como aplicación normal (no como servicio)
    if (GetLastError() != ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
        printf("Error al iniciar el servicio: %d\n", GetLastError());
        return (1);
    }

    // Paso 2: Si llegamos aquí, no fuimos iniciados como servicio
    // Procesamos los argumentos para realizar acciones de administración del servicio

    // Mostrar ayuda si no hay argumentos
    if (argc <= 1) {
        printf("Uso: %s [comando] [opciones]\n", argv[0]);
        printf("Comandos disponibles:\n");
        printf("  install     - Instala el servicio\n");
        printf("  start       - Inicia el servicio\n");
        printf("  stop        - Detiene el servicio\n");
        printf("  status      - Muestra el estado del servicio\n");
        printf("  config      - Configura el servicio\n");
        printf("  help        - Muestra esta ayuda\n");
        return (1);
    }
    
    // Procesar comandos de administración
    if (strcmp(argv[1], "install") == 0) {
        return install(argc - 1, &argv[1]);
    } 
    else if (strcmp(argv[1], "start") == 0) {
        return start(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "stop") == 0) {
        // Lógica para detener el servicio
        return start(argc - 1, &argv[1]);  // Reutilizo la función start
    }
    else if (strcmp(argv[1], "config") == 0) {
        return config(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "status") == 0 || strcmp(argv[1], "query") == 0) {
        return utils(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || 
             strcmp(argv[1], "/?") == 0) {
        return utils(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "version") == 0 || strcmp(argv[1], "-v") == 0) {
        return utils(argc - 1, &argv[1]);
    }
    else if (strcmp(argv[1], "service") == 0 || 
             strcmp(argv[1], "-service") == 0 || 
             strcmp(argv[1], "/service") == 0) {
        return service();
    }
    else {
        printf("Comando desconocido: %s\n", argv[1]);
        printf("Ejecute '%s help' para ver la lista de comandos disponibles\n", argv[0]);
        return 1;
    }
}
