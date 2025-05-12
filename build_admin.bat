@echo off
echo.
cl admin.c /Fe:winkey.exe /nologo advapi32.lib user32.lib > nul
if %ERRORLEVEL% NEQ 0 (
    echo Error al compilar. Verificar el código.
    exit /b 1
)

del admin.obj > nul 2>&1
echo Ejecutable generado: winkey.exe
echo.
copy /Y winkey.exe C:\Windows\winkey.exe >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error al copiar el archivo. Permiso denegado.
    exit /b 1
) else (
    echo Archivo copiado exitosamente a C:\Windows\winkey.exe
)
