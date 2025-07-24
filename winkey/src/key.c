#include "winkey.h"

char* VirtualKeyToChar(DWORD vkCode, DWORD scanCode)
{
    static char keyName[32];
    static WCHAR unicodeChar[2];
    BYTE keyboardState[256] = {0};


    switch(vkCode)
    {
        case VK_RETURN:     return "[ENTER]\n";
        case VK_TAB:        return "[TAB]";
        case VK_BACK:       return "[BACKSPACE]";
        case VK_DELETE:     return "[DELETE]";
        case VK_ESCAPE:     return "[ESCAPE]";
        case VK_LWIN:       return "[L_WIN]";
        case VK_RWIN:       return "[R_WIN]";
        case VK_CAPITAL:    return "[CAPS_LOCK]"; // Corregido el bug
        case VK_NUMLOCK:    return "[NUM_LOCK]";
        case VK_LEFT:       return "[LEFT_ARROW]";
        case VK_RIGHT:      return "[RIGHT_ARROW]";
        case VK_UP:         return "[UP_ARROW]";
        case VK_DOWN:       return "[DOWN_ARROW]";
        case VK_F1:         return "[F1]";
        case VK_F2:         return "[F2]";
        case VK_F3:         return "[F3]";
        case VK_F4:         return "[F4]";
        case VK_F5:         return "[F5]";
        case VK_F6:         return "[F6]";
        case VK_F7:         return "[F7]";
        case VK_F8:         return "[F8]";
        case VK_F9:         return "[F9]";
        case VK_F10:        return "[F10]";
        case VK_F11:        return "[F11]";
        case VK_F12:        return "[F12]";
        case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT:
        case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL:
        case VK_MENU: case VK_LMENU: case VK_RMENU:
        default:
            // Para otras teclas no imprimibles, no registramos nada
            return "";
    }

    // Comprueba el estado fisico de la tecla en el momento de llamarla
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

    // Traducir el codigo de la tecla a un caracter Unicode
    int lenUnicode = sizeof(unicodeChar) / sizeof(WCHAR);
    int result = ToUnicode(vkCode, scanCode, keyboardState, unicodeChar, lenUnicode, 0);
    if (result > 0) {
        // Si la conversion fue valida, convertimos el caracter Unicode (WCHAR) a un string 
        // multibyte (char *)
        unicodeChar[result] = '\0'; // string termine en nulo
        WideCharToMultiByte(CP_UTF8, 0, unicodeChar, -1, keyName, sizeof(keyName), NULL, NULL);
        return keyName;
    }
}