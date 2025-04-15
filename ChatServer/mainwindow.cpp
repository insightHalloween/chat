#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chatserver.h"
#include <QFile>
#include <QDateTime>
#include "clientmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(new ChatServer(this))
{
    ui->setupUi(this);
    setWindowTitle("服务端");

    // Apply QSS styles
    QFile styleFile(":/styles.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        this->setStyleSheet(styleSheet);
    }

    // 连接信号和槽
    connect(server, &ChatServer::clientConnected, this, &MainWindow::handleClientConnected);
    connect(server, &ChatServer::clientDisconnected, this, &MainWindow::handleClientDisconnected);
    connect(server, &ChatServer::messageReceived, this, &MainWindow::handleMessageReceived);
    connect(ui->stopServerButton, &QPushButton::clicked, this, &MainWindow::handleStopServer);
    connect(ui->manageClientsButton, &QPushButton::clicked, this, &MainWindow::openClientManager);

    // 启动服务器
    if (server->startServer()) {
        appendLog("服务器已启动，等待客户端连接...");
    } else {
        appendLog("服务器启动失败！");
    }

    updateClientCount();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}

void MainWindow::handleClientConnected(const QString &clientAddress)
{
    appendLog(QString("客户端[%1]加入聊天室").arg(clientAddress));
    updateClientCount();
}

void MainWindow::handleClientDisconnected(const QString &clientAddress)
{
    appendLog(QString("客户端[%1]离开聊天室").arg(clientAddress));
    updateClientCount();
}

void MainWindow::handleMessageReceived(const QString &from, const QString &message)
{
    appendLog(QString("[%1]：%2").arg(from).arg(message));
}

void MainWindow::handleStopServer()
{
    if (server->isListening()) {
        // 关闭所有客户端连接
        for (QTcpSocket *client : server->getClients()) {
            client->disconnectFromHost();
        }
        server->close();
        appendLog("服务器已停止");
        ui->stopServerButton->setText("启动服务器");
    } else {
        if (server->startServer()) {
            appendLog("服务器已启动");
            ui->stopServerButton->setText("停止服务器");
        } else {
            appendLog("服务器启动失败！");
        }
    }
}

void MainWindow::appendLog(const QString &log)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(log));
}

void MainWindow::updateClientCount()
{
    int count = server->getClients().size();
    ui->statusLabel->setText(QString("聊天室在线人数：%1").arg(count));
}

void MainWindow::openClientManager()
{
    ClientManager manager(server, this);
    manager.exec();
}
