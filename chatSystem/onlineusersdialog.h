#ifndef ONLINEUSERSDIALOG_H
#define ONLINEUSERSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class OnlineUsersDialog : public QDialog
{
    Q_OBJECT

public:
    OnlineUsersDialog(QWidget *parent = nullptr);
    ~OnlineUsersDialog();

private:
    QLabel *onlineUsersLabel;
    QLabel *onlineCountLabel;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // ONLINEUSERSDIALOG_H
