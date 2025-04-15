#include "onlineusersdialog.h"

OnlineUsersDialog::OnlineUsersDialog(QWidget *parent)
    : QDialog(parent)
{
    // 设置窗口标题和大小
    setWindowTitle("Server");
    resize(400, 200);

    // 设置淡绿色背景
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(240, 255, 240));
    setPalette(pal);
    setAutoFillBackground(true);

    // 创建在线人数标签
    onlineUsersLabel = new QLabel("在线人数:", this);
    QFont font = onlineUsersLabel->font();
    font.setPointSize(12);
    onlineUsersLabel->setFont(font);

    // 创建在线人数值标签
    onlineCountLabel = new QLabel("0", this);
    onlineCountLabel->setFont(font);

    // 创建按钮
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 添加在线人数行
    QHBoxLayout *onlineLayout = new QHBoxLayout();
    onlineLayout->addStretch();
    onlineLayout->addWidget(onlineUsersLabel);
    onlineLayout->addWidget(onlineCountLabel);
    onlineLayout->addStretch();
    mainLayout->addLayout(onlineLayout);

    // 添加按钮行
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // 设置布局
    setLayout(mainLayout);

    // 连接信号和槽
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

OnlineUsersDialog::~OnlineUsersDialog()
{
}
