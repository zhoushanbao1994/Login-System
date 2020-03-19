#include "qdb.h"

QDB::QDB()
{

}

QDB::~QDB()
{
    Disconnect();
}

// 连接数据库
bool QDB::Connect(const QString dbname)
{
    if (!this->m_db.isOpen()) {  // 数据库不是处于打开状态
        this->m_db = QSqlDatabase::addDatabase("QSQLITE");  // 数据库类型
        this->m_db.setDatabaseName(dbname);                 // 数据库名

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

QSqlQuery Query(QString q)
{
    QSqlQuery query(q);
    return query;
}
