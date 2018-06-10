#ifndef DATABASE_H
#define DATABASE_H

#include <QWidget>

class QSqlDatabase;

class DbWidget : public QWidget
{
public:
    void readPolygon(QPolygon* polygon);
    void writePolygon(QPolygon *polygon);

private:
    bool openDb(QSqlDatabase *db);
};
#endif // DATABASE_H
