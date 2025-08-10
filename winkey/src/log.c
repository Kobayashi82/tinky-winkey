#include "winkey.h"

// Abre el archivo de Log en C:\Users\Public\winkey.log
BOOL OpenLogFile(void)
{
    char logPath[MAX_PATH];
    time_t currentTime;
    struct tm timeInfo;

    // Crea la ruta completa del archivo de log
    snprintf(logPath, sizeof(logPath), "C:\\Users\\Public\\winkey.log");

    // Abre el archivo de log de forma segura 
    if (fopen_s(&g_winkeyState.logFile, logPath, "a") != 0 || !g_winkeyState.logFile)
        return FALSE;

    // Obtiene tiempo actual y escribe cabecera de inicio
    time(&currentTime);
    localtime_s(&timeInfo, &currentTime);
    
    fprintf(g_winkeyState.logFile, "\n=== Winkey Started: %02d/%02d/%04d %02d:%02d:%02d ===\n",
            timeInfo.tm_mday, timeInfo.tm_mon + 1, timeInfo.tm_year + 1900,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    
    fflush(g_winkeyState.logFile);

    return TRUE;
}