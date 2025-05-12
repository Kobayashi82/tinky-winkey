#include <windows.h>
#include <stdio.h>

// Función para comprobar si se está ejecutando como administrador
BOOL IsRunAsAdmin() {
    BOOL isAdmin = FALSE;
    HANDLE hToken = NULL;
    
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD dwSize = sizeof(TOKEN_ELEVATION);
        
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
            isAdmin = elevation.TokenIsElevated;
        }
        
        CloseHandle(hToken);
    }
    
    return isAdmin;
}

int main() {
    // Comprobar si se está ejecutando como administrador
    BOOL isAdmin = IsRunAsAdmin();
    
    // Abrir un archivo para escribir el resultado
    FILE* file = fopen("D:\\winkey_status.txt", "w");
    
    if (file) {
        // Obtener la hora actual
        SYSTEMTIME st;
        GetLocalTime(&st);
        
        // Escribir información en el archivo
        fprintf(file, "Fecha: %02d/%02d/%04d %02d:%02d:%02d\n", 
                st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
        
        if (isAdmin) {
            fprintf(file, "Estado: Ejecutándose como administrador\n");
			printf("Estado: Ejecutandose como administrador\n");
        } else {
            fprintf(file, "Estado: Ejecutándose como usuario normal\n");
			printf("Estado: Ejecutandose como usuario normal\n");
        }
        
        // Información del sistema
        TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(computerName) / sizeof(computerName[0]);
        if (GetComputerName(computerName, &size)) {
            fprintf(file, "Equipo: %s\n", computerName);
        }
        
        TCHAR userName[MAX_PATH];
        size = sizeof(userName) / sizeof(userName[0]);
        if (GetUserName(userName, &size)) {
            fprintf(file, "Usuario: %s\n", userName);
        }
        
        fclose(file);
    }
    
    // Esperar un poco antes de terminar (opcional)
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, "Global\\WinkeyTerminateEvent");
	while(1) {
		if (WaitForSingleObject(hEvent, 100) == WAIT_OBJECT_0) {
			FILE* file = fopen("D:\\winkey_exit.txt", "w");
    		if (file) {
				fprintf(file, "Estado: saliendo\n");
				fclose(file);
				printf("Estado: saliendo\n");			
			}
			break;
		}
	}
    
    return 0;
}