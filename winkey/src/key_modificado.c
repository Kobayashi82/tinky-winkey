#include "winkey.h"
#include <wctype.h>

char* VirtualKeyToChar(
    DWORD vkCode,  // Codigo virtual, posicion fisica de la tecla
    DWORD scanCode, // Codigo hardware de la tecla (especifico del fabricante)
    HKL keyboardLayout //layout actual(Qwerty, AZERTY, etc), idioma SO y configuracion regional
)
{
    (void)vkCode;
    (void)scanCode;
    (void)keyboardLayout;
    static char keyName[64];

    // Limpiar el buffer
    memset(keyName, 0, sizeof(keyName));
    keyName[0] = '\0';

    //Obtener estado de todas las teclas
    BYTE keyState[256];
    GetKeyboardState(keyState);

    //Donde poner el resultado
    WCHAR buffer[64];

    //tamano del buffer
    int bufferSize = sizeof(buffer) / sizeof(WCHAR);

    // que diccionario usar
    HKL layout = keyboardLayout;

    // Obtener el caracter o nombre de la tecla
    int result = ToUnicodeEx(
        vkCode, // 1. Qué tecla presionaste (vkCode)
        scanCode, // 2. Código del hardware (scanCode)
        keyState, // 3. Estado de TODAS las teclas
        buffer, // 4. Dónde poner el resultado
        bufferSize, // 5. Tamaño del resultado
        0, // 6. Opciones especiales (normalmente 0)
        layout // 7. Qué "diccionario" usar (keyboardLayout)
    );

    if (result > 0) {
        // Si result es positivo, significa que se obtuvo un caracter
        // Convertir de WCHAR a char
        buffer[result] = L'\0'; // Asegurarse de que el string esté terminado en nulo
        // Convertir a UTF-8    
        WideCharToMultiByte(CP_UTF8, 0, buffer, -1, keyName, sizeof(keyName), NULL, NULL);
    } else if (result == 0) {
        // Si result es 0, significa que no se pudo convertir a un caracter
        snprintf(keyName, sizeof(keyName), "VK_%lu", vkCode); // Usar el codigo virtual como nombre
    } else {
        // Si result es negativo, significa que hubo un error
        snprintf(keyName, sizeof(keyName), "Error_%d", result);
    }

    return keyName;
}