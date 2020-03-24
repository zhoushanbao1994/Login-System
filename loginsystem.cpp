#include "loginsystem.h"
#include "ui_loginsystem.h"
#include <QSqlRecord>
#include <QFileDialog>
#include <QMessageBox>

LoginSystem::LoginSystem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginSystem)
{
    ui->setupUi(this);

    m_db = new QDB(QCoreApplication::applicationDirPath()+"/db3.s3db");


    // 连接数据库
    bool dbstate = m_db->Connect();
    if(dbstate == false) {
        QMessageBox::warning(
                    nullptr,
                    "warning",
                    "Connect DB Error",
                    QMessageBox::Cancel);
    }
    m_db->Init();

    ui->winStack->setCurrentIndex(0);   // 显示第0页
    //ui->stackedWidget->setCurrentIndex(1);

    // 设置输入框格式
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
    this->m_loggedIn = m_db->Login(ui->Login_usernameBox->text(), ui->Login_passwordBox->text());

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
    // 选择照片，保存文件名
    this->m_picName = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "/",
                tr("Image Files (*.png *.jpg *.bmp)")
                );
    // 显示照片
    ui->Register_rpLabel->setText("<img src=\"file:///"+this->m_picName+"\" alt=\"Image read error!\" height=\"128\" width=\"128\" />");
}

// 注册页-回退按键
void LoginSystem::on_Register_backButton_clicked()
{
    // 登录页消息框清空
    ui->Login_loginLabelInfo->setText("");
    // 跳转到登录页
    ui->winStack->setCurrentIndex(0);
}

// 注册页-确认注册按键
void LoginSystem::on_Register_completeRegButton_clicked()
{
    bool halt = false;

    // 用户名
    if(ui->Register_uBox->text() == "") {
        ui->Register_uBox->setPlaceholderText("Username EMPTY!");
        halt = true;
    }

    // 密码
    if(ui->Register_pBox->text() == "") {
        ui->Register_pBox->setPlaceholderText("Password EMPTY!");
        halt = true;
    }

    // 邮箱
    if(ui->Register_eBox->text() == "") {
        ui->Register_eBox->setPlaceholderText("E-mail EMPTY!");
        halt = true;
    }

    // 名字
    if(ui->Register_fBox->text() == "") {
        ui->Register_fBox->setPlaceholderText("First Name EMPTY!");
        halt = true;
    }

    // 名字
    if(ui->Register_mBox->text() == "") {
        ui->Register_mBox->setPlaceholderText("Middle Name (optional)");
        halt = false;
    }

    // 名字
    if(ui->Register_lBox->text() == "") {
        ui->Register_lBox->setPlaceholderText("Last Name EMPTY!");
        halt = true;
    }

    // 查询数据库，判断用户名是否存在
    if(m_db->QueryUserIsExist(ui->Register_uBox->text())) {
        ui->Register_uBox->setText("");
        ui->Register_uBox->setPlaceholderText("选择其他用户名!");
        halt = true;
    }

    // 查询数据库，判断邮箱是否存在
    if(m_db->QueryEmailIsExist( ui->Register_eBox->text())) {
        ui->Register_eBox->setText("");
        ui->Register_eBox->setPlaceholderText("使用其他电子邮件!");
        halt = true;
    }

    // 填写信息有误
    if(halt) {
        ui->Register_regLabelInfo->setText("请更正您的错误。");
    }
    // 填写信息正确
    else {
        // 设置头像相关
        if (this->m_picName != "") {
            QString to = this->picDir+"/"+ui->Register_uBox->text();

            if (QFile::exists(to)) {
                QFile::remove(to);
            }

            QFile::copy(this->m_picName, to);
            this->m_picName = "";
        }

        ui->Register_regLabelInfo->setText("");
        // 执行SQL，插入数据
        QString un = ui->Register_uBox->text();
        QString pw = ui->Register_pBox->text();
        QString fn = ui->Register_fBox->text();
        QString mn = ui->Register_mBox->text();
        QString ln = ui->Register_lBox->text();
        QString em = ui->Register_eBox->text();

        // 执行成功，回到登录页面
        if(m_db->AddUser(un, pw, fn, mn, ln, em)) {
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
    // 跳转到管理面板页
    ui->winStack->setCurrentIndex(4);
}

// 登录成功页-登出按键
void LoginSystem::on_LoggedIn_logoutButton_clicked()
{
    // 确认是否登出
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Login System", "Are you sure you want to logout?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        this->m_loggedIn = false;       // 登录标志设为False
        ui->Login_passwordBox->setText("");
        ui->Login_loginLabelInfo->setText("You signed out!");
        ui->winStack->setCurrentIndex(0);   // 返回至登录页
    }
}

// 登录成功页-编辑个人资料按键
void LoginSystem::on_LoggedIn_editButton_clicked()
{
    // 查询
    QString fn, mn, ln, em;
    if(m_db->QueryUserInfo(this->username, this->password, fn, mn, ln, em)) {
        qDebug() << fn << mn << ln;
        // 将信息填写到“编辑资料页”
        ui->EditProfil_uBox->setText(this->username);
        ui->EditProfil_pBox->setText(this->password);
        ui->EditProfil_eBox->setText(em);
        ui->EditProfil_fBox->setText(fn);
        ui->EditProfil_mBox->setText(mn);
        ui->EditProfil_lBox->setText(ln);

        // 跳转到编辑资料页
        ui->winStack->setCurrentIndex(3);
    }
}

// 登录成功页-删除账户按键
void LoginSystem::on_LoggedIn_delButton_clicked()
{
    // 删除账号
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Login System", "Are you sure you want to delete your account?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        // 账户存在头像，先将头像删除
        QString to = this->picDir+"/"+this->username;
        if (QFile::exists(to)) {
            QFile::remove(to);
        }

        // 执行语句，删除账户
        if(m_db->DeleteUser(this->username)) {
            ui->Login_usernameBox->setText("");
            ui->Login_passwordBox->setText("");
            ui->Login_loginLabelInfo->setText("Account deleted!");
            ui->winStack->setCurrentIndex(0);       // 删除成功，回到登录页
        }
    }
}

