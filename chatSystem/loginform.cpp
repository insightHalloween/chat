#include "loginform.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDateTime>
#include <QTextCursor>
#include <QPainter>
#include <QFileDialog>
#include <QDataStream>
#include <QFileInfo>
#include <QTimer>

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent)
    , clientSocket(nullptr)
    , isConnected(false)
    , progressDialog(nullptr)
    , emojiDialog(nullptr)
    , currentFile(nullptr)
    , isDownloading(false)
    , fileListDialog(nullptr)
{
    // 设置窗口标题和大小
    setWindowTitle("聊天系统");
    resize(800, 600);

    // 设置整体样式
    setStyleSheet(R"(
        QWidget {
            font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;
            font-size: 14px;
        }

        QLabel {
            color: #333333;
            font-weight: bold;
        }

        QLineEdit, QTextEdit {
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            padding: 8px;
            background-color: #ffffff;
            selection-background-color: #4CAF50;
        }

        QLineEdit:focus, QTextEdit:focus {
            border-color: #4CAF50;
        }

        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #45a049;
        }

        QPushButton:pressed {
            background-color: #3d8b40;
        }

        QPushButton:disabled {
            background-color: #cccccc;
        }

        QTextEdit#chatAreaEdit {
            background-color: #f5f5f5;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            padding: 8px;
        }

        QTextEdit#inputEdit {
            background-color: #ffffff;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            padding: 8px;
        }
    )");

    // 创建UI组件
    serverIpLabel = new QLabel("服务器IP:");
    serverIpEdit = new QLineEdit();
    serverIpEdit->setText("127.0.0.1");
    serverIpEdit->setPlaceholderText("请输入服务器IP地址");

    usernameLabel = new QLabel("用户名:");
    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("请输入用户名");

    passwordLabel = new QLabel("密码:");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("请输入密码");

    chatAreaEdit = new QTextEdit();
    chatAreaEdit->setReadOnly(true);
    chatAreaEdit->setObjectName("chatAreaEdit");
    chatAreaEdit->setPlaceholderText("消息将显示在这里...");

    inputLabel = new QLabel("消息:");
    inputEdit = new QTextEdit();
    inputEdit->setObjectName("inputEdit");
    inputEdit->setMaximumHeight(100);
    inputEdit->setPlaceholderText("输入消息...");

    loginButton = new QPushButton("登录");
    sendButton = new QPushButton("发送");
    sendButton->setEnabled(false);

    emojiButton = new QPushButton("😊");
    emojiButton->setFont(QFont("Segoe UI Emoji", 12));
    emojiButton->setFixedSize(40, 40);
    emojiButton->setStyleSheet(R"(
        QPushButton {
            background-color: #f0f0f0;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            padding: 0px;
        }
        QPushButton:hover {
            background-color: #e0e0e0;
        }
        QPushButton:pressed {
            background-color: #d0d0d0;
        }
    )");

    // 创建文件上传按钮
    uploadButton = new QPushButton("上传文件");
    uploadButton->setEnabled(false);
    uploadButton->setFixedWidth(100);  // 设置固定宽度
    uploadButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;  /* 使用与页面相同的绿色 */
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: bold;
            margin-left: 10px;
        }
        QPushButton:hover {
            background-color: #45a049;  /* 略深的绿色用于悬停效果 */
        }
        QPushButton:pressed {
            background-color: #3d8b40;  /* 更深的绿色用于按下效果 */
        }
        QPushButton:disabled {
            background-color: #a5d6a7;  /* 浅绿色用于禁用状态 */
        }
    )");

    // 创建群文件按钮
    groupFilesButton = new QPushButton("群文件");
    groupFilesButton->setEnabled(false);
    groupFilesButton->setFixedWidth(100);
    groupFilesButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 16px;
            font-weight: bold;
            margin-left: 10px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton:disabled {
            background-color: #a5d6a7;
        }
    )");

    // 创建布局
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // 添加服务器IP和登录按钮行
    QHBoxLayout *serverLayout = new QHBoxLayout();
    serverLayout->addWidget(serverIpLabel);
    serverLayout->addWidget(serverIpEdit);
    serverLayout->addWidget(loginButton);
    mainLayout->addLayout(serverLayout, 0, 0, 1, 2);

    // 添加用户名和密码行
    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(usernameEdit);
    userLayout->addWidget(passwordLabel);
    userLayout->addWidget(passwordEdit);
    mainLayout->addLayout(userLayout, 1, 0, 1, 2);

    // 创建聊天区域的水平布局
    QHBoxLayout *chatLayout = new QHBoxLayout();
    chatLayout->addWidget(chatAreaEdit);
    
    // 创建右侧按钮的垂直布局
    QVBoxLayout *rightButtonLayout = new QVBoxLayout();
    rightButtonLayout->addWidget(uploadButton);
    rightButtonLayout->addWidget(groupFilesButton);
    rightButtonLayout->addStretch();
    rightButtonLayout->setContentsMargins(0, 0, 0, 0);
    
    chatLayout->addLayout(rightButtonLayout);
    mainLayout->addLayout(chatLayout, 2, 0, 1, 2);

    // 添加输入区域
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(inputEdit, 1);
    inputLayout->addWidget(emojiButton);
    inputLayout->addWidget(sendButton);
    inputLayout->setSpacing(10);
    mainLayout->addLayout(inputLayout, 3, 0, 1, 2);

    // 设置布局
    setLayout(mainLayout);

    // 连接信号和槽
    connect(loginButton, &QPushButton::clicked, this, &LoginForm::onLoginButtonClicked);
    connect(sendButton, &QPushButton::clicked, this, &LoginForm::onSendButtonClicked);
    connect(emojiButton, &QPushButton::clicked, this, &LoginForm::onEmojiButtonClicked);
    connect(groupFilesButton, &QPushButton::clicked, this, &LoginForm::onGroupFilesButtonClicked);
    connect(inputEdit, &QTextEdit::textChanged, this, [this]() {
        bool hasText = !inputEdit->toPlainText().trimmed().isEmpty();
        sendButton->setEnabled(hasText && isConnected);
    });

    // 安装事件过滤器以处理Enter键
    inputEdit->installEventFilter(this);

    // 设置聊天区域的样式
    chatAreaEdit->setStyleSheet(R"(
        QTextEdit {
            background-color: #f5f5f5;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            padding: 8px;
        }
    )");

    // 设置聊天区域支持HTML
    chatAreaEdit->setAcceptRichText(true);

    // 连接文件上传按钮信号
    connect(uploadButton, &QPushButton::clicked, this, &LoginForm::onUploadButtonClicked);
}

