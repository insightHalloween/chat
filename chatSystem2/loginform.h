#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QProgressDialog>
#include <QFileDialog>
#include <QFile>
#include <QScrollArea>
#include <QFrame>
#include <QDialog>
#include "onlineusersdialog.h"
#include "emojidialog.h"

// 文件列表对话框类
class FileListDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FileListDialog(QWidget *parent = nullptr);
    void clearFiles();
    void addFile(const QString &fileName);

signals:
    void fileSelected(const QString &fileName);

private:
    QVBoxLayout *mainLayout;
    QWidget *fileListWidget;
    QVBoxLayout *fileListLayout;
    QScrollArea *scrollArea;
};

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onLoginButtonClicked();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError error);
    void onSendButtonClicked();
    void onEmojiButtonClicked();
    void onEmojiSelected(const QString &emoji);
    void onUploadButtonClicked();
    void onGroupFilesButtonClicked();
    void onFileSelected(const QString &fileName);
    void sendFile(const QString &filePath);
    void downloadFile(const QString &fileName);
    void handleConnectionCanceled();

private:
    QString generateMessageBubble(const QString &message, const QString &timestamp, bool isSelf);
    void cleanupConnection(bool showErrorDialog);

    QLabel *serverIpLabel;
    QLineEdit *serverIpEdit;
    QLabel *usernameLabel;
    QLineEdit *usernameEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordEdit;
    QTextEdit *chatAreaEdit;
    QLabel *inputLabel;
    QTextEdit *inputEdit;
    QPushButton *loginButton;
    QPushButton *sendButton;
    QPushButton *emojiButton;
    QPushButton *uploadButton;
    QPushButton *groupFilesButton;
    FileListDialog *fileListDialog;

    // 网络相关成员
    QTcpSocket *clientSocket;
    QString username;
    bool isConnected;
    QProgressDialog *progressDialog;
    EmojiDialog *emojiDialog;

    // 文件传输相关
    QFile *currentFile;
    qint64 totalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    QByteArray outBlock;
    bool isDownloading;
    QString downloadingFileName;
};

#endif // LOGINFORM_H
