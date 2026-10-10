#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "SenderWindow.h"
#include "ReceiverWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("QtTwoWindows"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));

    // Requirement: Closing windows should quit app cleanly when all are closed
    QApplication::setQuitOnLastWindowClosed(true);

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Qt 6 Dual Window Inter-Process & Signal Communication Demo"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption senderOnlyOption(QStringList() << QStringLiteral("sender"), QObject::tr("Launch Sender window only."));
    QCommandLineOption receiverOnlyOption(QStringList() << QStringLiteral("receiver"), QObject::tr("Launch Receiver window only."));
    parser.addOption(senderOnlyOption);
    parser.addOption(receiverOnlyOption);

    parser.process(app);

    bool isSenderOnly = parser.isSet(senderOnlyOption);
    bool isReceiverOnly = parser.isSet(receiverOnlyOption);

    if (isSenderOnly && !isReceiverOnly) {
        // Launch standalone Sender process
        auto *senderWindow = new SenderWindow();
        senderWindow->setAttribute(Qt::WA_DeleteOnClose);
        senderWindow->show();
        return app.exec();
    } else if (isReceiverOnly && !isSenderOnly) {
        // Launch standalone Receiver process
        auto *receiverWindow = new ReceiverWindow();
        receiverWindow->setAttribute(Qt::WA_DeleteOnClose);
        receiverWindow->show();
        return app.exec();
    }

    // Default mode: Create both windows on the stack
    SenderWindow senderWindow;
    ReceiverWindow receiverWindow;

    // Requirement: Connect SenderWindow::dataSent to ReceiverWindow::dataReceived
    // using modern function-pointer syntax.
    // Signal/Slot Flow: senderWindow.dataSent(QString) -> receiverWindow.dataReceived(QString)
    QObject::connect(&senderWindow, &SenderWindow::dataSent,
                     &receiverWindow, &ReceiverWindow::dataReceived);

    // Show both top-level windows
    senderWindow.show();
    receiverWindow.show();

    // Requirement: Position the windows side by side
    const int startX = 150;
    const int startY = 150;
    const int gap = 30;

    senderWindow.move(startX, startY);
    receiverWindow.move(startX + senderWindow.width() + gap, startY);

    return app.exec();
}
