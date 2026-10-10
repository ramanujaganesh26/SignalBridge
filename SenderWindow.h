#ifndef SENDERWINDOW_H
#define SENDERWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QTcpSocket>

/**
 * @brief SenderWindow class representing the data transmitter UI.
 * 
 * Allows users to input messages and transmit them either via direct Qt signals
 * (in-process) or over a TCP socket connection (network mode).
 */
class SenderWindow : public QWidget
{
    Q_OBJECT

public:
    enum class TransportMode {
        DirectSignalSlot = 0,
        TcpNetwork = 1
    };

    explicit SenderWindow(QWidget *parent = nullptr);
    ~SenderWindow() override = default;

    TransportMode currentTransportMode() const;

signals:
    /**
     * @brief Signal emitted when valid text message is submitted in Direct mode.
     * @param message The non-empty message text.
     */
    void dataSent(const QString &message);

private slots:
    void onSendClicked();
    void onConnectClicked();
    void onModeChanged(int index);
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    // Mode Selection UI
    QComboBox *m_modeComboBox{nullptr};

    // UI Controls
    QLineEdit *m_inputLineEdit{nullptr};
    QPushButton *m_sendButton{nullptr};

    // Network Extension Controls
    QLineEdit *m_hostLineEdit{nullptr};
    QSpinBox *m_portSpinBox{nullptr};
    QPushButton *m_connectButton{nullptr};
    QLabel *m_statusLabel{nullptr};

    // TCP Socket
    QTcpSocket *m_tcpSocket{nullptr};

    void setupUi();
    void updateUiState();
};

#endif // SENDERWINDOW_H
