# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/08 13:00:27 by vzurera-          #+#    #+#              #
#    Updated: 2025/05/11 15:02:51 by vzurera-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ───────────────────────────────────────────────────────────── #
# ─────────────────────── CONFIGURATION ─────────────────────── #
# ───────────────────────────────────────────────────────────── #

# /wd5045:		Avisos sobre funciones de inserción para mitigación especulativa
# /wd4255:		Falta de prototipos de función como func() vs func(void)
# /wd4668:		Macros no definidas en condiciones #if/#elif (reemplazadas por 0)

SRC				= src
OBJ				= build\obj
BIN				= bin

CC				= cl
FLAGS			= /nologo /Wall /WX /EHsc /Iinc /Qspectre /wd5045 /wd4255 /wd4668 /DEBUG
LIBS			= advapi32.lib user32.lib

EXE				= svc.exe

OBJS			= $(OBJ)\main.obj				\
				  $(OBJ)\service_control.obj	\
				  $(OBJ)\process_manager.obj	\
				  $(OBJ)\impersonation.obj

# ───────────────────────────────────────────────────────────── #
# ─────────────────────────── RULES ─────────────────────────── #
# ───────────────────────────────────────────────────────────── #

.SUFFIXES: .c .obj

all: $(BIN)\$(EXE)

$(BIN)\$(EXE): $(OBJS)
	@if not exist $(BIN) mkdir $(BIN)
	@$(CC) -Iinc $(FLAGS) /Fe$(BIN)\$(EXE) $(OBJS) $(LIBS)

$(OBJ)\main.obj: $(SRC)\main.c
	@if not exist $(OBJ) mkdir $(OBJ)
	@$(CC) -Iinc $(FLAGS) /c /Fo$@ $**

$(OBJ)\service_control.obj: $(SRC)\service_control.c
	@if not exist $(OBJ) mkdir $(OBJ)
	@$(CC) -Iinc $(FLAGS) /c /Fo$@ $**

$(OBJ)\process_manager.obj: $(SRC)\process_manager.c
	@if not exist $(OBJ) mkdir $(OBJ)
	@$(CC) -Iinc $(FLAGS) /c /Fo$@ $**

$(OBJ)\impersonation.obj: $(SRC)\impersonation.c
	@if not exist $(OBJ) mkdir $(OBJ)
	@$(CC) -Iinc $(FLAGS) /c /Fo$@ $**

clean:
	@if exist $(OBJ) rmdir /s /q $(OBJ)
	@if exist build rmdir /s /q build

fclean: clean
	@if exist $(BIN) rmdir /s /q $(BIN)

re: fclean all

_create_directories:
	@if not exist $(OBJ) mkdir $(OBJ)
	@if not exist $(BIN) mkdir $(BIN)

.PHONY: all svc clean fclean re
