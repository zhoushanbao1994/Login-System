#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H

#include <QMainWindow>

namespace Ui {
class LoginSystem;
}

class LoginSystem : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginSystem(QWidget *parent = 0);
    ~LoginSystem();
    bool Login(QString u, QString p);
    bool loggedIn;

private slots:
    void on_loginButton_clicked();

    void on_logoutButton_clicked();

    void on_completeRegButton_clicked();

    void on_backButton_clicked();

    void on_regButton_clicked();

private:
    Ui::LoginSystem *ui;
    QString username;
    QString password;
};

#endif // LOGINSYSTEM_H