#include "clientmanager.h"
#include <QDataStream>
#include <QCoreApplication>

ClientManager::ClientManager(ChatServer *server, QWidget *parent)
    : QDialog(parent), server(server)
{
    setWindowTitle("客户端管理");
    resize(400, 300);

    clientListWidget = new QListWidget(this);
    clientListWidget->setGeometry(10, 10, 380, 240);
    clientListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection); // 允许多选

    disconnectButton = new QPushButton("断开选中的连接", this);
    disconnectButton->setGeometry(130, 260, 140, 30);
    disconnectButton->setEnabled(false); // 初始状态禁用

    connect(disconnectButton, &QPushButton::clicked, this, &ClientManager::disconnectSelectedClient);
    connect(clientListWidget, &QListWidget::itemSelectionChanged, this, &ClientManager::onItemSelectionChanged);

    updateClientList();
}

void ClientManager::updateClientList()
{
    clientListWidget->clear();
    const auto clients = server->getClients();
    const auto names = server->getUsernames();
    int idx = 0;
    for (const auto &client : clients) {
        QString displayText = names[idx] + "（" + client->peerAddress().toString() + ":" + 
                            QString::number(client->peerPort()) + "）";
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, client->peerAddress().toString() + ":" + 
                     QString::number(client->peerPort())); // 存储地址信息
        clientListWidget->addItem(item);
        idx++;
    }
}

void ClientManager::onItemSelectionChanged()
{
    // 根据是否有选中项来启用或禁用断开连接按钮
    disconnectButton->setEnabled(!clientListWidget->selectedItems().isEmpty());
}

void ClientManager::disconnectSelectedClient()
{
    QList<QListWidgetItem*> selectedItems = clientListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }

    // 确认对话框
    QString confirmMessage = QString("确定要断开 %1 个选中的连接吗？").arg(selectedItems.size());
    if (QMessageBox::question(this, "断开连接确认", confirmMessage) != QMessageBox::Yes) {
        return;
    }

    // 断开所有选中的连接
    for (QListWidgetItem* item : selectedItems) {
        QString address = item->data(Qt::UserRole).toString();
        server->disconnectClient(address);
    }

    // 更新列表
    updateClientList();
}

void ClientManager::handleNewConnection(QTcpSocket* clientSocket) {
    if (!clientSocket) {
        return;
    }

    bool connectionHandled = false;

    connect(clientSocket, &QTcpSocket::readyRead, [this, clientSocket, &connectionHandled]() {
        // 确保这是新连接的第一次数据接收
        if (m_clientNames.values().contains(clientSocket)) {
            connectionHandled = true;
            return;
        }

        QByteArray data = clientSocket->readAll();
        QDataStream stream(data);
        QString username;
        stream >> username;

        // 检查用户名是否已存在
        if (m_clientNames.contains(username)) {
            // 发送拒绝连接信号
            emit clientRejected(clientSocket, "Username already exists");
            
            // 关闭连接
            clientSocket->disconnectFromHost();
            clientSocket->deleteLater();
            connectionHandled = true;
            return;
        }

        // 用户名可用，保存用户信息
        m_clientNames.insert(username, clientSocket);

        // 发送接受连接信号
        emit clientAccepted(clientSocket, username);
        connectionHandled = true;
    });

    // 等待连接处理完成
    while (!connectionHandled && clientSocket->state() == QAbstractSocket::ConnectedState) {
        QCoreApplication::processEvents();
    }
}

