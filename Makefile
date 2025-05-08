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

# ─────────── #
# ── FLAGS ── #
# ─────────── #

CC					= cl
FLAGS				= /nologo /Wall /WX /EHsc /Iinc /Qspectre /wd5045
LDFLAGS				= /DEBUG
LIBS                = user32.lib advapi32.lib

# ───────────────── #
# ── DIRECTORIES ── #
# ───────────────── #

SRC_DIR				= src
OBJ_DIR				= build\obj

# ────────── #
# ── NAME ── #
# ────────── #

SVC_EXE				= bin\svc.exe
WINKEY_EXE			= bin\winkey.exe

SVC_SRCS			= $(SRC_DIR)\svc\main.cpp
WINKEY_SRCS			= $(SRC_DIR)\tinky\main.cpp

SVC_OBJS			= $(OBJ_DIR)\svc\main.obj
WINKEY_OBJS			= $(OBJ_DIR)\tinky\main.obj

# ───────────────────────────────────────────────────────────── #
# ─────────────────────────── RULES ─────────────────────────── #
# ───────────────────────────────────────────────────────────── #

all: $(SVC_EXE) $(WINKEY_EXE)
	@echo.
	@echo Compilacion finalizada con exito.
	@echo.

$(SVC_EXE): $(SVC_OBJS)
	@if not exist bin mkdir bin
	@$(CC) $(FLAGS) $(LDFLAGS) /Fe$@ $** $(LIBS)

$(WINKEY_EXE): $(WINKEY_OBJS)
	@$(CC) $(FLAGS) $(LDFLAGS) /Fe$@ $** $(LIBS)

$(OBJ_DIR)\svc\main.obj: $(SRC_DIR)\svc\main.cpp
	@if not exist $(OBJ_DIR)\svc mkdir $(OBJ_DIR)\svc
	@$(CC) $(FLAGS) /c /Fo$@ $(SRC_DIR)\svc\main.cpp

$(OBJ_DIR)\tinky\main.obj: $(SRC_DIR)\tinky\main.cpp
	@if not exist $(OBJ_DIR)\tinky mkdir $(OBJ_DIR)\tinky
	@$(CC) $(FLAGS) /c /Fo$@ $(SRC_DIR)\tinky\main.cpp

re: fclean all

clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)

fclean: clean
	@if exist bin rmdir /s /q bin

.PHONY: all clean fclean re
