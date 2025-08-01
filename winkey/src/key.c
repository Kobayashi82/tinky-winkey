#include "winkey.h"
#include <wctype.h>

char* VirtualKeyToChar(
    DWORD vkCode,  // Codigo virtual, posicion fisica de la tecla
    DWORD scanCode, // Codigo hardware de la tecla (especifico del fabricante)
    HKL keyboardLayout //layout actual(Qwerty, AZERTY, etc), idioma SO y configuracion regional
)
{
    (void)scanCode;
    (void)keyboardLayout;
    (void)vkCode;
    static char keyName[64];

    //limpiar el buffer
    keyName[0] = '\0';

    /*     int ToUnicodeEx(
        UINT wVirtKey,        // 1. Qué tecla presionaste (vkCode)
        UINT wScanCode,       // 2. Código del hardware (scanCode)
        const BYTE *lpKeyState, // 3. Estado de TODAS las teclas
        LPWSTR pwszBuff,      // 4. Dónde poner el resultado
        int cchBuff,          // 5. Tamaño del resultado
        UINT wFlags,          // 6. Opciones especiales (normalmente 0)
        HKL dwhkl            // 7. Qué "diccionario" usar (keyboardLayout)
    ); */

    return keyName;
}