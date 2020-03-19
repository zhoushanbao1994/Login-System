#include "loginsystem.h"
#include "ui_loginsystem.h"
#include "qdb.h"
#include <QSqlRecord>
#include <QFileDialog>
#include <QMessageBox>

QDBLite::DB db;

LoginSystem::LoginSystem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginSystem)
{
    ui->setupUi(this);

    // 连接数据库
    bool dbstate = db.Connect(QCoreApplication::applicationDirPath()+"/d1b.s3db");
    if(dbstate == false) {
        QMessageBox::warning(
                    nullptr,
                    "warning",
                    "Connect DB Error",
                    QMessageBox::Cancel);
    }

    ui->winStack->setCurrentIndex(0);   // 显示第0页
    //ui->stackedWidget->setCurrentIndex(1);

    ui->Login_passwordBox->setEchoMode(QLineEdit::Password);
    ui->Login_passwordBox->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);

    ui->Register_pBox->setEchoMode(QLineEdit::Password);
    ui->Register_pBox->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);

    ui->EditProfil_pBox->setEchoMode(QLineEdit::Password);
    ui->EditProfil_pBox->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
}

LoginSystem::~LoginSystem()
{
    delete ui;
}

/*****************************************************************************************************
 ***************************************** Login 登录页 ***********************************************
 *****************************************************************************************************/
// 登录页-登录按键
void LoginSystem::on_Login_loginButton_clicked()
{
    // 登录是否成功
    this->m_loggedIn = Login(ui->Login_usernameBox->text(), ui->Login_passwordBox->text());

    if(this->m_loggedIn) {      // 登录成功
        this->username = ui->Login_usernameBox->text(); // 记录当前的账号密码
        this->password = ui->Login_passwordBox->text();

        ui->Login_loginLabelInfo->setText("");  // 登录窗口信息栏显示
        ui->winStack->setCurrentIndex(2);       // 跳转到登录成功页面
    }
    else {                      // 登录失败
        // 登录窗口信息栏显示登录失败
        ui->Login_loginLabelInfo->setText("Login failed: Invalid credentials!");
    }
}

bool LoginSystem::Login(QString u, QString p)
{
    ui->LoggedIn_adminButton->setVisible(false);

    bool exists = false;

    QSqlQuery checkQuery(db.getDb());
    checkQuery.prepare("SELECT username FROM sys_users WHERE username = (:un) AND passwd = (:pw)");
    checkQuery.bindValue(":un", u);
    checkQuery.bindValue(":pw", p);

    if (checkQuery.exec()) {  // 执行SQL语句。执行成功
        if (checkQuery.next()) {  // 执行结果中有数据，说明账号密码正确
            exists = true;
        }
    }

    return exists;  // 返回结果
}

// 登录页-注册按键
void LoginSystem::on_Login_regButton_clicked()
{
    // 将登录页的信息复制到注册页面
    ui->Register_uBox->setText(ui->Login_usernameBox->text());
    ui->Register_pBox->setText(ui->Login_passwordBox->text());
    // 跳转到注册页
    ui->winStack->setCurrentIndex(1);
}




/*****************************************************************************************************
 **************************************** Register 注册页 *********************************************
 *****************************************************************************************************/
// 注册页-上传照片按键
void LoginSystem::on_Register_uplButton_clicked()
{
    this->m_picName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    ui->Register_rpLabel->setText("<img src=\"file:///"+this->m_picName+"\" alt=\"Image read error!\" height=\"128\" width=\"128\" />");
}

// 注册页-回退按键
void LoginSystem::on_Register_backButton_clicked()
{
    ui->Login_loginLabelInfo->setText("");
    ui->winStack->setCurrentIndex(0);
}