/*****************************************************************************************************
 ************************************ Edit Profil 编辑资料页 *******************************************
 *****************************************************************************************************/
// 编辑资料页-上传照片按键
void LoginSystem::on_EditProfil_uplButton_clicked()
{
    // 选择照片，保存文件名
    this->m_picName = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "/",
                tr("Image Files (*.png *.jpg *.bmp)"));
    // 显示照片
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

    // 用户名
    if(ui->EditProfil_uBox->text() == "") {
        ui->EditProfil_uBox->setPlaceholderText("Username EMPTY!");
        halt = true;
    }

    // 密码
    if(ui->EditProfil_pBox->text() == "") {
        ui->EditProfil_pBox->setPlaceholderText("Password EMPTY!");
        halt = true;
    }

    // 邮箱
    if(ui->EditProfil_eBox->text() == "") {
        ui->EditProfil_eBox->setPlaceholderText("E-mail EMPTY!");
        halt = true;
    }

    // 名字
    if(ui->EditProfil_fBox->text() == "") {
        ui->EditProfil_fBox->setPlaceholderText("First Name EMPTY!");
        halt = true;
    }

    // 名字
    if(ui->EditProfil_mBox->text() == "") {
        ui->EditProfil_mBox->setPlaceholderText("Middle Name (optional)");
        halt = false;
    }

    // 名字
    if(ui->EditProfil_lBox->text() == "") {
        ui->EditProfil_lBox->setPlaceholderText("Last Name EMPTY!");
        halt = true;
    }

    // 查询数据库，判断用户名是否存在
    if(m_db->QueryUserIsExist(ui->EditProfil_uBox->text())) {
        ui->EditProfil_uBox->setText("");
        ui->EditProfil_uBox->setPlaceholderText("选择其他用户名!");
        halt = true;
    }

    // 查询数据库，判断邮箱是否存在
    if(m_db->QueryEmailIsExist( ui->EditProfil_eBox->text())) {
        ui->EditProfil_eBox->setText("");
        ui->EditProfil_eBox->setPlaceholderText("使用其他电子邮件!");
        halt = true;
    }

    // 信息错误
    if(halt) {
        ui->EditProfil_regLabelInfo->setText("Please correct your mistakes.");
    }
    // 信息正确
    else {
        // 删除旧头像，设置新头像
        if (this->m_picName != "") {
            QString to = this->picDir+"/"+ui->EditProfil_uBox->text();
            if (QFile::exists(to)) {
                QFile::remove(to);
            }

            QFile::copy(this->m_picName, to);
            this->m_picName = "";
        }

        // 更新信息
        ui->EditProfil_regLabelInfo->setText("");

        QString un = ui->EditProfil_uBox->text();
        QString pw = ui->EditProfil_pBox->text();
        QString fn = ui->EditProfil_fBox->text();
        QString mn = ui->EditProfil_mBox->text();
        QString ln = ui->EditProfil_lBox->text();
        QString em = ui->EditProfil_eBox->text();
        QString uno = this->username;  // 原用户名

        if(m_db->UpDateUserInfo(uno, un, pw, fn, mn, ln, em)) { // 执行成功
            this->username = un;
            this->password = pw;
            ui->winStack->setCurrentIndex(2);   // 返回登陆成功页
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
        if(m_db->DeleteAllUser()) {
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
        if(m_db->DeleteAllAdmin()) {
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

