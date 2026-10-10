<div align="center">

# 🚀 QtTwoWindows (SignalBridge)

### *High-Performance Event-Driven IPC & Dual-Window Qt 6 C++ Architecture*

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?style=for-the-badge&logo=cplusplus)](https://en.cppreference.com/w/cpp/17)
[![Qt Version](https://img.shields.io/badge/Qt-6.8%2B-brightgreen.svg?style=for-the-badge&logo=qt)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-red.svg?style=for-the-badge&logo=cmake)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-Passing-success.svg?style=for-the-badge)]()

<p align="center">
  <b>A modern C++17 / Qt 6 desktop application demonstrating decoupled event-driven architecture, dual-channel Inter-Process Communication (In-Process Direct Signals vs. Out-of-Process TCP Sockets), robust RAII memory safety, and clean OOP design patterns.</b>
</p>

</div>

---

## 📖 Overview

**QtTwoWindows** (also known as *SignalBridge*) is an architectural demonstration built using **Qt 6 Widgets** and **CMake**. It illustrates how two distinct top-level application windows—a **Sender** and a **Receiver**—can transmit data across boundaries using two decoupled communication models:

1. **In-Process Signal/Slot Dispatch**: Type-safe, compile-time checked direct message passing via modern C++ pointer-to-member Qt signals (`dataSent`) and slots (`dataReceived`).
2. **Out-of-Process TCP Socket Streaming**: Non-blocking asynchronous network IPC using `QTcpSocket` (Client) and `QTcpServer` (Server) over local host ports.

---

## 🖼️ Visual Gallery

<div align="center">

### 1. Dual-Window In-Process Communication Mode
*Side-by-side view showing the Sender window with mode selection and the Receiver window with real-time log tracking.*

![Sender and Receiver Application Setup](IMG/Sender%20and%20Receiver.png)

---

### 2. Live Out-of-Process TCP Socket Streaming
*Demonstrating active client connection on port 12345 with real-time timestamped data formatting `[HH:mm:ss] Hello World`.*

![Active TCP Connection and Data Received](IMG/Received%20Data.png)

</div>

<details>
<summary><b>🔍 View Individual Component Screenshots</b></summary>

| Sender Component | Receiver Component |
| :---: | :---: |
| ![Sender UI](IMG/Sender.png) | ![Receiver UI](IMG/Receiver.png) |

</details>

---

## 🏗️ Architecture & Component Design

```
+---------------------------------------------------------------------------------+
|                                 APPLICATION MAIN                                |
|                                   (main.cpp)                                    |
+----------------------------------------+----------------------------------------+
                                         |
               +-------------------------+-------------------------+
               |                                                   |
               v                                                   v
   +-----------------------+                           +-----------------------+
   |     SenderWindow      |                           |    ReceiverWindow     |
   |   (Top-Level QWidget) |                           |  (Top-Level QWidget)  |
   +-----------+-----------+                           +-----------+-----------+
               |                                                   |
               +--- Mode 1: Direct Signal/Slot -------------------->+
               |    [QObject::connect(&sender, &dataSent, ...)]   |
               |                                                   |
               +--- Mode 2: TCP Network Socket (Port 12345) ------>+
                    [QTcpSocket -> QTcpServer Stream Parsing]
```

### Key Architectural Highlights

- **Decoupled Event Handling**: `SenderWindow` has zero direct dependencies on `ReceiverWindow`. Communication occurs strictly through Qt signals/slots or TCP network sockets.
- **Transport Mode Guarding**: An explicit transport mode state machine prevents duplicate data logging and guards against fallback transmission when network sockets are disconnected.
- **Non-Blocking Asynchronous Network Layer**: Uses Qt's event loop (`QEventLoop`) with `readyRead` signals to stream line-delimited UTF-8 network payloads without freezing the UI thread.
- **Strict Memory Safety & RAII**: Zero manual `delete` calls. All UI widgets, layouts, and socket objects leverage Qt's parent-child ownership tree (`QObject` memory management).
- **Responsive Layout System**: Dynamic UI constructed entirely with nested `QVBoxLayout` and `QHBoxLayout` layout management—eliminating hardcoded coordinates and accommodating dynamic screen DPI scaling.

---

## 🔄 Sequence Diagrams

### 1. In-Process Direct Signal/Slot Mode

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Sender as SenderWindow
    participant Main as main.cpp
    participant Receiver as ReceiverWindow

    User->>Sender: Types "Hello World" & Clicks Send / Presses Enter
    Sender->>Sender: Sanitize input (trim whitespace)
    Sender->>Sender: Check Mode == DirectSignalSlot
    Sender->>Receiver: emit dataSent("Hello World")
    Receiver->>Receiver: Format "[HH:mm:ss] Hello World"
    Receiver->>Receiver: Append to QListWidget & scrollToBottom()
    Sender->>Sender: m_inputLineEdit->clear()
```

### 2. Out-of-Process TCP Network Mode

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Client as Sender (QTcpSocket)
    participant Server as Receiver (QTcpServer)
    participant ReceiverUI as Receiver QListWidget

    User->>Client: Click "Connect" (127.0.0.1:12345)
    Client->>Server: TCP Handshake (connectToHost)
    Server-->>Client: Connection Accepted
    Server->>Server: Update status label ("Connected clients: 1")
    User->>Client: Types "Hello World" & Clicks Send
    Client->>Server: write("Hello World\n") UTF-8 payload
    Server->>Server: readyRead signal -> readLine()
    Server->>ReceiverUI: dataReceived("Hello World")
    ReceiverUI->>ReceiverUI: Append timestamped log entry
```

---

## 📦 Project Structure

```
QtTwoWindows/
├── CMakeLists.txt         # CMake 3.16+ build configuration & Qt 6 linking
├── main.cpp               # Application entry point, CLI parser, & window initialization
├── SenderWindow.h         # Sender UI declaration, signals, & QTcpSocket members
├── SenderWindow.cpp       # Input validation, mode switching, & TCP transmission
├── ReceiverWindow.h       # Receiver UI declaration, slots, & QTcpServer members
├── ReceiverWindow.cpp     # Log formatting, auto-scrolling, & socket stream parsing
├── IMG/                   # High-resolution application screenshots
└── README.md              # Project documentation
```

---

## 💻 Deep Dive: Major Implementation Highlights

### 1. Modern Function-Pointer Signal/Slot Binding ([main.cpp](file:///c:/Users/RAM/Documents/SignalBridge/main.cpp#L40-L45))

```cpp
// Direct In-Process Signal/Slot Connection using compile-time checked syntax
QObject::connect(&senderWindow, &SenderWindow::dataSent,
                 &receiverWindow, &ReceiverWindow::dataReceived);
```

### 2. Input Sanitization & Transport Mode Guarding ([SenderWindow.cpp](file:///c:/Users/RAM/Documents/SignalBridge/SenderWindow.cpp#L125-L150))

```cpp
void SenderWindow::onSendClicked()
{
    const QString text = m_inputLineEdit->text().trimmed();

    // Ignore empty or whitespace-only input
    if (text.isEmpty()) {
        return;
    }

    if (currentTransportMode() == TransportMode::DirectSignalSlot) {
        // Direct In-Process Signal/Slot Mode
        emit dataSent(text);
        m_inputLineEdit->clear();
    } else {
        // TCP Network Socket Mode
        if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
            QByteArray payload = text.toUtf8() + "\n";
            m_tcpSocket->write(payload);
            m_tcpSocket->flush();
            m_inputLineEdit->clear();
        } else {
            // Guard against disconnected TCP socket
            m_statusLabel->setText(tr("Error: Cannot send — TCP socket is not connected!"));
            m_statusLabel->setStyleSheet(QStringLiteral("color: #c0392b; font-weight: bold;"));
        }
    }
}
```

### 3. Log Formatting & Auto-Scrolling ([ReceiverWindow.cpp](file:///c:/Users/RAM/Documents/SignalBridge/ReceiverWindow.cpp#L70-L83))

```cpp
void ReceiverWindow::dataReceived(const QString &message)
{
    // Format entry as "[HH:mm:ss] <message>"
    const QString timestamp = QTime::currentTime().toString(QStringLiteral("HH:mm:ss"));
    const QString formattedEntry = QStringLiteral("[%1] %2").arg(timestamp, message);

    // Append to log and scroll to newest item
    m_logWidget->addItem(formattedEntry);
    m_logWidget->scrollToBottom();
}
```

---

## ⚙️ Build & Installation Guide

### Prerequisites

- **Compiler**: C++17 compliant compiler (MSVC 2022, GCC 9+, or Clang 10+)
- **Build System**: CMake 3.16 or higher
- **Framework**: Qt 6.0+ (`Widgets` and `Network` components)

### Building via CMake

```bash
# 1. Clone the repository
git clone https://github.com/your-username/QtTwoWindows.git
cd QtTwoWindows

# 2. Configure build directory (specify Qt prefix path if Qt is in custom directory)
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/msvc2022_64"

# 3. Build executable in Release mode
cmake --build build --config Release
```

---

## 🚀 Execution Modes

### Mode A: Dual-Window Single Process (Default)

Launch the application directly to open both windows side by side:

```bash
./build/Release/QtTwoWindows.exe
```

### Mode B: Multi-Process Independent Execution

Run two separate application instances in distinct terminal windows to demonstrate true process-isolated IPC over local network sockets:

```bash
# Terminal 1: Receiver Server Process
./build/Release/QtTwoWindows.exe --receiver

# Terminal 2: Sender Client Process
./build/Release/QtTwoWindows.exe --sender
```

---

## 🛡️ Code Quality Standards

- **Zero Compiler Warnings**: Verified under MSVC `/W4` and GCC/Clang `-Wall -Wextra -Wpedantic`.
- **Standards Compliance**: Native C++17 constructs (`std::as_const`, `QStringLiteral`, `qobject_cast`).
- **Clean Teardown**: `QApplication::setQuitOnLastWindowClosed(true)` ensures complete application termination without leaving background processes hanging.

---

## 📝 License

Distributed under the **MIT License**. See `LICENSE` for more information.
