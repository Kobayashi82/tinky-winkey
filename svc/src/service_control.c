/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service_control.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 13:31:13 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/11 16:32:30 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma region "Includes"

	#include "svc.h"

	#include <stdio.h>
	#include <string.h>

#pragma endregion

#pragma region "Install"

	int install(void) {
		printf("[*] Installing service...\n");
		printf("[+] Service '%s' installed successfully\n", Name);
		
		printf("[!] Service '%s' already exists\n", Name);

		return (0);
	}

	int delete(void) {
		printf("[*] Deleting service...\n");
		printf("[+] Service '%s' deleted\n", Name);
		
		printf("[!] Service '%s' doesn't exists\n", Name);

		return (0);
	}

	int reinstall(void) {
		delete();
		return (install());
	}

#pragma endregion

#pragma region "Start"

	int start(void) {
		printf("[*] Starting service...\n");
		printf("[+] Service '%s' started\n", Name);
		
		printf("[!] Service '%s' already started\n", Name);

		return (0);
	}

	int stop(void) {
		printf("[*] Stopping service...\n");
		printf("[+] Service '%s' stopped\n", Name);
		
		printf("[!] Service '%s' is not started\n", Name);

		return (0);
	}

	int restart(void) {
		stop();
		return (start());
	}

#pragma endregion

#pragma region "Help"

	int help(char *exe) {		
		printf("Uso: %s [comando] [opciones]\n\n", exe);
		printf("Comandos disponibles:\n\n");
		printf("  install     - Instala el servicio\n");
		printf("  delete      - Instala el servicio\n");
		printf("  reinstall   - Instala el servicio\n");
		printf("\n");
		printf("  start       - Inicia el servicio\n");
		printf("  stop        - Detiene el servicio\n");
		printf("  restart     - Inicia el servicio\n");
		printf("\n");
		printf("  status      - Muestra el estado del servicio\n");
		printf("  config      - Configura el servicio\n");
		printf("  help        - Muestra esta ayuda\n");

		return (0);
	}

#pragma endregion

#pragma region "Status"

	int status(void) {
		printf("Status of %s\n", Name);

		return (0);
	}

#pragma endregion

#pragma region "Update"

	// svc.exe update [path]
	int update(char *dst, char *src) {
		printf("Updating file %s with %s\n", dst, src);
		// cmd.exe /c timeout 2 & move /Y update.exe svc.exe & start svc.exe start

		return (0);
	}

#pragma endregion

#pragma region "Config"

	// svc.exe config start auto
	int config(char *exe, char *arg) {
		if (!strcmp(arg, "manual") || !strcmp(arg, "demand"))		;
		else if (!strcmp(arg, "auto"))								;
		else if (!strcmp(arg, "delayed-auto"))						;
		else if (!strcmp(arg, "disabled"))							;
		else 														return (help(exe), 1);

		printf("value is: 'config start %s'\n", arg);

		return (0);
	}

#pragma endregion

#pragma region "Control"

	int control(int argc, char **argv) {
		printf("\n");

		if (argc <= 1) ;
		else if (argc == 2 && !strcmp(argv[1], "install"))   													return (install());
		else if (argc == 2 && !strcmp(argv[1], "delete"))    													return (delete());
		else if (argc == 2 && !strcmp(argv[1], "reinstall")) 													return (reinstall());

		else if (argc == 2 && !strcmp(argv[1], "start"))     													return (start());
		else if (argc == 2 && !strcmp(argv[1], "stop"))      													return (stop());
		else if (argc == 2 && !strcmp(argv[1], "restart"))   													return (restart());

		else if (argc == 2 && (!strcmp(argv[1], "help") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "/?")))	return (help(argv[0]));
		else if (argc == 2 && (!strcmp(argv[1], "version") || !strcmp(argv[1], "-v")))							return (printf("%s version: %s\n", Name, Version), 0);

		else if (argc == 2 && !strcmp(argv[1], "status"))														return (status());
		else if (argc == 3 && !strcmp(argv[1], "update"))														return (update(argv[0], argv[2]));
		else if (argc == 4 && !strcmp(argv[1], "config") && !strcmp(argv[2], "start"))							return (config(argv[0], argv[3]));

		return (help(argv[0]), 1);
	}

#pragma endregion
