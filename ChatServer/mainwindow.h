#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "chatserver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleClientConnected(const QString &clientAddress);
    void handleClientDisconnected(const QString &clientAddress);
    void handleMessageReceived(const QString &from, const QString &message);
    void handleStopServer();
    void openClientManager();

private:
    void appendLog(const QString &log);
    void updateClientCount();

    Ui::MainWindow *ui;
    ChatServer *server;
};
#endif // MAINWINDOW_H
