#ifndef RECEIVERWINDOW_H
#define RECEIVERWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

/**
 * @brief ReceiverWindow class representing the data receiver UI.
 * 
 * Displays incoming data in a QListWidget with timestamps. Receives data either
 * via direct Qt slot invocation or through an embedded TCP server.
 */
class ReceiverWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiverWindow(QWidget *parent = nullptr);
    ~ReceiverWindow() override = default;

public slots:
    /**
     * @brief Public slot to handle incoming data messages.
     * @param message The non-empty message payload received.
     */
    void dataReceived(const QString &message);

private slots:
    void onClearClicked();
    void onToggleServerClicked();
    void onNewConnection();
    void onClientReadyRead();
    void onClientDisconnected();

private:
    // UI Controls
    QListWidget *m_logWidget{nullptr};
    QPushButton *m_clearButton{nullptr};

    // Network Extension Controls
    QSpinBox *m_portSpinBox{nullptr};
    QPushButton *m_listenButton{nullptr};
    QLabel *m_statusLabel{nullptr};

    // TCP Server & Client Connections
    QTcpServer *m_tcpServer{nullptr};
    QList<QTcpSocket *> m_clients;

    void setupUi();
    void startServer();
    void stopServer();
};

#endif // RECEIVERWINDOW_H
