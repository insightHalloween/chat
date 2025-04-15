#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QFile>
#include <QDir>
#include <QStringList>

class ChatServer : public QObject
{
    Q_OBJECT

public:
    explicit ChatServer(QObject *parent = nullptr);
    ~ChatServer();

public slots:
    void startServer();
    void stopServer();

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onClientReadyRead();
    void processFileData(QTcpSocket *clientSocket, const QByteArray &data);
    void sendFileList(QTcpSocket *clientSocket);
    void handleFileDownload(QTcpSocket *clientSocket, const QString &fileName);

private:
    QTcpServer *server;
    QMap<QTcpSocket*, QString> clients; // 存储客户端socket和用户名
    void broadcastMessage(const QString &message, QTcpSocket *exclude = nullptr);
    QStringList getFileList() const;

    // 文件传输相关
    struct FileInfo {
        QString fileName;
        qint64 fileSize;
        qint64 bytesReceived;
        QFile *file;
        bool isReceivingFile;
    };
    QMap<QTcpSocket*, FileInfo> fileTransfers;
    QString uploadDir;
};

#endif // CHATSERVER_H
