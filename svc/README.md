# SVC

## Comandos básicos (requeridos según el enunciado)

- svc.exe install - Instala el servicio con nombre "tinky"
- svc.exe start - Inicia el servicio
- svc.exe stop - Detiene el servicio
- svc.exe delete - Elimina el servicio

## Comandos de configuración (opcionales, más allá del enunciado)

- svc.exe config start auto - Configura el servicio para inicio automático
- svc.exe config start demand o svc.exe config start manual - Configura para inicio manual
- svc.exe config start disabled - Deshabilita el servicio
- svc.exe config start delayed-auto - Configura para inicio automático con retraso
- svc.exe config description "texto descriptivo" - Establece la descripción del servicio
- svc.exe config displayname "nombre para mostrar" - Cambia el nombre visible del servicio
- svc.exe config depend "servicio1/servicio2" - Establece dependencias de otros servicios

## Comandos de utilidad (opcionales)

- svc.exe status o svc.exe query - Muestra el estado actual del servicio
- svc.exe installstart - Instala e inicia el servicio inmediatamente
- svc.exe stopdelete - Detiene y elimina el servicio
- svc.exe reinstall - Desinstala y vuelve a instalar el servicio

## Argumentos especiales para ejecución interna

- svc.exe service - Argumento que indica a svc.exe que debe ejecutarse como un servicio (usado internamente cuando Windows inicia el servicio)

## Argumentos de ayuda

- svc.exe help o svc.exe -h o svc.exe /? - Muestra información de ayuda sobre los comandos disponibles
- svc.exe version o svc.exe -v - Muestra información de versión

## Flags adicionales (pueden combinarse con otros comandos)

- svc.exe install -s o svc.exe install --silent - Instala sin mostrar mensajes (modo silencioso)
- svc.exe config start auto -f o svc.exe config start auto --force - Fuerza el cambio incluso si hay errores no críticos
