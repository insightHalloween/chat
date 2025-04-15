#include <QApplication>
#include "loginform.h"
#include "emojidialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建并显示登录窗口
    LoginForm loginForm;
    loginForm.show();
    
    return app.exec();
}