void LoginForm::onEmojiButtonClicked()
{
    if (!emojiDialog) {
        emojiDialog = new EmojiDialog(this);
        connect(emojiDialog, &EmojiDialog::emojiSelected, this, &LoginForm::onEmojiSelected);
    }
    emojiDialog->show();
}

void LoginForm::onEmojiSelected(const QString &emoji)
{
    inputEdit->insertPlainText(emoji);
}

bool LoginForm::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == inputEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (keyEvent->modifiers() & Qt::ShiftModifier) {
                // Shift+Enter 插入换行
                inputEdit->insertPlainText("\n");
            } else {
                // 普通Enter发送消息
                onSendButtonClicked();
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

// 生成消息气泡的HTML
QString LoginForm::generateMessageBubble(const QString &message, const QString &timestamp, bool isSelf) {
    // 创建一个临时变量来存储替换后的消息
    QString formattedMessage = message;
    formattedMessage.replace("\n", "<br>");
    
    if (isSelf) {
        return QString("%1\t%2<br>%3").arg(timestamp, username, formattedMessage);
    } else {
        return QString("%1\t%2<br>%3").arg(timestamp, "对方", formattedMessage);
    }
}

void LoginForm::onSendButtonClicked()
{
    if (isConnected && !inputEdit->toPlainText().isEmpty()) {
        QString message = inputEdit->toPlainText();
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        
        // 构建HTML格式的消息
        QString htmlMessage = generateMessageBubble(message, timestamp, true);
        
        // 发送消息
        QString sendMessage = username + ": " + message;
        clientSocket->write(sendMessage.toUtf8());
        
        // 在本地显示消息
        chatAreaEdit->append(htmlMessage);
        inputEdit->clear();
        
        // 自动滚动到底部
        QTextCursor cursor = chatAreaEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        chatAreaEdit->setTextCursor(cursor);
    }
}

LoginForm::~LoginForm()
{
    if (clientSocket) {
        clientSocket->disconnectFromHost();
        delete clientSocket;
    }
    if (progressDialog) {
        delete progressDialog;
    }
}

void LoginForm::cleanupConnection(bool showErrorDialog)
{
    // 先保存进度对话框的指针，因为我们可能需要在最后才关闭它
    QProgressDialog* dialogToClose = progressDialog;
    progressDialog = nullptr;  // 立即将成员变量设为nullptr，避免重复清理

    // 断开所有信号连接并清理socket
    if (clientSocket) {
        clientSocket->disconnect(); // 断开所有信号连接
        clientSocket->abort();
        clientSocket->deleteLater();
        clientSocket = nullptr;
    }

    // 重置UI状态
    loginButton->setText("登录");
    isConnected = false;
    uploadButton->setEnabled(false);
    groupFilesButton->setEnabled(false);

    // 最后才关闭进度对话框，这样错误消息会显示在进度对话框之后
    if (dialogToClose) {
        dialogToClose->disconnect(); // 断开所有信号连接
        dialogToClose->hide();
        dialogToClose->deleteLater();
    }
}

void LoginForm::handleConnectionCanceled()
{
    cleanupConnection(true);
    QMessageBox::information(this, "连接取消", "已取消连接到服务器");
}

void LoginForm::onSocketReadyRead()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket || clientSocket != this->clientSocket) return;

    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data);

    // 检查是否是错误消息
    if (message.startsWith("ERROR:")) {
        if (message == "ERROR:USERNAME_EXISTS") {
            // 用户名重复错误处理
            cleanupConnection(true);
            QMessageBox::warning(this, "连接失败", "该用户名已被使用，请更换用户名后重试。");
            return;
        }
    }
    
    // 检查是否是文件列表响应
    if (data.startsWith("FILE_LIST:")) {
        if (fileListDialog) {
            fileListDialog->clearFiles();
            QString fileList = QString::fromUtf8(data.mid(10)); // 跳过"FILE_LIST:"
            QStringList files = fileList.split(',', Qt::SkipEmptyParts);
            for (const QString &fileName : files) {
                fileListDialog->addFile(fileName);
            }
        }
        return;
    }
    
    // 检查是否是文件下载响应
    if (data.startsWith("FILE_DATA:")) {
        if (isDownloading && currentFile) {
            QByteArray fileData = data.mid(10); // 跳过"FILE_DATA:"
            currentFile->write(fileData);
            // 更新进度条
            if (progressDialog) {
                progressDialog->setValue(progressDialog->value() + 10);
            }
        }
        return;
    }
    
    // 检查是否是文件下载完成
    if (data.startsWith("DOWNLOAD_COMPLETE:")) {
        if (currentFile) {
            currentFile->close();
            delete currentFile;
            currentFile = nullptr;
        }
        isDownloading = false;
        if (progressDialog) {
            progressDialog->hide();
            progressDialog->deleteLater();
            progressDialog = nullptr;
        }
        
        QMessageBox::information(this, "下载完成", 
            QString("文件 %1 下载完成").arg(downloadingFileName));
        return;
    }

    // 解析普通消息
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    
    if (message.startsWith(username + ":")) {
        // 自己发送的消息已经在onSendButtonClicked中处理
        return;
    } else {
        // 解析其他用户的消息
        int colonIndex = message.indexOf(":");
        if (colonIndex != -1) {
            QString senderName = message.left(colonIndex);
            QString content = message.mid(colonIndex + 2); // +2 跳过 ": "
            
            // 显示普通聊天消息
            QString htmlMessage = QString("%1\t%2<br>%3").arg(
                timestamp,
                senderName,
                content
            );
            chatAreaEdit->append(htmlMessage);
        } else {
            // 系统消息或文件传输消息
            if (message.contains("\n")) {
                // 处理包含换行的消息（文件上传开始消息）
                QStringList parts = message.split("\n");
                QString endtime = QDateTime::currentDateTime().toString("hh:mm:ss");
                QString htmlMessage = QString("%1\t%2<br>%3<br>%4\t%5").arg(
                    timestamp,
                    parts[0],
                    parts[1],
                    endtime,
                    parts[2]
                );
                chatAreaEdit->append(htmlMessage);
            } else {
                // 普通系统消息
                QString htmlMessage = QString("%1\t%2").arg(
                    timestamp,
                    message
                );
                chatAreaEdit->append(htmlMessage);
            }
        }
    }
    
    // 如果是第一条消息（欢迎消息），则完成连接过程
    if (message.startsWith("欢迎")) {
        if (progressDialog) {
            progressDialog->hide();
            progressDialog->deleteLater();
            progressDialog = nullptr;
        }
        
        isConnected = true;
        loginButton->setText("断开");
        uploadButton->setEnabled(true);
        groupFilesButton->setEnabled(true);
        
        // 请求文件列表
        QByteArray request = "LIST_FILES:";
        clientSocket->write(request);
    }
}

