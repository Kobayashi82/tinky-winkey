#include "winkey.h"

char* VirtualKeyToChar(DWORD vkCode)
{
    static char keyName[32];

    switch(vkCode)
    {
        // Teclas especiales no imprimibles
        case VK_SPACE:      return "[SPACE]";
        case VK_RETURN:     return "[ENTER]\n";
        case VK_TAB:        return "[TAB]";
        case VK_BACK:       return "[BACKSPACE]";
        case VK_DELETE:     return "[DELETE]";
        case VK_ESCAPE:     return "[ESCAPE]";

        // Modificadores
        case VK_SHIFT:      return "[SHIFT]+";
        case VK_LSHIFT:     return "[L_SHIFT]+";
        case VK_RSHIFT:     return "[R_SHIFT]+";
        case VK_CONTROL:    return "[CTRL]+";
        case VK_LCONTROL:   return "[L_CTRL]+";
        case VK_RCONTROL:   return "[R_CTRL]+";
        case VK_MENU:       return "[ALT]+";
        case VK_LMENU:      return "[L_ALT]+";
        case VK_RMENU:      return "[R_ALT]+";
        case VK_LWIN:       return "[L_WIN]";
        case VK_RWIN:       return "[R_WIN]";
        case VK_CAPITAL:    return "[CAPS_LOCK]";
        case VK_NUMLOCK:    return "[NUM_LOCK]";

        // Flechas
        case VK_LEFT:       return "[LEFT_ARROW]";
        case VK_RIGHT:      return "[RIGHT_ARROW]";
        case VK_UP:         return "[UP_ARROW]";
        case VK_DOWN:       return "[DOWN_ARROW]";

        // Teclas de funcion F1-F12
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

        // Teclado numerico (Numpad)
        case VK_NUMPAD0:    return "0";
        case VK_NUMPAD1:    return "1";
        case VK_NUMPAD2:    return "2";
        case VK_NUMPAD3:    return "3";
        case VK_NUMPAD4:    return "4";
        case VK_NUMPAD5:    return "5";
        case VK_NUMPAD6:    return "6";
        case VK_NUMPAD7:    return "7";
        case VK_NUMPAD8:    return "8";
        case VK_NUMPAD9:    return "9";
        case VK_MULTIPLY:   return "*";
        case VK_ADD:        return "+";
        case VK_SUBTRACT:   return "-";
        case VK_DECIMAL:    return ".";
        case VK_DIVIDE:     return "/";

        // Teclas de Simbolos (OEM)
        case VK_OEM_1:      return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? ":" : ";";
        case VK_OEM_PLUS:   return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "+" : "=";
        case VK_OEM_COMMA:  return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "<" : ",";
        case VK_OEM_MINUS:  return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "_" : "-";
        case VK_OEM_PERIOD: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? ">" : ".";
        case VK_OEM_2:      return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "?" : "/";
        case VK_OEM_3:      return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "~" : "`";
        case VK_OEM_4:      return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "{" : "[";
        case VK_OEM_5:      return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "|" : "\\";
        case VK_OEM_6:      return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "}" : "]";
        case VK_OEM_7:      return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? "\"" : "'";
        default:
            // Para caracteres alfanumericos y simbolos
            if (vkCode >= 'A' && vkCode <= 'Z') {
                // Verificar si Shift esta presionado
                BOOL shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                BOOL capsLock = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

                char c = (char)vkCode;
                if (!(shiftPressed ^ capsLock)) { // XOR: minuscula si solo uno esta activo
                    c = c + 32; // convertir a minuscula
                }
                sprintf_s(keyName, sizeof(keyName), "%c", c);
                return keyName;
            }
            // Para numeros de la fila superior del teclado
            else if (vkCode >= '0' && vkCode <= '9') {
                // Numeros con posibles simbolos si Shift esta presionado
                if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                    char symbols[] = ")!@#$%^&*(";
                    sprintf_s(keyName, sizeof(keyName), "%c", symbols[vkCode - '0']);
                }
                else {
                    sprintf_s(keyName, sizeof(keyName), "%c", (char)vkCode);
                }
                return keyName;
            }
            else {
                // Para otro codigos, mostramos el numero
                sprintf_s(keyName, sizeof(keyName), "VK_%lu", vkCode);
                return keyName;
            }
    }
}