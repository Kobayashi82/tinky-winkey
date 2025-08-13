#include "winkey.h"
#include <string.h>

// Buffer estático para construcción de strings de teclas
static char keyStringBuffer[32];

// Macro para detectar teclas modificadoras 
#define IS_MOD_KEY(vk) ( \
    (vk)==VK_LSHIFT || (vk)==VK_RSHIFT || (vk)==VK_SHIFT || \
    (vk)==VK_LCONTROL || (vk)==VK_RCONTROL || (vk)==VK_CONTROL || \
    (vk)==VK_LMENU || (vk)==VK_RMENU || (vk)==VK_MENU || \
    (vk)==VK_LWIN || (vk)==VK_RWIN )

// Obtiene el prefijo de modificadores activos (sin superar el buffer)
static void GetActiveModifiers(char* dst, size_t cap)
{
    dst[0] = '\0';
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) strcat_s(dst, cap, "[CTRL]");
    if (GetAsyncKeyState(VK_MENU)    & 0x8000) strcat_s(dst, cap, "[ALT]");
    if (GetAsyncKeyState(VK_SHIFT)   & 0x8000) strcat_s(dst, cap, "[SHIFT]");
}

// Construye un keyboardState coherente para ToUnicodeEx (Shift/Ctrl/Alt/AltGr/Caps)
static void BuildKeyboardState(BYTE ks[256])
{
    if (!GetKeyboardState(ks)) memset(ks, 0, 256);
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) { ks[VK_SHIFT]|=0x80; ks[VK_LSHIFT]|=0x80; ks[VK_RSHIFT]|=0x80; } else { ks[VK_SHIFT]&=~0x80; ks[VK_LSHIFT]&=~0x80; ks[VK_RSHIFT]&=~0x80; }
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) { ks[VK_CONTROL]|=0x80; ks[VK_LCONTROL]|=0x80; ks[VK_RCONTROL]|=0x80; } else { ks[VK_CONTROL]&=~0x80; ks[VK_LCONTROL]&=~0x80; ks[VK_RCONTROL]&=~0x80; }
    if (GetAsyncKeyState(VK_MENU) & 0x8000) { ks[VK_MENU]|=0x80; ks[VK_LMENU]|=0x80; ks[VK_RMENU]|=0x80; } else { ks[VK_MENU]&=~0x80; ks[VK_LMENU]&=~0x80; ks[VK_RMENU]&=~0x80; }
    if (GetAsyncKeyState(VK_RMENU) & 0x8000) { ks[VK_RMENU]|=0x80; ks[VK_MENU]|=0x80; ks[VK_CONTROL]|=0x80; ks[VK_RCONTROL]|=0x80; }
    if (GetKeyState(VK_CAPITAL) & 0x0001) ks[VK_CAPITAL]|=0x01; else ks[VK_CAPITAL]&=~0x01;
}

// Mapea teclas especiales a etiquetas legibles
static const char* SpecialKeyToString(DWORD vk)
{
    switch (vk) {
        case VK_RETURN:  return "\n";
        case VK_BACK:    return "[BACKSPACE]";
        case VK_TAB:     return "[TAB]";
        case VK_ESCAPE:  return "[ESC]";
        case VK_CAPITAL: return "[CAPS]";
        case VK_INSERT:  return "[INSERT]";
        case VK_DELETE:  return "[DELETE]";
        case VK_HOME:    return "[HOME]";
        case VK_END:     return "[END]";
        case VK_PRIOR:   return "[PAGE UP]";
        case VK_NEXT:    return "[PAGE DOWN]";
        case VK_LEFT:    return "[LEFT]";
        case VK_RIGHT:   return "[RIGHT]";
        case VK_UP:      return "[UP]";
        case VK_DOWN:    return "[DOWN]";
        default:         return NULL;
    }
}

// Usa ToUnicodeEx para traducir VK a UTF-8 según layout activo
static const char* ToUnicodeVkToUtf8(DWORD vk, char* out, size_t outCap)
{
    BYTE ks[256]; BuildKeyboardState(ks);
    HKL lay = GetKeyboardLayout(0);
    UINT sc = MapVirtualKeyEx((UINT)vk, MAPVK_VK_TO_VSC, lay);
    WCHAR wbuf[8] = {0};
    int r = ToUnicodeEx(vk, sc, ks, wbuf, 8, 0, lay);
    if (r < 0) { WCHAR dummy[8]; ToUnicodeEx(vk, sc, ks, dummy, 8, 0, lay); out[0] = '\0'; return out; }
    if (r >= 1) {
        int n = WideCharToMultiByte(CP_UTF8, 0, wbuf, r, out, (int)outCap-1, NULL, NULL);
        if (n > 0) { unsigned char c0 = (unsigned char)out[0]; if (c0 >= 0x20 && c0 != 0x7F) { out[n] = '\0'; return out; } }
    }
    out[0] = '\0';
    return out;
}

// Convierte código de tecla virtual a string legible (UTF-8)
const char* VkCodeToString(DWORD vk)
{
    if (IS_MOD_KEY(vk)) return "";
    char mods[32]; GetActiveModifiers(mods, sizeof(mods));

    // Evitar control chars en Ctrl+letra
    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && vk >= 'A' && vk <= 'Z') {
        if (mods[0]) sprintf_s(keyStringBuffer, sizeof(keyStringBuffer), "%s%c", mods, (char)vk);
        else { keyStringBuffer[0] = (char)vk; keyStringBuffer[1] = '\0'; }
        return keyStringBuffer;
    }

    const char* sp = SpecialKeyToString(vk);
    if (sp) { if (mods[0]) { sprintf_s(keyStringBuffer, sizeof(keyStringBuffer), "%s%s", mods, sp); return keyStringBuffer; } return sp; }

    char base[32]; ToUnicodeVkToUtf8(vk, base, sizeof(base));
    if (!base[0]) return "";
    if (mods[0]) { sprintf_s(keyStringBuffer, sizeof(keyStringBuffer), "%s%s", mods, base); return keyStringBuffer; }
    strcpy_s(keyStringBuffer, sizeof(keyStringBuffer), base);
    return keyStringBuffer;
}