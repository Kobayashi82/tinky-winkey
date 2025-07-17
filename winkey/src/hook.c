#include "winkey.h"

// Variable global para el hook
static HHOOK g_hHook = NULL;

// Funcion que se ejecuta cada vez que se pulsa una tecla
LRESULT CALLBACK LowLevelKeyboardProc(
    int nCode,	// Estado del hook (debe ser HC_ACTION para actuar) 
    WPARAM wParam, // Tipo de evento: tecla presionada (WM_KEYDOWN)
    LPARAM lParam) // Info sobre la tecla (estructura) 
{
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        DWORD vkCode = p->vkCode;  // Código virtual de la tecla

        printf ("Tecla pulsada: %lu\n", vkCode);

        // Si presiona ESC, termina el programa
        if (vkCode == VK_ESCAPE) {
            PostQuitMessage(0);
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Activar hook

BOOL ActivateHook(void)
{
    // Activar el hook
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hHook) {
        printf("Error to create the hook\n");
        return FALSE;
    }
    printf("Hook instalado. Pulsa algunas teclas. ESC para salir.\n");
    return TRUE;
}

void DeactivateHook(void)
{
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
        printf("Hook desactivado.\n");
    }
}