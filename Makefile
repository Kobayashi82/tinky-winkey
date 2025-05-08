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

# Verificar si el entorno de Visual Studio está configurado
!IF "$(VSCMD_ARG_HOST_ARCH)" == ""
# No estamos en un entorno de VS, configurar el entorno y reiniciar nmake
BOOTSTRAPPED=0
!ELSE
# Ya estamos en un entorno de VS
BOOTSTRAPPED=1
!ENDIF

!IF $(BOOTSTRAPPED) == 0
.PHONY: bootstrap
bootstrap:
	@echo Configurando el entorno de Visual Studio...
	@for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do \
	(@call "%%i\Common7\Tools\VsDevCmd.bat" -arch=x86 && nmake /nologo BOOTSTRAPPED=1 %*)
	@exit

# Redireccionar todos los objetivos a bootstrap si no estamos en un entorno VS
all clean fclean re: bootstrap

!ELSE

# ──────────── #
# ── COLORS ── #
# ──────────── #

RED     			= \033[0;31m
GREEN   			= \033[0;32m
YELLOW  			= \033[0;33m
BLUE    			= \033[0;34m
MAGENTA 			= \033[0;35m
CYAN    			= \033[0;36m
WHITE   			= \033[0;37m
NC    				= \033[0m
INV_RED  			= \033[7;31m
INV_GREEN	  		= \033[7;32m
INV_YELLOW  		= \033[7;33m
INV_BLUE  			= \033[7;34m
INV_MAGENTA			= \033[7;35m
INV_CYAN			= \033[7;36m
INV_WHITE			= \033[7;37m
BG_CYAN				= \033[40m
FG_YELLOW			= \033[89m
COUNTER 			= 0

# ─────────── #
# ── FLAGS ── #
# ─────────── #

CC					= cl
FLAGS				= /Wall /WX /EHsc /Iinc /Qspectre /wd5045
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

# Fix for NMAKE pattern substitution
SVC_OBJS			= $(OBJ_DIR)\svc\main.obj
WINKEY_OBJS			= $(OBJ_DIR)\tinky\main.obj

# ───────────────────────────────────────────────────────────── #
# ────────────────────────── NORMAL ─────────────────────────── #
# ───────────────────────────────────────────────────────────── #

all: $(SVC_EXE) $(WINKEY_EXE)
	@echo.
	@echo Compilacion finalizada con exito.
	@echo.

$(SVC_EXE): $(SVC_OBJS)
	@if not exist bin mkdir bin
	$(CC) $(FLAGS) $(LDFLAGS) /Fe$@ $** $(LIBS)

$(WINKEY_EXE): $(WINKEY_OBJS)
	$(CC) $(FLAGS) $(LDFLAGS) /Fe$@ $** $(LIBS)

# Fix for NMAKE - specific rules instead of pattern rule
$(OBJ_DIR)\svc\main.obj: $(SRC_DIR)\svc\main.cpp
	@if not exist $(OBJ_DIR)\svc mkdir $(OBJ_DIR)\svc
	$(CC) $(FLAGS) /c /Fo$@ $(SRC_DIR)\svc\main.cpp

$(OBJ_DIR)\tinky\main.obj: $(SRC_DIR)\tinky\main.cpp
	@if not exist $(OBJ_DIR)\tinky mkdir $(OBJ_DIR)\tinky
	$(CC) $(FLAGS) /c /Fo$@ $(SRC_DIR)\tinky\main.cpp

# ───────────────────────────────────────────────────────────── #
# ────────────────────────── RE-MAKE ────────────────────────── #
# ───────────────────────────────────────────────────────────── #

re: fclean all

# ───────────────────────────────────────────────────────────── #
# ─────────────────────────── CLEAN ─────────────────────────── #
# ───────────────────────────────────────────────────────────── #

clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)

# ───────────────────────────────────────────────────────────── #
# ────────────────────────── F-CLEAN ────────────────────────── #
# ───────────────────────────────────────────────────────────── #

fclean: clean
	@if exist bin rmdir /s /q bin

# ───────────────────────────────────────────────────────────── #
# ───────────────────────── FUNCTIONS ───────────────────────── #
# ───────────────────────────────────────────────────────────── #

# ─────────── #
# ── PHONY ── #
# ─────────── #

.PHONY: all clean fclean re

!ENDIF
