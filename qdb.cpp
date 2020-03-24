#include "qdb.h"

QDB::QDB(const QString dbname)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");  // 数据库类型
    m_db.setDatabaseName(dbname);                 // 数据库名

    m_check = new QSqlQuery(m_db);              // 初始化QSqlQuery，执行SQL语句关联的数据库
}

QDB::~QDB()
{
    Disconnect();
}

// 连接数据库
bool QDB::Connect()
{
    if (!this->m_db.isOpen()) {  // 数据库不是处于打开状态
        if (!this->m_db.open()) {   // 打开失败
            m_dbstate = false;
            return false;
        }
        else {                      // 打开成功
            m_dbstate = true;
            return true;
        }
    }
    else
    {
        m_dbstate = false;
        return false;
    }
}

// 数据库打开后进行初始化
bool QDB::Init()
{
    QSqlQuery checkQuery(m_db);
    // 如果表不存在，则创建表sys_users
    checkQuery.prepare("CREATE TABLE IF NOT EXISTS sys_users (\
        id INTEGER PRIMARY KEY AUTOINCREMENT, \
        username TEXT NOT NULL UNIQUE, \
        passwd TEXT NOT NULL, \
        fname TEXT NOT NULL, \
        mname TEXT, \
        lname TEXT NOT NULL, \
        rank INTEGER DEFAULT (1), \
        email TEXT NOT NULL UNIQUE, \
        UNIQUE (username))");
    if (!checkQuery.exec()) {  // 执行SQL语句。执行失败
        qDebug() << "判断表是否存在，若不存在创建表。失败。";
        return false;
    }
    qDebug() << "判断表是否存在，若不存在创建表。成功。";

    // 查询管理员用户
    checkQuery.prepare("SELECT username FROM sys_users WHERE username = (:un)");
    checkQuery.bindValue(":un", "admin");
    if (!checkQuery.exec()) {  // 执行SQL语句。执行失败
        qDebug() << "查询管理员是否存在。失败。";
        return false;
    }
    else { // 执行成功
        qDebug() << "查询管理员是否存在。成功。";
        if (checkQuery.next()) {  // 执行结果中有数据，说明存在管理员账户
            qDebug() << "管理员账户存在。";
            return true;
        }
        else {  // 结果中没数据，不存在管理员账户
            checkQuery.prepare("INSERT INTO sys_users (id, username, passwd, fname, mname, lname, rank, email) VALUES (1, 'admin', 'pass', 'Root', NULL, 'Administrator', -1, 'admin@root.org');");
            checkQuery.exec();
            qDebug() << "管理员账户不存在。创建账户";
        }
    }
    return true;
}

// 断开数据库
bool QDB::Disconnect()
{
    if (this->m_db.isOpen()) {
        this->m_db.close();
        return true;
    }
    else {
        return false;
    }
}

QSqlDatabase QDB::getDb()
{
    return m_db;
}

// 登录 -- 查询指定的用户、密码是否存在
bool QDB::Login(QString u, QString p)
{
    m_check->prepare("SELECT username FROM sys_users WHERE username = (:un) AND passwd = (:pw)");
    m_check->bindValue(":un", u);
    m_check->bindValue(":pw", p);

    if (m_check->exec()) {  // 执行SQL语句。执行成功
        if (m_check->next()) {  // 执行结果中有数据，说明账号密码正确
            return true;
        }
    }

    return false;  // 返回结果
}

// 查询用户是否存在
bool QDB::QueryUserIsExist(QString un)
{
    m_check->prepare("SELECT username FROM sys_users WHERE username = (:un)");
    m_check->bindValue(":un", un);

    if(m_check->exec()) {
        if(m_check->next()) {
            return true;        // 用户存在，返回True
        }
    }
    return false;       // 用户不存在，返回False
}

// 查询邮箱是否存在
bool QDB::QueryEmailIsExist(QString em)
{
    m_check->prepare("SELECT email FROM sys_users WHERE email = (:em)");
    m_check->bindValue(":em", em);
    if(m_check->exec())  {
        if(m_check->next()) {
            return true;        // 邮箱存在，返回True
        }
    }
    return false;       // 邮箱不存在，返回False
}

// 增加用户
bool QDB::AddUser(QString un, QString pw, QString fn, QString mn, QString ln, QString em)
{
    m_check->prepare("INSERT INTO sys_users(username, passwd, fname, mname, lname, email)"\
                   "VALUES(:un, :pw, :fn, :mn, :ln, :em)");
    m_check->bindValue(":un", un);
    m_check->bindValue(":pw", pw);
    m_check->bindValue(":fn", fn);
    m_check->bindValue(":mn", mn);
    m_check->bindValue(":ln", ln);
    m_check->bindValue(":em", em);

    // 执行成功，
    if(m_check->exec()) {
        return true;
    }
    return false;
}

// 查询用户信息
bool QDB::QueryUserInfo(QString un, QString pw, QString &fn, QString &mn, QString &ln, QString &em)
{
    // 查询
    m_check->prepare("SELECT * FROM sys_users WHERE username = (:un) AND passwd = (:pw)");
    m_check->bindValue(":un", un);
    m_check->bindValue(":pw", pw);
    if(!m_check->exec())  { // 执行失败
        return false;
    }

    // 获取查询结果
    //int idUsername = m_check->record().indexOf("username");
    //int idPasswd = m_check->record().indexOf("passwd");
    int idEmail = m_check->record().indexOf("email");
    int idFname = m_check->record().indexOf("fname");
    int idMname = m_check->record().indexOf("mname");
    int idLname = m_check->record().indexOf("lname");

    // 获取内容
    while (m_check->next()) {
        //m_check->value(idUsername).toString();
        //m_check->value(idPasswd).toString();
        em = m_check->value(idEmail).toString();
        fn = m_check->value(idFname).toString();
        mn = m_check->value(idMname).toString();
        ln = m_check->value(idLname).toString();

        return true;
    }

    return false;
}

// 删除用户
bool QDB::DeleteUser(QString un)
{
    // 执行语句，删除账户
    m_check->prepare("DELETE FROM sys_users WHERE username = (:un)");
    m_check->bindValue(":un", un);
    if(m_check->exec()) {
        return true;
    }
    return false;
}

// 更新用户信息
bool QDB::UpDateUserInfo(QString uno, QString un, QString pw, QString fn, QString mn, QString ln, QString em)
{
    m_check->prepare("UPDATE sys_users SET username=(:un), passwd=(:pw), fname=(:fn), mname=(:mn), lname=(:ln), email=(:em) WHERE username=(:uno)");
    m_check->bindValue(":un", un);
    m_check->bindValue(":pw", pw);
    m_check->bindValue(":fn", fn);
    m_check->bindValue(":mn", mn);
    m_check->bindValue(":ln", ln);
    m_check->bindValue(":em", em);
    m_check->bindValue(":uno", uno);  // 原用户名

    if(m_check->exec()) { // 执行成功
        return true;
    }
    return false;
}

// 删除所有普通用户  // rank = 1
bool QDB::DeleteAllUser()
{
    m_check->prepare("DELETE FROM sys_users WHERE rank == 1");

    if(m_check->exec()) {
        return true;
    }
    return false;
}

// 删除所有管理用户  // rank = 0
bool QDB::DeleteAllAdmin()
{
    m_check->prepare("DELETE FROM sys_users WHERE rank == 0");

    if(m_check->exec()) {
        return true;
    }
    return false;
}
