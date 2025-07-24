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

#pragma endregion

#pragma region "Variables"

	#define NAME		"Winkey"
	#define VERSION		"1.0.0"

#pragma endregion

#pragma region "Methods"

	// llamadas desde el main
	BOOL IsAdmin(void);
	BOOL ActivateHook(void);
	void DeactivateHook(void);

	// llamada desde hook.c
	char* VirtualKeyToChar(DWORD vkCode, DWORD scanCode);
	

#pragma endregion
