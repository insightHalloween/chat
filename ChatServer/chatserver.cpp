#include "chatserver.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QTimer>
#include "clientmanager.h"

ChatServer::ChatServer(QObject *parent) : QTcpServer(parent)
{
    m_clientManager = new ClientManager(this);

    connect(m_clientManager, &ClientManager::clientRejected, this, [](QTcpSocket* clientSocket, const QString& reason) {
        qDebug() << "Client rejected:" << reason;
    });

    connect(m_clientManager, &ClientManager::clientAccepted, this, [](QTcpSocket* clientSocket, const QString& username) {
        qDebug() << "Client accepted:" << username;
    });

    // 创建上传目录
    uploadDir = "uploads";
    QDir dir;
    if (!dir.exists(uploadDir)) {
        dir.mkdir(uploadDir);
    }
}

bool ChatServer::startServer()
{
    if (listen(QHostAddress::Any, 8888)) {
        qDebug() << "服务器已启动，监听端口: 8888";
        return true;
    } else {
        qDebug() << "服务器启动失败!";
        return false;
    }
}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    connect(clientSocket, &QTcpSocket::disconnected, this, &ChatServer::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::readyRead, this, &ChatServer::onClientReadyRead);

    // 等待客户端发送用户名
    clients.insert(clientSocket, "");
    m_clientManager->handleNewConnection(clientSocket);

    QString clientNameAndAddress = clients[clientSocket] + "（端口" + clientSocket->peerAddress().toString() + ":" +
                          QString::number(clientSocket->peerPort()) + "）";
    emit clientConnected(clientNameAndAddress);
    qDebug() << "新客户端连接:" << clientNameAndAddress;
}

void ChatServer::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    // 清理文件传输相关资源
    if (fileTransfers.contains(clientSocket)) {
        FileInfo &fileInfo = fileTransfers[clientSocket];
        if (fileInfo.file) {
            fileInfo.file->close();
            delete fileInfo.file;
        }
        fileTransfers.remove(clientSocket);
    }

    QString username = clients.value(clientSocket);
    QString clientAddress = username + "（端口" + clientSocket->peerAddress().toString() + ":" +
                          QString::number(clientSocket->peerPort()) + "）";
    
    if (!username.isEmpty()) {
        QString leaveMsg = username + " 已离开聊天室";
        broadcastMessage(leaveMsg, clientSocket);
    }

    clients.remove(clientSocket);
    emit clientDisconnected(clientAddress);
    clientSocket->deleteLater();
}

void ChatServer::onClientReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray data = clientSocket->readAll();
    
    // 检查是否正在接收文件
    if (fileTransfers.contains(clientSocket) && fileTransfers[clientSocket].isReceivingFile) {
        processFileData(clientSocket, data);
        return;
    }

    // 检查是否是请求文件列表
    if (data.startsWith("LIST_FILES:")) {
        sendFileList(clientSocket);
        return;
    }

    // 检查是否是文件下载请求
    if (data.startsWith("DOWNLOAD:")) {
        QString fileName = QString::fromUtf8(data.mid(9)); // 跳过"DOWNLOAD:"
        handleFileDownload(clientSocket, fileName);
        return;
    }
    
    // 检查是否是文件传输开始的标记
    if (data.startsWith("FILE:")) {
        QByteArray header = data.mid(5);  // 跳过"FILE:"标记
        QDataStream stream(header);
        stream.setVersion(QDataStream::Qt_5_15);
        
        FileInfo fileInfo;
        stream >> fileInfo.fileName >> fileInfo.fileSize;
        
        if (stream.status() == QDataStream::Ok) {
            fileInfo.bytesReceived = 0;
            fileInfo.isReceivingFile = true;
            
            // 创建文件
            QString filePath = uploadDir + "/" + fileInfo.fileName;
            fileInfo.file = new QFile(filePath);
            
            if (!fileInfo.file->open(QIODevice::WriteOnly)) {
                qDebug() << "无法创建文件:" << filePath;
                delete fileInfo.file;
                return;
            }
            
            fileTransfers[clientSocket] = fileInfo;
            
            // 广播文件开始上传的消息
            QString username = clients.value(clientSocket);
            QString message = username + " 开始上传文件 " + fileInfo.fileName;
            broadcastMessage(message);
            
            // 处理剩余的数据
            int headerSize = stream.device()->pos() + 5;  // 5是"FILE:"的长度
            if (data.size() > headerSize) {
                QByteArray fileData = data.mid(headerSize);
                processFileData(clientSocket, fileData);
            }
        }
        return;
    }

    QString message = QString::fromUtf8(data);
    QString username = clients.value(clientSocket);

    if (username.isEmpty()) {
        // 处理登录消息
        if (message.startsWith("LOGIN:")) {
            QString newUsername = message.mid(6);
            
            // 检查是否存在重名用户
            bool usernameExists = false;
            for (const QString &existingUsername : clients.values()) {
                if (existingUsername == newUsername) {
                    usernameExists = true;
                    break;
                }
            }
            
            if (usernameExists) {
                // 发送用户名重复的错误消息
                QString errorMsg = "ERROR:USERNAME_EXISTS";
                clientSocket->write(errorMsg.toUtf8());
                clientSocket->waitForBytesWritten();
                
                QTimer::singleShot(100, clientSocket, [clientSocket]() {
                    clientSocket->disconnectFromHost();
                });
                return;
            }
            
            // 用户名可用，保存并广播加入消息
            username = newUsername;
            clients[clientSocket] = username;
            QString joinMsg = username + " 加入了聊天室";
            broadcastMessage(joinMsg, clientSocket);

            QString welcomeMsg = "欢迎 " + username + " 加入聊天室！";
            clientSocket->write(welcomeMsg.toUtf8());
        }
    } else {
        // 广播普通消息
        QString formattedMessage = username + ": " + message;
        broadcastMessage(formattedMessage, nullptr);
        emit messageReceived(username, message);
    }
}

