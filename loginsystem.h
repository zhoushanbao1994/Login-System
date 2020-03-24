#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QSqlTableModel>

#include "qdb.h"

namespace Ui {
class LoginSystem;
}

class LoginSystem : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginSystem(QWidget *parent = nullptr);
    ~LoginSystem();

private slots:
    // Login 登录页
    void on_Login_loginButton_clicked();
    void on_Login_regButton_clicked();

    // Register 注册页
    void on_Register_uplButton_clicked();
    void on_Register_backButton_clicked();
    void on_Register_completeRegButton_clicked();

    // Logged In 登录成功页
    void on_LoggedIn_adminButton_clicked();
    void on_LoggedIn_logoutButton_clicked();
    void on_LoggedIn_editButton_clicked();
    void on_LoggedIn_delButton_clicked();

    // Edit Profil 编辑资料页
    void on_EditProfil_uplButton_clicked();
    void on_EditProfil_backButton_clicked();
    void on_EditProfil_editedButton_clicked();

    // Admin Panel 管理面板页
    void on_AdminPanel_userBrowse_clicked();
    void on_AdminPanel_adminBrowse_clicked();
    void on_AdminPanel_pageButton_clicked();

    void on_AdminPanel_delUButton_clicked();
    void on_AdminPanel_delAButton_clicked();
    void on_AdminPanel_backButton_clicked();
    void on_AdminPanel_editedButton_clicked();


    ////////////////////////////////////////////////////////////


    void on_winStack_currentChanged(int arg1);
    void on_stackedWidget_currentChanged(int arg1);



private:
    Ui::LoginSystem *ui;

    bool m_loggedIn;        // 登录是否成功

    QDB *m_db;

    QString m_picName;      // 存放头像路径
    QString picDir = QCoreApplication::applicationDirPath()+"/../../LogSys/users/avatar";
    QSqlTableModel* tblMdl;

    QString username;
    QString password;
};

#endif // LOGINSYSTEM_H
