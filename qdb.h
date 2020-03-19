#ifndef QDB_H
#define QDB_H

#include <QSqlDatabase>
#include <QSqlQuery>

class QDB
{
public:
    QDB();
    ~QDB();
    bool Connect(const QString dbname);
    bool Disconnect();
    QSqlDatabase getDb();   // 返回dang当前数据库
    QSqlQuery Query(QString q);

private:
    QSqlDatabase m_db;  // 数据库
    bool m_dbstate;     // 数据库状态
};

namespace QDBLite {
    class DB : public QDB {};
}

#endif // QDB_H                                                                                                                                                                                                                              