void LoginForm::onSocketError(QAbstractSocket::SocketError error)
{
    // 如果socket已经被删除或不是当前socket，直接返回
    if (!clientSocket || sender() != clientSocket) {
        return;
    }

    QString errorMessage;
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
        errorMessage = "连接被拒绝。请检查：\n1. 服务器IP地址是否正确\n2. 服务器是否正在运行\n3. 服务器端口(8888)是否被占用";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMessage = "找不到服务器。请检查：\n1. 服务器IP地址是否正确\n2. 是否在同一个局域网内";
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorMessage = "连接超时。请检查：\n1. 网络连接是否正常\n2. 服务器是否响应";
        break;
    case QAbstractSocket::NetworkError:
        errorMessage = "网络错误。请检查：\n1. 网络连接是否正常\n2. 防火墙设置";
        break;
    case QAbstractSocket::OperationError:
        // 操作错误（比如手动取消）不显示错误消息
        cleanupConnection(false);
        return;
    default:
        errorMessage = "连接错误: " + clientSocket->errorString();
    }

    cleanupConnection(true);
    QMessageBox::critical(this, "连接错误", errorMessage);
}

void LoginForm::onSocketDisconnected()
{
    // 如果不是当前socket触发的断开连接，直接返回
    if (sender() != clientSocket) {
        return;
    }

    cleanupConnection(false);
    
    // 清理文件列表对话框
    if (fileListDialog) {
        fileListDialog->hide();
    }

    chatAreaEdit->append("已断开连接");
}