void ChatServer::processFileData(QTcpSocket *clientSocket, const QByteArray &data)
{
    if (!fileTransfers.contains(clientSocket)) return;
    
    FileInfo &fileInfo = fileTransfers[clientSocket];
    
    // 写入文件数据
    qint64 bytesWritten = fileInfo.file->write(data);
    if (bytesWritten == -1) {
        qDebug() << "写入文件失败:" << fileInfo.file->errorString();
        return;
    }
    fileInfo.bytesReceived += bytesWritten;
    
    // 检查文件是否接收完成
    if (fileInfo.bytesReceived >= fileInfo.fileSize) {
        fileInfo.file->close();
        delete fileInfo.file;
        
        // 广播文件上传完成消息
        QString username = clients.value(clientSocket);
        QString message = username + " 的文件 " + fileInfo.fileName + " 上传完成";
        broadcastMessage(message);
        
        // 清理文件传输信息
        fileTransfers.remove(clientSocket);
    }
}

void ChatServer::broadcastMessage(const QString &message, QTcpSocket *exclude)
{
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it.key() != exclude && !it.value().isEmpty()) {
            it.key()->write(message.toUtf8());
        }
    }
}

QList<QTcpSocket*> ChatServer::getClients() const
{
    return clients.keys();
}

QList<QString> ChatServer::getUsernames() const {
    return clients.values();
}

void ChatServer::disconnectClient(const QString &address)
{
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        QTcpSocket *socket = it.key();
        if (socket->peerAddress().toString() + ":" + 
            QString::number(socket->peerPort()) == address) {
            socket->disconnectFromHost();
            break;
        }
    }
}

QStringList ChatServer::getFileList() const
{
    QDir dir(uploadDir);
    return dir.entryList(QDir::Files);
}

void ChatServer::sendFileList(QTcpSocket *clientSocket)
{
    QStringList files = getFileList();
    QString fileList = "FILE_LIST:" + files.join(',');
    clientSocket->write(fileList.toUtf8());
}

void ChatServer::handleFileDownload(QTcpSocket *clientSocket, const QString &fileName)
{
    QString filePath = uploadDir + "/" + fileName;
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件:" << filePath;
        emit fileTransferFailed(clientSocket->peerAddress().toString(), fileName, "无法打开文件");
        return;
    }

    // 分块读取并发送文件
    while (!file.atEnd()) {
        QByteArray data = "FILE_DATA:" + file.read(4096);
        clientSocket->write(data);
        clientSocket->waitForBytesWritten();
    }

    file.close();

    // 发送下载完成消息
    QByteArray completeMsg = "DOWNLOAD_COMPLETE:" + fileName.toUtf8();
    clientSocket->write(completeMsg);
}