// 注册页-确认注册按键
void LoginSystem::on_Register_completeRegButton_clicked()
{
    bool halt = false;

    if(ui->Register_uBox->text() == "")
    {
        ui->Register_uBox->setPlaceholderText("Username EMPTY!");
        halt = true;
    }

    if(ui->Register_pBox->text() == "")
    {
        ui->Register_pBox->setPlaceholderText("Password EMPTY!");
        halt = true;
    }

    if(ui->Register_eBox->text() == "")
    {
        ui->Register_eBox->setPlaceholderText("E-mail EMPTY!");
        halt = true;
    }

    if(ui->Register_fBox->text() == "")
    {
        ui->Register_fBox->setPlaceholderText("First Name EMPTY!");
        halt = true;
    }

    if(ui->Register_mBox->text() == "")
    {
        ui->Register_mBox->setPlaceholderText("Middle Name (optional)");
        halt = false;
    }

    if(ui->Register_lBox->text() == "")
    {
        ui->Register_lBox->setPlaceholderText("Last Name EMPTY!");
        halt = true;
    }

    QSqlQuery cQuery(db.getDb());
    cQuery.prepare("SELECT username FROM sys_users WHERE username = (:un)");
    cQuery.bindValue(":un", ui->Register_uBox->text());

    if(cQuery.exec())
    {
        if(cQuery.next())
        {
            ui->Register_uBox->setText("");
            ui->Register_uBox->setPlaceholderText("Choose a different Username!");
            halt = true;
        }
    }

    QSqlQuery cQuery2(db.getDb());
    cQuery2.prepare("SELECT email FROM sys_users WHERE email = (:em)");
    cQuery2.bindValue(":em", ui->Register_eBox->text());

    if(cQuery2.exec())
    {
        if(cQuery2.next())
        {
            ui->Register_eBox->setText("");
            ui->Register_eBox->setPlaceholderText("Use another E-mail!");
            halt = true;
        }
    }


    if(halt)
    {
        ui->Register_regLabelInfo->setText("Please correct your mistakes.");
    }
    else
    {
        if (this->m_picName != "")
        {
            QString to = this->picDir+"/"+ui->Register_uBox->text();

            if (QFile::exists(to))
            {
                QFile::remove(to);
            }

            QFile::copy(this->m_picName, to);
            this->m_picName = "";
        }

        ui->Register_regLabelInfo->setText("");
        QSqlQuery iQuery(db.getDb());
        iQuery.prepare("INSERT INTO sys_users(username, passwd, fname, mname, lname, email)"\
                       "VALUES(:un, :pw, :fn, :mn, :ln, :em)");
        iQuery.bindValue(":un", ui->Register_uBox->text());
        iQuery.bindValue(":pw", ui->Register_pBox->text());
        iQuery.bindValue(":fn", ui->Register_fBox->text());
        iQuery.bindValue(":mn", ui->Register_mBox->text());
        iQuery.bindValue(":ln", ui->Register_lBox->text());
        iQuery.bindValue(":em", ui->Register_eBox->text());

        if(iQuery.exec())
        {
            ui->Register_uBox->setText("");
            ui->Register_pBox->setText("");
            ui->Register_eBox->setText("");
            ui->Register_fBox->setText("");
            ui->Register_mBox->setText("");
            ui->Register_lBox->setText("");
            ui->Register_rpLabel->setText("<img src=\":user.png\" />");
            ui->Login_loginLabelInfo->setText("Registration Successful! You can now login.");
            ui->winStack->setCurrentIndex(0);
        }

    }
}


/*****************************************************************************************************
 ************************************* Logged In 登录成功页 ********************************************
 *****************************************************************************************************/
// 登录成功页-管理面板按键
void LoginSystem::on_LoggedIn_adminButton_clicked()
{
    ui->winStack->setCurrentIndex(4);
}

// 登录成功页-登出按键
void LoginSystem::on_LoggedIn_logoutButton_clicked()
{
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Login System", "Are you sure you want to logout?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        this->m_loggedIn = false;
        ui->Login_passwordBox->setText("");
        ui->Login_loginLabelInfo->setText("You signed out!");
        ui->winStack->setCurrentIndex(0);
    }
}

// 登录成功页-编辑个人资料按键
void LoginSystem::on_LoggedIn_editButton_clicked()
{
    QSqlQuery fetcher;
    fetcher.prepare("SELECT * FROM sys_users WHERE username = (:un) AND passwd = (:pw)");
    fetcher.bindValue(":un", this->username);
    fetcher.bindValue(":pw", this->password);
    fetcher.exec();

    int idUsername = fetcher.record().indexOf("username");
    int idPasswd = fetcher.record().indexOf("passwd");
    int idEmail = fetcher.record().indexOf("email");
    int idFname = fetcher.record().indexOf("fname");
    int idMname = fetcher.record().indexOf("mname");
    int idLname = fetcher.record().indexOf("lname");

    while (fetcher.next())
    {
        ui->EditProfil_uBox->setText(fetcher.value(idUsername).toString());
        ui->EditProfil_pBox->setText(fetcher.value(idPasswd).toString());
        ui->EditProfil_eBox->setText(fetcher.value(idEmail).toString());
        ui->EditProfil_fBox->setText(fetcher.value(idFname).toString());
        ui->EditProfil_mBox->setText(fetcher.value(idMname).toString());
        ui->EditProfil_lBox->setText(fetcher.value(idLname).toString());
    }

    ui->winStack->setCurrentIndex(3);
}

// 登录成功页-删除账户按键
void LoginSystem::on_LoggedIn_delButton_clicked()
{
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Login System", "Are you sure you want to delete your account?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        QString to = this->picDir+"/"+this->username;

        if (QFile::exists(to))
        {
            QFile::remove(to);
        }

        QSqlQuery dQuery(db.getDb());
        dQuery.prepare("DELETE FROM sys_users WHERE username = (:un)");
        dQuery.bindValue(":un", this->username);

        if(dQuery.exec())
        {
            ui->Login_usernameBox->setText("");
            ui->Login_passwordBox->setText("");
            ui->Login_loginLabelInfo->setText("Account deleted!");
            ui->winStack->setCurrentIndex(0);
        }
    }
}