void LoginForm::onSocketConnected()
{
    // 发送登录消息
    QString loginMessage = "LOGIN:" + username;
    clientSocket->write(loginMessage.toUtf8());
}

void LoginForm::onUploadButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件", "", "所有文件 (*.*)");
    if (!filePath.isEmpty()) {
        sendFile(filePath);
    }
}

void LoginForm::sendFile(const QString &filePath)
{
    if (!isConnected) {
        QMessageBox::warning(this, "错误", "未连接到服务器");
        return;
    }

    qDebug() << "开始发送文件:" << filePath;

    currentFile = new QFile(filePath);
    if (!currentFile->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + currentFile->errorString());
        delete currentFile;
        currentFile = nullptr;
        return;
    }

    // 获取文件信息
    QFileInfo fileInfo(*currentFile);
    QString fileName = fileInfo.fileName();
    qint64 fileSize = fileInfo.size();

    qDebug() << "文件名:" << fileName << "大小:" << fileSize;

    // 发送文件头信息
    QByteArray header;
    QDataStream out(&header, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);

    // 写入标记和文件信息
    out.writeRawData("FILE:", 5);  // 使用writeRawData直接写入标记
    out << fileName << fileSize;

    qDebug() << "发送文件头信息，大小:" << header.size();
    clientSocket->write(header);
    clientSocket->waitForBytesWritten();

    // 显示进度对话框
    progressDialog = new QProgressDialog("正在上传文件...", "取消", 0, fileSize, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setWindowTitle("文件上传");
    connect(progressDialog, &QProgressDialog::canceled, [this]() {
        if (currentFile) {
            currentFile->close();
            delete currentFile;
            currentFile = nullptr;
        }
    });
    progressDialog->show();

    // 开始发送文件数据
    bytesToWrite = fileSize;
    totalBytes = fileSize;
    bytesWritten = 0;

    // 分块读取并发送文件
    while (!currentFile->atEnd()) {
        QByteArray block = currentFile->read(4096);
        clientSocket->write(block);
        bytesWritten += block.size();
        progressDialog->setValue(bytesWritten);
        clientSocket->waitForBytesWritten();
    }

    currentFile->close();
    delete currentFile;
    currentFile = nullptr;
    progressDialog->close();
    progressDialog = nullptr;

    // 显示上传完成消息
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString message = generateMessageBubble("文件 " + fileName + " 上传完成", timestamp, true);
    chatAreaEdit->append(message);
}

