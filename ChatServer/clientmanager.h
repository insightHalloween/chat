#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include "chatserver.h"
#include <QObject>
#include <QTcpSocket>
#include <QMap>
#include <QMessageBox>

class ClientManager : public QDialog
{
    Q_OBJECT

public:
    explicit ClientManager(ChatServer *server, QWidget *parent = nullptr);

    // 添加一个方法来处理新连接
    void handleNewConnection(QTcpSocket* clientSocket);

private slots:
    void updateClientList();
    void disconnectSelectedClient();
    void onItemSelectionChanged();  // 新增：选择变化时更新按钮状态

private:
    ChatServer *server;
    QListWidget *clientListWidget;
    QPushButton *disconnectButton;
    QMap<QString, QTcpSocket*> m_clientNames; // 存储用户名和对应的socket

signals:
    void clientRejected(QTcpSocket* clientSocket, const QString& reason);
    void clientAccepted(QTcpSocket* clientSocket, const QString& username);
};

#endif // CLIENTMANAGER_H

