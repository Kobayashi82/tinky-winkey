/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/12 20:59:14 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

    #include "svc.h"

	#include <stdio.h>

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		// Executed as a service
		if (StartServiceCtrlDispatcher((SERVICE_TABLE_ENTRY[]) {{ Name, (LPSERVICE_MAIN_FUNCTION)ServiceMain }, { NULL, NULL }})) return (0);

		// Executed as a process, check if is admin
		DWORD dwSize; HANDLE hToken; TOKEN_ELEVATION elevation = {0};
		if (!(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)
			&& GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)
			&& (CloseHandle(hToken), elevation.TokenIsElevated)))
			return (printf("\nSe requieren permisos de administrador\n"), 1);

		// Process actions
		return (control(argc, argv));
	}

#pragma endregion