/*****************************************************************************************************
 ************************************ Edit Profil 编辑资料页 *******************************************
 *****************************************************************************************************/
// 编辑资料页-上传照片按键
void LoginSystem::on_EditProfil_uplButton_clicked()
{
    this->m_picName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    ui->EditProfil_rpLabel->setText("<img src=\"file:///"+this->m_picName+"\" alt=\"Image read error!\" height=\"128\" width=\"128\" />");
}
// 编辑资料页-返回按键
void LoginSystem::on_EditProfil_backButton_clicked()
{
    // 返回登录成功页
    ui->winStack->setCurrentIndex(2);
}
// 编辑资料页-确认编辑资料
void LoginSystem::on_EditProfil_editedButton_clicked()
{
    bool halt = false;

    if(ui->EditProfil_uBox->text() == "")
    {
        ui->EditProfil_uBox->setPlaceholderText("Username EMPTY!");
        halt = true;
    }

    if(ui->EditProfil_pBox->text() == "")
    {
        ui->EditProfil_pBox->setPlaceholderText("Password EMPTY!");
        halt = true;
    }

    if(ui->EditProfil_eBox->text() == "")
    {
        ui->EditProfil_eBox->setPlaceholderText("E-mail EMPTY!");
        halt = true;
    }

    if(ui->EditProfil_fBox->text() == "")
    {
        ui->EditProfil_fBox->setPlaceholderText("First Name EMPTY!");
        halt = true;
    }

    if(ui->EditProfil_mBox->text() == "")
    {
        ui->EditProfil_mBox->setPlaceholderText("Middle Name (optional)");
        halt = false;
    }

    if(ui->EditProfil_lBox->text() == "")
    {
        ui->EditProfil_lBox->setPlaceholderText("Last Name EMPTY!");
        halt = true;
    }

    QSqlQuery cQuery(db.getDb());
    cQuery.prepare("SELECT username FROM sys_users WHERE username = (:un)");
    cQuery.bindValue(":un", ui->Register_uBox->text());

    if(cQuery.exec())
    {
        if(cQuery.next() && ui->EditProfil_uBox->text() != cQuery.value(0).toString())
        {
            ui->EditProfil_uBox->setText("");
            ui->EditProfil_uBox->setPlaceholderText("Choose a different Username!");
            halt = true;
        }
    }

    QSqlQuery cQuery2(db.getDb());
    cQuery2.prepare("SELECT email FROM sys_users WHERE email = (:em)");
    cQuery2.bindValue(":em", ui->EditProfil_eBox->text());

    if(cQuery2.exec())
    {
        if(cQuery2.next() && ui->EditProfil_eBox->text() != cQuery2.value(0).toString())
        {
            ui->EditProfil_eBox->setText("");
            ui->EditProfil_eBox->setPlaceholderText("Use another E-mail!");
            halt = true;
        }
    }


    if(halt)
    {
        ui->EditProfil_regLabelInfo->setText("Please correct your mistakes.");
    }
    else
    {
        if (this->m_picName != "")
        {
            QString to = this->picDir+"/"+ui->EditProfil_uBox->text();

            if (QFile::exists(to))
            {
                QFile::remove(to);
            }

            QFile::copy(this->m_picName, to);
            this->m_picName = "";
        }

        ui->EditProfil_regLabelInfo->setText("");
        QSqlQuery iQuery(db.getDb());
        iQuery.prepare("UPDATE sys_users SET username=(:un), passwd=(:pw), fname=(:fn), mname=(:mn), lname=(:ln), email=(:em) WHERE username=(:uno)");
        iQuery.bindValue(":un", ui->EditProfil_uBox->text());
        iQuery.bindValue(":pw", ui->EditProfil_pBox->text());
        iQuery.bindValue(":fn", ui->EditProfil_fBox->text());
        iQuery.bindValue(":mn", ui->EditProfil_mBox->text());
        iQuery.bindValue(":ln", ui->EditProfil_lBox->text());
        iQuery.bindValue(":em", ui->EditProfil_eBox->text());
        iQuery.bindValue(":uno", ui->EditProfil_uBox->text());

        if(iQuery.exec())
        {
            ui->winStack->setCurrentIndex(2);
        }

    }
}

/*****************************************************************************************************
 ************************************ Admin Panel 管理面板页 *******************************************
 *****************************************************************************************************/
// 管理面板页-显示所有管理用户
void LoginSystem::on_AdminPanel_userBrowse_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
// 管理面板页-显示所有普通用户
void LoginSystem::on_AdminPanel_adminBrowse_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
// 管理面板页-返回到个人主页
void LoginSystem::on_AdminPanel_pageButton_clicked()
{
    ui->winStack->setCurrentIndex(2);
}

