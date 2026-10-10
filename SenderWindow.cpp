#include "SenderWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHostAddress>

SenderWindow::SenderWindow(QWidget *parent)
    : QWidget(parent)
    , m_tcpSocket(new QTcpSocket(this))
{
    setWindowTitle(tr("Sender"));
    resize(420, 300);

    setupUi();

    // Signal/Slot connections for TCP Socket state
    connect(m_tcpSocket, &QTcpSocket::connected, this, &SenderWindow::onSocketConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &SenderWindow::onSocketDisconnected);
    connect(m_tcpSocket, &QAbstractSocket::errorOccurred, this, &SenderWindow::onSocketError);

    updateUiState();
}

void SenderWindow::setupUi()
{
    // Main layout for the Sender window
    auto *mainLayout = new QVBoxLayout(this);

    // --- Communication Mode Selector ---
    auto *modeGroup = new QGroupBox(tr("Communication Mode"), this);
    auto *modeLayout = new QHBoxLayout(modeGroup);
    auto *modeLabel = new QLabel(tr("Mode:"), this);

    m_modeComboBox = new QComboBox(this);
    m_modeComboBox->addItem(tr("Direct Signal / Slot (In-Process)"), static_cast<int>(TransportMode::DirectSignalSlot));
    m_modeComboBox->addItem(tr("TCP Network Socket"), static_cast<int>(TransportMode::TcpNetwork));

    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(m_modeComboBox);
    mainLayout->addWidget(modeGroup);

    connect(m_modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SenderWindow::onModeChanged);

    // --- Message Input Group ---
    auto *sendGroup = new QGroupBox(tr("Message Input"), this);
    auto *sendLayout = new QHBoxLayout(sendGroup);

    m_inputLineEdit = new QLineEdit(this);
    m_inputLineEdit->setPlaceholderText(tr("Type message to send..."));

    m_sendButton = new QPushButton(tr("Send"), this);

    sendLayout->addWidget(m_inputLineEdit);
    sendLayout->addWidget(m_sendButton);

    mainLayout->addWidget(sendGroup);

    // Connect button click AND Enter key (returnPressed) to send handler
    connect(m_sendButton, &QPushButton::clicked, this, &SenderWindow::onSendClicked);
    connect(m_inputLineEdit, &QLineEdit::returnPressed, this, &SenderWindow::onSendClicked);

    // --- Network Communication Group (Optional TCP Extension) ---
    auto *netGroup = new QGroupBox(tr("Network Connection (TCP)"), this);
    auto *netLayout = new QVBoxLayout(netGroup);

    auto *addressLayout = new QHBoxLayout();
    auto *hostLabel = new QLabel(tr("Host:"), this);
    m_hostLineEdit = new QLineEdit(QStringLiteral("127.0.0.1"), this);
    
    auto *portLabel = new QLabel(tr("Port:"), this);
    m_portSpinBox = new QSpinBox(this);
    m_portSpinBox->setRange(1024, 65535);
    m_portSpinBox->setValue(12345);

    m_connectButton = new QPushButton(tr("Connect"), this);

    addressLayout->addWidget(hostLabel);
    addressLayout->addWidget(m_hostLineEdit);
    addressLayout->addWidget(portLabel);
    addressLayout->addWidget(m_portSpinBox);
    addressLayout->addWidget(m_connectButton);

    m_statusLabel = new QLabel(tr("Status: Disconnected"), this);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #7f8c8d; font-weight: bold;"));

    netLayout->addLayout(addressLayout);
    netLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(netGroup);
    mainLayout->addStretch();

    connect(m_connectButton, &QPushButton::clicked, this, &SenderWindow::onConnectClicked);
}

SenderWindow::TransportMode SenderWindow::currentTransportMode() const
{
    return static_cast<TransportMode>(m_modeComboBox->currentData().toInt());
}

void SenderWindow::onModeChanged(int index)
{
    Q_UNUSED(index);
    updateUiState();
}

void SenderWindow::updateUiState()
{
    const bool isTcpMode = (currentTransportMode() == TransportMode::TcpNetwork);
    m_hostLineEdit->setEnabled(isTcpMode);
    m_portSpinBox->setEnabled(isTcpMode);
    m_connectButton->setEnabled(isTcpMode);

    if (!isTcpMode) {
        m_statusLabel->setText(tr("Status: Using Direct Signal/Slot"));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #2980b9; font-weight: bold;"));
    } else {
        if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
            onSocketConnected();
        } else if (m_tcpSocket->state() == QAbstractSocket::ConnectingState) {
            m_statusLabel->setText(tr("Status: Connecting..."));
            m_statusLabel->setStyleSheet(QStringLiteral("color: #e67e22; font-weight: bold;"));
        } else {
            onSocketDisconnected();
        }
    }
}

void SenderWindow::onSendClicked()
{
    const QString text = m_inputLineEdit->text().trimmed();

    // Requirement: Ignore empty or whitespace-only input
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
            // TCP socket is NOT connected: Do NOT send, display clear warning!
            m_statusLabel->setText(tr("Error: Cannot send — TCP socket is not connected!"));
            m_statusLabel->setStyleSheet(QStringLiteral("color: #c0392b; font-weight: bold;"));
        }
    }
}

void SenderWindow::onConnectClicked()
{
    // Auto-switch mode to TCP Network mode when Connect is clicked
    int tcpIndex = m_modeComboBox->findData(static_cast<int>(TransportMode::TcpNetwork));
    if (tcpIndex != -1 && m_modeComboBox->currentIndex() != tcpIndex) {
        m_modeComboBox->setCurrentIndex(tcpIndex);
    }

    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState ||
        m_tcpSocket->state() == QAbstractSocket::ConnectingState) {
        m_tcpSocket->disconnectFromHost();
    } else {
        m_statusLabel->setText(tr("Status: Connecting..."));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #e67e22; font-weight: bold;"));
        m_tcpSocket->connectToHost(m_hostLineEdit->text().trimmed(), static_cast<quint16>(m_portSpinBox->value()));
    }
}

void SenderWindow::onSocketConnected()
{
    if (currentTransportMode() == TransportMode::TcpNetwork) {
        m_statusLabel->setText(tr("Status: Connected to %1:%2")
                                   .arg(m_tcpSocket->peerAddress().toString())
                                   .arg(m_tcpSocket->peerPort()));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #27ae60; font-weight: bold;"));
    }
    m_connectButton->setText(tr("Disconnect"));
}

void SenderWindow::onSocketDisconnected()
{
    if (currentTransportMode() == TransportMode::TcpNetwork) {
        m_statusLabel->setText(tr("Status: Disconnected"));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #7f8c8d; font-weight: bold;"));
    }
    m_connectButton->setText(tr("Connect"));
}

void SenderWindow::onSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    if (currentTransportMode() == TransportMode::TcpNetwork) {
        m_statusLabel->setText(tr("Error: %1").arg(m_tcpSocket->errorString()));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #c0392b; font-weight: bold;"));
    }
    m_connectButton->setText(tr("Connect"));
}
