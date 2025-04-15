#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QDateTime>

// 文件传输信息结构体
struct FileInfo {
    QString fileName;
    qint64 fileSize;
    qint64 bytesReceived;
    QFile* file;
    bool isReceivingFile;
};

// 添加 ClientManager 的前向声明
class ClientManager;

class ChatServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit ChatServer(QObject *parent = nullptr);
    bool startServer();
    QList<QTcpSocket*> getClients() const;
    QList<QString> getUsernames() const;
    void disconnectClient(const QString &address);
    QStringList getFileList() const;

signals:
    void clientConnected(const QString &clientAddress);
    void clientDisconnected(const QString &clientAddress);
    void messageReceived(const QString &from, const QString &message);
    void fileTransferFailed(const QString &clientAddress, const QString &fileName, const QString &error);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onClientReadyRead();
    void onClientDisconnected();
    void processFileData(QTcpSocket *clientSocket, const QByteArray &data);
    void broadcastMessage(const QString &message, QTcpSocket *exclude = nullptr);
    void sendFileList(QTcpSocket *clientSocket);
    void handleFileDownload(QTcpSocket *clientSocket, const QString &fileName);

private:
    QMap<QTcpSocket*, QString> clients;  // socket -> username
    QMap<QTcpSocket*, FileInfo> fileTransfers;
    QString uploadDir;
    ClientManager* m_clientManager;
};

#endif // CHATSERVER_H
