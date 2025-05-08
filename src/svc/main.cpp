/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 13:00:34 by vzurera-          #+#    #+#             */
/*   Updated: 2025/05/08 13:00:35 by vzurera-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Funciona sip\n");
    
    if (argc > 1) {
        printf("Argumentos pasados:\n");
        for (int i = 1; i < argc; i++) {
            printf("  Argumento %d: %s\n", i, argv[i]);
        }
    } else {
        printf("No se han pasado argumentos\n");
    }
    
    return 0;
}
