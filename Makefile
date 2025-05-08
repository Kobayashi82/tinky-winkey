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
FLAGS				= /nologo /Wall /WX /EHsc /Iinc /Qspectre /wd5045 /DEBUG
LIBS                = user32.lib advapi32.lib

# ───────────────── #
# ── DIRECTORIES ── #
# ───────────────── #

SRC					= src
OBJ					= build
BIN					= bin

# ───────── #
# ── SVC ── #
# ───────── #

SVC_EXE				= $(BIN)\svc.exe

SVC_SRCS			= $(SRC)\svc\main.cpp

SVC_OBJS			= $(OBJ)\svc\main.obj

# ─────────── #
# ── TINKY ── #
# ─────────── #

TINKY_EXE			= $(BIN)\tinky.exe

TINKY_SRCS			= $(SRC)\tinky\main.cpp

TINKY_OBJS			= $(OBJ)\tinky\main.obj

# ──────────── #
# ── WINKEY ── #
# ──────────── #

WINKEY_EXE			= $(BIN)\winkey.exe

WINKEY_SRCS			= $(SRC)\winkey\main.cpp

WINKEY_OBJS			= $(OBJ)\winkey\main.obj

# ───────────────────────────────────────────────────────────── #
# ─────────────────────────── RULES ─────────────────────────── #
# ───────────────────────────────────────────────────────────── #

.SUFFIXES: .cpp .obj

all: _create_directories svc tinky winkey
svc: _create_directories $(SVC_EXE)
tinky: _create_directories $(TINKY_EXE)
winkey: _create_directories $(WINKEY_EXE)

_create_directories:
	@if not exist $(OBJ) mkdir $(OBJ)
	@if not exist $(OBJ)\svc mkdir $(OBJ)\svc
	@if not exist $(OBJ)\tinky mkdir $(OBJ)\tinky
	@if not exist $(OBJ)\winkey mkdir $(OBJ)\winkey
	@if not exist $(BIN) mkdir $(BIN)

$(SVC_EXE): $(SVC_OBJS)
	@$(CC) $(FLAGS) /Fe$@ $** $(LIBS)

$(TINKY_EXE): $(TINKY_OBJS)
	@$(CC) $(FLAGS) /Fe$@ $** $(LIBS)

$(WINKEY_EXE): $(WINKEY_OBJS)
	@$(CC) $(FLAGS) /Fe$@ $** $(LIBS)

{$(SRC)\svc\}.cpp{$(OBJ)\svc\}.obj:
	@$(CC) $(FLAGS) /c /Fo$@ $<

{$(SRC)\tinky\}.cpp{$(OBJ)\tinky\}.obj:
	@$(CC) $(FLAGS) /c /Fo$@ $<

{$(SRC)\winkey\}.cpp{$(OBJ)\winkey\}.obj:
	@$(CC) $(FLAGS) /c /Fo$@ $<

clean:
	@if exist $(OBJ) rmdir /s /q $(OBJ)

fclean: clean
	@if exist $(BIN) rmdir /s /q $(BIN)

re: fclean all

.PHONY: all svc tinky winkey clean fclean re _create_directories
