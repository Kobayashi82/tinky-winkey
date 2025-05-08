# Tinky Winkey

## Windows Service and Keylogger Implementation

This project implements a Windows service called "tinky" and a keylogger application called "winkey". It's an educational project designed to understand Windows service architecture, keyboard hooking mechanisms, and system process interaction.

> **IMPORTANT**: This project is for educational purposes only. The code in this repository should only be used in controlled environments, such as virtual machines. Using keyloggers without proper authorization is illegal and unethical.

## Project Overview

The project consists of two main components:

### 1. Windows Service (svc.exe)

A Windows service application with the following capabilities:
- Installation and registration as a system service
- Starting and stopping the service
- Service removal
- Impersonation of SYSTEM token
- Management of the keylogger process

### 2. Keylogger Application (winkey.exe)

A keylogging application that:
- Detects foreground processes
- Captures keystrokes through low-level keyboard hooks
- Records timestamps and process information
- Saves captured data in a human-readable format
- Supports current locale identifiers

## Technical Requirements

- **Platform**: Windows 10/11
- **Languages**: C/C++
- **Build tools**: NMAKE and CL compiler
- **Compilation flags**: /Wall and /WX
- **Required Windows API functions**:
  - OpenSCManager
  - CreateService
  - OpenService
  - StartService
  - ControlService
  - CloseServiceHandle
  - DuplicateTokenEx

## Usage

### Building the project

```
nmake
```

### Service Management

```
# Install the service
svc.exe install

# Start the service
svc.exe start

# Stop the service
svc.exe stop

# Delete the service
svc.exe delete
```

### Output

The keylogger generates a log file (winkey.log) with the following format:
```
[TIMESTAMP] - 'FOREGROUND PROCESS'
KEYSTROKE_DATA
```

## Development Environment

- Visual Studio Code with C/C++ extensions
- Windows SDK
- Git

## Security Notice

This project is developed purely for educational purposes to understand Windows system programming concepts. The implementation should only be used in isolated environments like virtual machines.
