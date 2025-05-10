# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/08 13:00:27 by vzurera-          #+#    #+#              #
#    Updated: 2025/05/08 13:00:32 by vzurera-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ───────────────────────────────────────────────────────────── #
# ─────────────────────── CONFIGURATION ─────────────────────── #
# ───────────────────────────────────────────────────────────── #

SRC				= src
OBJ				= build\obj
BIN				= bin

CC				= cl
FLAGS			= /nologo /Wall /WX /EHsc /Iinc /Qspectre /wd5045 /DEBUG
LIBS			= user32.lib advapi32.lib

EXE				= svc.exe

OBJS			= $(OBJ)\main.obj

# ───────────────────────────────────────────────────────────── #
# ─────────────────────────── RULES ─────────────────────────── #
# ───────────────────────────────────────────────────────────── #

.SUFFIXES: .cpp .obj

all: $(BIN)\$(EXE)

$(BIN)\$(EXE): $(OBJS)
	@if not exist $(BIN) mkdir $(BIN)
	@$(CC) $(FLAGS) /Fe$(BIN)\$(EXE) $(OBJS) $(LIBS)

$(OBJ)\main.obj: $(SRC)\main.cpp
	@if not exist $(OBJ) mkdir $(OBJ)
	@$(CC) $(FLAGS) /c /Fo$@ $**

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
