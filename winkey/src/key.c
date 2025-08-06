#include "winkey.h"
#include <wctype.h>

// Función para convertir vkCode a string legible
const char* VkCodeToString(DWORD vkCode) {
    switch (vkCode) {
        case VK_BACK: return "[BACKSPACE]";
        case VK_TAB: return "[TAB]";
        case VK_RETURN: return "[\\n]\n";
        case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT: return "";
        case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL: return "";
        case VK_MENU: case VK_LMENU: case VK_RMENU: return "";
        case VK_CAPITAL: return "[CAPSLOCK]";
        case VK_ESCAPE: return "[ESC]";
        case VK_SPACE: return "[SPACE]";
        case VK_LEFT: return "[LEFT]";
        case VK_RIGHT: return "[RIGHT]";
        case VK_UP: return "[UP]";
        case VK_DOWN: return "[DOWN]";
        case VK_DELETE: return "[DELETE]";
        case VK_HOME: return "[HOME]";
        case VK_END: return "[END]";
        case VK_PRIOR: return "[PAGEUP]";
        case VK_NEXT: return "[PAGEDOWN]";
        case VK_INSERT: return "[INSERT]";
        case VK_LWIN: case VK_RWIN: return "[WIN]";
        case VK_F1: return "[F1]";
        case VK_F2: return "[F2]";
        case VK_F3: return "[F3]";
        case VK_F4: return "[F4]";
        case VK_F5: return "[F5]";
        case VK_F6: return "[F6]";
        case VK_F7: return "[F7]";
        case VK_F8: return "[F8]";
        case VK_F9: return "[F9]";
        case VK_F10: return "[F10]";
        case VK_F11: return "[F11]";
        case VK_F12: return "[F12]";
        // Teclas especiales del teclado español (UTF-8)
        /* case 186: {
            int shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            return shiftPressed ? "º" : "ª";
        } */
        //case 187: return "+";           // VK_OEM_PLUS
        //case 188: return ",";           // VK_OEM_COMMA
        //case 189: return "-";           // VK_OEM_MINUS
        //case 190: return ".";           // VK_OEM_PERIOD
        //case 191: return "ç";           // VK_OEM_2 en layout español
        case 192: { // VK_OEM_1 en layout español (ñ/Ñ)
            int shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            return shiftPressed ? "Ñ" : "ñ";
        }
        //case 219: return "'";           // VK_OEM_4 en layout español
        case 220: {
            int shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            return shiftPressed ? "º" : "ª";
        }
        //case 221: return "¡";           // VK_OEM_6 en layout español
        //case 222: return "´";           // VK_OEM_7 en layout español
        //case 226: return "<";           // VK_OEM_102 en layout español
        default: {
            static char buf[16];
            // Letras A-Z
            if (vkCode >= 0x41 && vkCode <= 0x5A) {
                // Verificar estado de CAPS LOCK y Shift
                int capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
                int shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                
                // Si CAPS está ON y NO hay Shift, O si CAPS está OFF y SÍ hay Shift → Mayúscula
                // Si CAPS está ON y SÍ hay Shift, O si CAPS está OFF y NO hay Shift → Minúscula
                char letter = (char)vkCode;
                if (capsLockOn ^ shiftPressed) { // XOR: solo una de las dos condiciones
                    letter = letter; // Mantener mayúscula (A-Z)
                } else {
                    letter = letter + ('a' - 'A'); // Convertir a minúscula
                }
                
                buf[0] = letter;
                buf[1] = '\0';
                return buf;
            }
            // Números 0-9
            if (vkCode >= 0x30 && vkCode <= 0x39) {
                buf[0] = (char)vkCode;
                buf[1] = '\0';
                return buf;
            }
            // Si no reconoces la tecla
            sprintf_s(buf, sizeof(buf), "[VK=%lu]", vkCode);
            return buf;
        }
    }
}