void LoginForm::onGroupFilesButtonClicked()
{
    if (!fileListDialog) {
        fileListDialog = new FileListDialog(this);
        connect(fileListDialog, &FileListDialog::fileSelected, this, &LoginForm::onFileSelected);
    }

    if (!fileListDialog->isVisible()) {
        // 请求最新的文件列表
        QByteArray request = "LIST_FILES:";
        clientSocket->write(request);
        fileListDialog->show();
    }
}

void LoginForm::onFileSelected(const QString &fileName)
{
    downloadFile(fileName);
}

void LoginForm::downloadFile(const QString &fileName)
{
    if (isDownloading) {
        QMessageBox::warning(this, "警告", "正在下载其他文件，请稍后再试");
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, "保存文件",
                                                  QDir::homePath() + "/" + fileName);
    if (savePath.isEmpty()) {
        return;
    }

    currentFile = new QFile(savePath);
    if (!currentFile->open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "错误", "无法创建文件: " + currentFile->errorString());
        delete currentFile;
        currentFile = nullptr;
        return;
    }

    isDownloading = true;
    downloadingFileName = fileName;

    // 发送下载请求
    QByteArray request = "DOWNLOAD:" + fileName.toUtf8();
    clientSocket->write(request);

    // 显示进度对话框
    progressDialog = new QProgressDialog("正在下载文件...", "取消", 0, 100, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setWindowTitle("文件下载");
    connect(progressDialog, &QProgressDialog::canceled, [this]() {
        if (currentFile) {
            currentFile->close();
            delete currentFile;
            currentFile = nullptr;
        }
        isDownloading = false;
    });
    progressDialog->show();
}

