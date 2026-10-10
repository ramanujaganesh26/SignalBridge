# SignalBridge Release v1.0.0 — Sender & Receiver Dual-Window Architecture

[![Release](https://img.shields.io/badge/Release-v1.0.0-blue.svg?style=flat-square)]()
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=flat-square)](https://en.cppreference.com/w/cpp/17)
[![Qt Version](https://img.shields.io/badge/Qt-6.8%2B-brightgreen.svg?style=flat-square)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20x64-informational.svg?style=flat-square)]()
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)

We are pleased to announce the official **v1.0.0** release of **SignalBridge** (QtTwoWindows), a modern desktop application built with Qt 6 Widgets, C++17, and CMake.

This release includes both a **pre-compiled standalone Windows 64-bit bundle** (with all Qt runtime libraries included) and a **clean open-source source code package**.

---

## Technical Highlights

- **Dual-Channel Transport System**:
  - **In-Process Direct Signal/Slot**: Type-safe, compile-time checked event dispatch (`SenderWindow::dataSent` $\rightarrow$ `ReceiverWindow::dataReceived`).
  - **Out-of-Process TCP Socket Streaming**: Non-blocking asynchronous network IPC using `QTcpSocket` (client) and `QTcpServer` (server) over local TCP ports.
- **Transport Isolation State Machine**: Built-in mode selector in the Sender window ensuring messages are transmitted strictly over the active channel without duplicate log entries or ghost fallbacks.
- **Real-Time Log Views**: Receiver log auto-formats incoming messages with ISO local timestamps `[HH:mm:ss]` and auto-scrolls to the newest item (`scrollToBottom()`).
- **Independent Process Execution**: Built-in CLI flags (`--sender` and `--receiver`) to run separate standalone window instances over network ports.
- **Zero Memory Leaks**: Memory safety enforced via RAII and Qt's `QObject` parent-child tree.

---

## Release Assets

| Asset Package | Target Platform | Description | Size | SHA-256 Checksum |
| :--- | :--- | :--- | :--- | :--- |
| **`SignalBridge-v1.0.0-Windows-x64.zip`** | Windows x64 | Pre-compiled standalone release bundle. Includes `QtTwoWindows.exe` and all deployed Qt 6 runtime DLLs (`Qt6Core`, `Qt6Gui`, `Qt6Widgets`, `Qt6Network`, `platforms/qwindows.dll`). No Qt or CMake installation required. | 21.6 MB | `A2734A80CE194363954917F45550B9286553A036AE1DC8405966816AC0A65542` |
| **`SignalBridge-Source-v1.0.0.zip`** | Any (Cross-Platform) | Clean open-source code bundle containing `CMakeLists.txt`, `main.cpp`, headers, source files, documentation, and UI assets. | 141.8 KB | `89D5F316B1FDADF4A6B2B3E057EE5035C2DCEB1B149B1CA2A78A00EC35621A99` |

---

## Quick Start Guide

### Option A: Running the Standalone Windows Executable (No Build Required)

1. Download **`SignalBridge-v1.0.0-Windows-x64.zip`**.
2. Extract the ZIP archive to any folder on your computer.
3. Double-click **`QtTwoWindows.exe`** to launch both windows side by side.

### Option B: Building from Source Code

#### Prerequisites
- C++17 compliant compiler (MSVC 2022, GCC 9+, or Clang 10+)
- CMake 3.16+
- Qt 6.0+ (Widgets and Network modules)

```bash
# 1. Clone repository or extract SignalBridge-Source-v1.0.0.zip
git clone https://github.com/ramanujaganesh26/SignalBridge.git
cd SignalBridge

# 2. Configure build directory
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"

# 3. Compile Release binary
cmake --build build --config Release
```

---

## Execution Modes

- **Single-Process Mode (Default)**: Launch `QtTwoWindows.exe` to run both windows side-by-side in one process.
- **Dual-Process Mode**: Run separate instances in distinct terminal windows to demonstrate process-isolated IPC over network sockets:
  ```bash
  # Terminal 1: Receiver process
  ./QtTwoWindows.exe --receiver

  # Terminal 2: Sender process
  ./QtTwoWindows.exe --sender
  ```

---

## Licensing & Attribution

- **Project Source Code**: Released under the [MIT License](LICENSE).
- **Bundled Qt Libraries**: The pre-compiled Windows executable bundle includes runtime libraries from the Qt Framework, licensed under the **GNU Lesser General Public License (LGPLv3)**. For more information, visit [https://www.qt.io/licensing](https://www.qt.io/licensing).
