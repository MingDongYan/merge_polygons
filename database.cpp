#include "database.h"
#include <QtSql>
#include <QInputDialog>


bool DbWidget::openDb(QSqlDatabase* db)
{
    *db = QSqlDatabase::addDatabase("QSQLITE");

    QString path(QDir::currentPath());
    path.append(QDir::separator()).append("polygons.db");
    path = QDir::toNativeSeparators(path);

    db->setDatabaseName(path);
    db->open();
return true;
}

void DbWidget::readPolygon(QPolygon *polygon)
{
    QSqlDatabase db;
    openDb(&db);
    QStringList Tables = db.tables();
    bool ok;

    QString text = QInputDialog::getItem(this, tr("Загрузить полигон"),
                                                 tr("Имя полигона:"), Tables,
                                                 0, false, &ok);
    if (ok && !text.isEmpty()) {
        QSqlQuery query("SELECT `x`, `y` from `"+text+"`");

        QSqlRecord record = query.record();
        while (query.next())
        {
           int x = query.value(record.indexOf("x")).toInt();
           int y = query.value(record.indexOf("y")).toInt();

           polygon->append(QPoint(x,y));
           qDebug() << "Read:" << polygon->last().x() << polygon->last().y() << "\n";
        }
    }
    db.close();
    db.removeDatabase("QSQLITE");

}


void DbWidget::writePolygon(QPolygon* polygon)
{
    QSqlDatabase db;
    openDb(&db);
    QStringList Tables = db.tables();
    bool ok;

    QString text = QInputDialog::getItem(this, tr("Сохранить полигон"),
                                                 tr("Имя полигона:"), Tables,
                                                 0, true, &ok);
        if (ok && !text.isEmpty()) {

         QSqlQuery query;
         query.exec("DROP TABLE IF EXISTS "+text);
         qDebug() <<  query.lastError();
         query.exec("CREATE TABLE `"+text+"` (`x` int, `y` int)");

            for(int i=0;i<polygon->size();i++) {
                qDebug() << "Insert:" << polygon->at(i).x() << polygon->at(i).y() << "\n";
                query.exec(QString("INSERT INTO `"+text+"` VALUES ('%1', '%2')").arg(polygon->at(i).x()).arg(polygon->at(i).y()));
            }
        }

     db.close();
     db.removeDatabase("QSQLITE");
}