FileListDialog::FileListDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("群文件");
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    resize(400, 500);  // 增加窗口大小

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);  // 增加边距
    
    // 添加标题标签
    QLabel *titleLabel = new QLabel("可用文件列表");
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 16px;
            font-weight: bold;
            color: #333333;
            padding: 5px 0;
            border-bottom: 2px solid #4CAF50;
            margin-bottom: 10px;
        }
    )");
    mainLayout->addWidget(titleLabel);
    
    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            border: 1px solid #e0e0e0;
            border-radius: 8px;
            background: white;
        }
        QScrollBar:vertical {
            border: none;
            background: #f0f0f0;
            width: 10px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical {
            background: #4CAF50;
            border-radius: 5px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");
    
    fileListWidget = new QWidget;
    fileListLayout = new QVBoxLayout(fileListWidget);
    fileListLayout->setSpacing(8);  // 减小按钮之间的间距
    fileListLayout->setContentsMargins(10, 10, 10, 10);
    fileListLayout->addStretch();
    
    scrollArea->setWidget(fileListWidget);
    mainLayout->addWidget(scrollArea);

    // 添加提示标签
    QLabel *hintLabel = new QLabel("点击文件名称即可下载");
    hintLabel->setStyleSheet(R"(
        QLabel {
            color: #666666;
            font-size: 12px;
            padding: 5px 0;
        }
    )");
    mainLayout->addWidget(hintLabel);

    setStyleSheet(R"(
        QDialog {
            background-color: white;
        }
        QPushButton {
            background-color: white;
            border: 1px solid #4CAF50;
            border-radius: 6px;
            padding: 10px;
            text-align: left;
            min-height: 20px;
            font-size: 14px;
            color: #333333;
        }
        QPushButton:hover {
            background-color: #E8F5E9;
            border: 1px solid #2E7D32;
        }
        QPushButton:pressed {
            background-color: #C8E6C9;
        }
    )");
}

void FileListDialog::clearFiles()
{
    while (QLayoutItem *item = fileListLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    fileListLayout->addStretch();
}

void FileListDialog::addFile(const QString &fileName)
{
    QPushButton *fileButton = new QPushButton(fileName);
    fileButton->setIcon(QIcon::fromTheme("text-x-generic"));  // 添加默认文件图标
    fileButton->setIconSize(QSize(16, 16));  // 设置图标大小
    connect(fileButton, &QPushButton::clicked, this, [this, fileName]() {
        emit fileSelected(fileName);
    });
    fileListLayout->insertWidget(fileListLayout->count() - 1, fileButton);
}

void LoginForm::onLoginButtonClicked()
{
    if (!isConnected) {
        // 获取服务器IP和端口
        QString serverIp = serverIpEdit->text();
        username = usernameEdit->text();

        if (username.isEmpty()) {
            QMessageBox::warning(this, "错误", "请输入用户名");
            return;
        }

        // 确保清理之前的连接
        cleanupConnection(false);

        // 创建新的socket连接
        clientSocket = new QTcpSocket(this);
        
        // 设置连接超时定时器
        QTimer *timeoutTimer = new QTimer(this);
        timeoutTimer->setSingleShot(true);
        connect(timeoutTimer, &QTimer::timeout, this, [this, timeoutTimer]() {
            if (clientSocket && !isConnected) {
                cleanupConnection(true);
                QMessageBox::warning(this, "连接超时", "连接服务器超时，请检查网络后重试。");
            }
            timeoutTimer->deleteLater();
        });
        
        // 设置连接信号
        connect(clientSocket, &QTcpSocket::connected, this, &LoginForm::onSocketConnected);
        connect(clientSocket, &QTcpSocket::disconnected, this, &LoginForm::onSocketDisconnected);
        connect(clientSocket, &QTcpSocket::readyRead, this, &LoginForm::onSocketReadyRead);
        connect(clientSocket, &QTcpSocket::errorOccurred, this, &LoginForm::onSocketError);
        
        // 创建并显示进度对话框
        progressDialog = new QProgressDialog("正在连接到服务器...", "取消", 0, 0, this);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setWindowTitle("连接中");
        progressDialog->setMinimumDuration(0);
        
        // 连接取消按钮的信号
        connect(progressDialog, &QProgressDialog::canceled, this, &LoginForm::handleConnectionCanceled);
        
        progressDialog->show();
        
        // 启动超时定时器（5秒）
        timeoutTimer->start(5000);
        
        // 连接到服务器
        clientSocket->connectToHost(serverIp, 8888);
    } else {
        // 断开连接
        cleanupConnection(false);
    }
}
