This is the stable release of SignalBridge v1.0.0.

## What's New

SignalBridge v1.0.0 introduces a modern Qt 6 C++ desktop architecture featuring decoupled dual-window event-driven communication (In-Process Direct Signals/Slots vs. Out-of-Process TCP Network Streaming).

> [!IMPORTANT]
> **Transport Isolation State Machine**: The Sender window features an explicit mode selector (`Direct Signal/Slot` vs `TCP Network Socket`). When Network Mode is selected and disconnected, transmission is safely blocked with a status indicator to prevent duplicate logs or ghost signal emissions.

---

## Changes in v1.0.0

- `feat(SenderWindow)`: add transport mode selector dropdown (Direct Signal/Slot vs TCP Network Socket)
- `feat(SenderWindow)`: input validation and trimming to ignore empty or whitespace-only messages
- `feat(SenderWindow)`: embedded `QTcpSocket` client with connection state tracking and error handling
- `feat(ReceiverWindow)`: real-time `[HH:mm:ss]` local timestamping and auto-scrolling log view (`scrollToBottom()`)
- `feat(ReceiverWindow)`: embedded asynchronous `QTcpServer` listening on customizable port (default `12345`)
- `feat(CLI)`: add `--sender` and `--receiver` command-line flags for independent multi-process execution
- `fix(Transport)`: isolate direct signal/slot from TCP socket stream to eliminate double-emission and duplicate logging
- `fix(Warnings)`: resolve MSVC `/W4` and GCC `-Wall -Wextra` warnings (`std::as_const` and variable scope cleanup)
- `chore(Build)`: CMake 3.16+ project setup targeting Qt 6 `Widgets` and `Network` components
- `docs(README)`: comprehensive technical architecture documentation with Mermaid data pipeline diagrams

---

## Installation

### Windows 64-bit (Standalone Bundle)
Download `SignalBridge-v1.0.0-Windows-x64.zip`, extract the archive, and launch `QtTwoWindows.exe`. All required Qt 6 runtime libraries (`Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6Network.dll`, `platforms/qwindows.dll`) are bundled. No Qt or CMake installation is required.

### Build from Source Code
Download `SignalBridge-Source-v1.0.0.zip` or clone the repository. Configure and build using CMake 3.16+ and Qt 6.0+:
```bash
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"
cmake --build build --config Release
```

For full system requirements and compilation options, see the [README](https://github.com/ramanujaganesh26/SignalBridge#readme).

---

## Reporting Issues

Please report any bugs or feature requests on [GitHub Issues](https://github.com/ramanujaganesh26/SignalBridge/issues).
