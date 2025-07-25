#include "winkey.h"
#include <wctype.h> // Necesario para iswprint

// La función ahora acepta un tercer parámetro: HKL keyboardLayout
char* VirtualKeyToChar(DWORD vkCode, DWORD scanCode, HKL keyboardLayout)
{
    static char keyName[32];
    static WCHAR unicodeChar[2];
    BYTE keyboardState[256] = {0};

    // Primero, manejamos las teclas especiales y modificadores que siempre deben tener un nombre.
    switch(vkCode)
    {
        // Modificadores
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:     return "[SHIFT]";
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:   return "[CTRL]";
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:      return "[ALT]";

        // Teclas de acción
        case VK_RETURN:     return "[ENTER]\n";
        case VK_TAB:        return "[TAB]";
        case VK_BACK:       return "[BACKSPACE]";
        case VK_DELETE:     return "[DELETE]";
        case VK_ESCAPE:     return "[ESCAPE]";
        case VK_LWIN:       return "[L_WIN]";
        case VK_RWIN:       return "[R_WIN]";
        case VK_CAPITAL:    return "[CAPS_LOCK]";
        case VK_NUMLOCK:    return "[NUM_LOCK]";
        case VK_LEFT:       return "[LEFT_ARROW]";
        case VK_RIGHT:      return "[RIGHT_ARROW]";
        case VK_UP:         return "[UP_ARROW]";
        case VK_DOWN:       return "[DOWN_ARROW]";
        // ... (el switch completo se queda exactamente igual) ...
        case VK_F1: case VK_F2: case VK_F3: case VK_F4: case VK_F5: case VK_F6:
        case VK_F7: case VK_F8: case VK_F9: case VK_F10: case VK_F11: case VK_F12:
            sprintf_s(keyName, sizeof(keyName), "[F%lu]", vkCode - VK_F1 + 1);
            return keyName;
    }

    // Construimos el estado del teclado (esto no cambia)
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        keyboardState[VK_SHIFT] = 0x80;
    }
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
        keyboardState[VK_CONTROL] = 0x80;
    }
    if (GetAsyncKeyState(VK_MENU) & 0x8000) { // Tecla ALT
        keyboardState[VK_MENU] = 0x80;
    }
    if (GetKeyState(VK_CAPITAL) & 1) { // Para Bloq Mayus.
        keyboardState[VK_CAPITAL] = 0x01;
    }

    // --- CAMBIO IMPORTANTE: Usamos ToUnicodeEx ---
    // Traducimos el código de la tecla usando la distribución de teclado correcta.
    int result = ToUnicodeEx(vkCode, scanCode, keyboardState, unicodeChar, 2, 0, keyboardLayout);

    // Solo devolvemos el carácter si es imprimible (evita el '♥' y otros).
    if (result > 0 && iswprint(unicodeChar[0])) {
        unicodeChar[result] = '\0';
        WideCharToMultiByte(CP_UTF8, 0, unicodeChar, -1, keyName, sizeof(keyName), NULL, NULL);
        return keyName;
    }

    // Si no es una tecla especial con nombre ni un carácter imprimible, la ignoramos.
    return "";
}