// 管理面板页-删除所有普通用户
void LoginSystem::on_AdminPanel_delUButton_clicked()
{
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                           "Login System", "Are you sure you want to erase all accounts?",
                                           QMessageBox::Yes|QMessageBox::No).exec())
    {
        QSqlQuery dQuery(db.getDb());
        dQuery.prepare("DELETE FROM sys_users WHERE rank != 0 AND rank != -1");

        if(dQuery.exec())
        {
            ui->AdminPanel_adminLabel->setText("Query executed!");
        }
    }
}
// 管理面板页-删除所有管理用户
void LoginSystem::on_AdminPanel_delAButton_clicked()
{
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                           "Login System", "Are you sure you want to erase all administrators?"\
                                           "\n(This won't erase regular users and you)",
                                           QMessageBox::Yes|QMessageBox::No).exec())
    {
        QSqlQuery dQuery(db.getDb());
        dQuery.prepare("DELETE FROM sys_users WHERE rank != 1 AND username != \"" + this->username + "\"");

        if(dQuery.exec())
        {
            ui->AdminPanel_adminLabel->setText("Query executed!");
        }
    }
}

// 管理面板页-撤销所有修改
void LoginSystem::on_AdminPanel_backButton_clicked()
{
    this->tblMdl->revertAll();
    this->tblMdl->database().rollback();
}

// 管理面板页-执行所有修改
void LoginSystem::on_AdminPanel_editedButton_clicked()
{
    if(this->tblMdl->submitAll())
    {
        this->tblMdl->database().commit();
        ui->AdminPanel_adminLabel->setText("Saved to database!");
    }
    else
    {
        this->tblMdl->database().rollback();
    }
}







/////////////////////////////////////////////////////////////////////////////////



void LoginSystem::on_winStack_currentChanged(int arg1)
{

    if(arg1 == 3 && this->m_loggedIn)
    {
        if(QFile::exists(this->picDir+"/"+this->username))
        {
            ui->EditProfil_rpLabel->setText("<img src=\"file:///"+this->picDir+"/"+this->username+"\" alt=\"Image read error!\" height=\"128\" width=\"128\" />");
        }
    }

    if(arg1 == 2 && this->m_loggedIn)
    {
        if(QFile::exists(this->picDir+"/"+this->username))
        {
            ui->LoggedIn_loggedPic->setText("<img src=\"file:///"+this->picDir+"/"+this->username+"\" alt=\"Image read error!\" height=\"128\" width=\"128\" />");
        }

        QSqlQuery fetcher;
        fetcher.prepare("SELECT * FROM sys_users WHERE username = (:un)");
        fetcher.bindValue(":un", this->username);
        fetcher.exec();

        int idFname = fetcher.record().indexOf("fname");
        int idMname = fetcher.record().indexOf("mname");
        int idLname = fetcher.record().indexOf("lname");
        int idRank = fetcher.record().indexOf("rank");
        int idEmail = fetcher.record().indexOf("email");

        QString fullname, email, rank;

        while (fetcher.next())
        {
            fullname = fetcher.value(idFname).toString();
            fullname += " " + fetcher.value(idMname).toString();
            fullname += " " + fetcher.value(idLname).toString();
            rank = fetcher.value(idRank).toString();
            email = fetcher.value(idEmail).toString();
        }
        if(rank == "-1")
        {
            ui->LoggedIn_adminButton->setVisible(true);
        }
        ui->LoggedIn_nameLabel->setText(fullname);
        ui->LoggedIn_rankLabel->setText(rank);
        ui->LoggedIn_emailLabel->setText(email);
    }

    if(arg1 == 4 && this->m_loggedIn)
    {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void LoginSystem::on_stackedWidget_currentChanged(int arg1)
{
    if(arg1 == 0 && this->m_loggedIn)
    {
        ui->AdminPanel_headLabel->setText("USERS");
        this->tblMdl = new QSqlTableModel;
        this->tblMdl->setTable("sys_users");
        this->tblMdl->setFilter("rank != -1 AND rank != 0");
        this->tblMdl->select();
        ui->AdminPanel_tableView->setModel(this->tblMdl);
        this->tblMdl->database().transaction();
    }

    if(arg1 == 1 && this->m_loggedIn)
    {
        ui->AdminPanel_headLabel->setText("ADMINS");
        this->tblMdl = new QSqlTableModel;
        this->tblMdl->setTable("sys_users");
        this->tblMdl->setFilter("rank == -1 OR rank == 0");
        this->tblMdl->select();
        ui->AdminPanel_tableView_2->setModel(this->tblMdl);
        this->tblMdl->database().transaction();
    }
}

