#include "ReceiverWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTime>
#include <utility>

ReceiverWindow::ReceiverWindow(QWidget *parent)
    : QWidget(parent)
    , m_tcpServer(new QTcpServer(this))
{
    setWindowTitle(tr("Receiver"));
    resize(450, 350);

    setupUi();

    // Connect TCP Server new connection signal
    connect(m_tcpServer, &QTcpServer::newConnection, this, &ReceiverWindow::onNewConnection);

    // Auto-start listening on default port
    startServer();
}

void ReceiverWindow::setupUi()
{
    // Main layout for the Receiver window
    auto *mainLayout = new QVBoxLayout(this);

    // --- Received Messages Log Group ---
    auto *logGroup = new QGroupBox(tr("Received Messages Log"), this);
    auto *logLayout = new QVBoxLayout(logGroup);

    m_logWidget = new QListWidget(this);

    auto *buttonLayout = new QHBoxLayout();
    m_clearButton = new QPushButton(tr("Clear"), this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_clearButton);

    logLayout->addWidget(m_logWidget);
    logLayout->addLayout(buttonLayout);

    mainLayout->addWidget(logGroup);

    // Signal/Slot Flow: Clear button click -> clears the m_logWidget
    connect(m_clearButton, &QPushButton::clicked, this, &ReceiverWindow::onClearClicked);

    // --- Network Communication Group (Optional TCP Extension) ---
    auto *netGroup = new QGroupBox(tr("Network Server (TCP)"), this);
    auto *netLayout = new QVBoxLayout(netGroup);

    auto *controlLayout = new QHBoxLayout();
    auto *portLabel = new QLabel(tr("Port:"), this);
    m_portSpinBox = new QSpinBox(this);
    m_portSpinBox->setRange(1024, 65535);
    m_portSpinBox->setValue(12345);

    m_listenButton = new QPushButton(tr("Stop Listening"), this);

    controlLayout->addWidget(portLabel);
    controlLayout->addWidget(m_portSpinBox);
    controlLayout->addWidget(m_listenButton);

    m_statusLabel = new QLabel(tr("Status: Initializing..."), this);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #7f8c8d; font-weight: bold;"));

    netLayout->addLayout(controlLayout);
    netLayout->addWidget(m_statusLabel);

    mainLayout->addWidget(netGroup);

    connect(m_listenButton, &QPushButton::clicked, this, &ReceiverWindow::onToggleServerClicked);
}

void ReceiverWindow::dataReceived(const QString &message)
{
    // Requirement: Format each entry as "[HH:mm:ss] <data>"
    const QString timestamp = QTime::currentTime().toString(QStringLiteral("HH:mm:ss"));
    const QString formattedEntry = QStringLiteral("[%1] %2").arg(timestamp, message);

    // Add entry to log list
    m_logWidget->addItem(formattedEntry);

    // Requirement: Auto-scroll to the newest entry
    m_logWidget->scrollToBottom();
}

void ReceiverWindow::onClearClicked()
{
    m_logWidget->clear();
}

void ReceiverWindow::onToggleServerClicked()
{
    if (m_tcpServer->isListening()) {
        stopServer();
    } else {
        startServer();
    }
}

void ReceiverWindow::startServer()
{
    const quint16 port = static_cast<quint16>(m_portSpinBox->value());
    if (m_tcpServer->listen(QHostAddress::Any, port)) {
        m_statusLabel->setText(tr("Status: Listening on port %1").arg(port));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #27ae60; font-weight: bold;"));
        m_listenButton->setText(tr("Stop Listening"));
        m_portSpinBox->setEnabled(false);
    } else {
        m_statusLabel->setText(tr("Error starting server: %1").arg(m_tcpServer->errorString()));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #c0392b; font-weight: bold;"));
        m_listenButton->setText(tr("Start Listening"));
        m_portSpinBox->setEnabled(true);
    }
}

void ReceiverWindow::stopServer()
{
    for (QTcpSocket *client : std::as_const(m_clients)) {
        client->disconnectFromHost();
    }
    m_clients.clear();

    m_tcpServer->close();
    m_statusLabel->setText(tr("Status: Server Stopped"));
    m_statusLabel->setStyleSheet(QStringLiteral("color: #7f8c8d; font-weight: bold;"));
    m_listenButton->setText(tr("Start Listening"));
    m_portSpinBox->setEnabled(true);
}

void ReceiverWindow::onNewConnection()
{
    while (m_tcpServer->hasPendingConnections()) {
        QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();
        m_clients.append(clientSocket);

        connect(clientSocket, &QTcpSocket::readyRead, this, &ReceiverWindow::onClientReadyRead);
        connect(clientSocket, &QTcpSocket::disconnected, this, &ReceiverWindow::onClientDisconnected);

        m_statusLabel->setText(tr("Status: Listening on port %1 (Connected clients: %2)")
                                   .arg(m_tcpServer->serverPort())
                                   .arg(m_clients.size()));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #27ae60; font-weight: bold;"));
    }
}

void ReceiverWindow::onClientReadyRead()
{
    auto *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket) {
        return;
    }

    while (clientSocket->canReadLine()) {
        QByteArray lineData = clientSocket->readLine().trimmed();
        if (!lineData.isEmpty()) {
            QString message = QString::fromUtf8(lineData);
            // Route incoming network data to public slot
            dataReceived(message);
        }
    }
}

void ReceiverWindow::onClientDisconnected()
{
    auto *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        m_clients.removeOne(clientSocket);
        clientSocket->deleteLater();
    }

    if (m_tcpServer->isListening()) {
        m_statusLabel->setText(tr("Status: Listening on port %1 (Connected clients: %2)")
                                   .arg(m_tcpServer->serverPort())
                                   .arg(m_clients.size()));
    }
}
