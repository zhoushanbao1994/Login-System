#ifndef QDB_H
#define QDB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>

#include <QString>
#include <QDebug>

namespace QDBLite {
    class QDB;
}

class QDB
{
public:
    QDB(const QString dbname);
    ~QDB();
    bool Connect();
    bool Init();
    bool Disconnect();
    QSqlDatabase getDb();   // 返回当前数据库


    // 登录
    bool Login(QString u, QString p);
    // 查询用户是否存在
    bool QueryUserIsExist(QString u);
    // 查询邮箱是否存在
    bool QueryEmailIsExist(QString em);

    // 新增用户
    bool AddUser(QString un, QString pw, QString fn, QString mn, QString ln, QString em);
    // 查询用户信息
    bool QueryUserInfo(QString un, QString pw, QString &fn, QString &mn, QString &ln, QString &em);
    // 删除用户
    bool DeleteUser(QString un);
    // 更新用户信息
    bool UpDateUserInfo(QString uno, QString un, QString pw, QString fn, QString mn, QString ln, QString em);
    // 删除所有普通用户  // rank = 1
    bool DeleteAllUser();
    // 删除所有管理用户  // rank = 0
    bool DeleteAllAdmin();

private:
    QSqlDatabase m_db;  // 数据库
    bool m_dbstate;     // 数据库状态
    QSqlQuery *m_check;    // 执行SQL语句
};


#endif // QDB_H                                                                                                                                                                                                                              
