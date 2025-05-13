/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   svc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   */
/*   Created: 2025/05/11 13:21:38 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/11 13:21:38 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma region "Includes"

	#include <windows.h>

#pragma endregion

#pragma region "Variables"

	#define Name	"Tinky"
	#define Version	"1.0.0"

#pragma endregion

#pragma region "Methods"

	// Service Manager
	void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv);

	// Service Control
	int control(int argc, char **argv);

	BOOL ImpersonateSystemToken();

#pragma endregion
