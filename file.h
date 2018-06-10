#ifndef FILE_H
#define FILE_H

class QPolygon;

void save_to_file(QPolygon* polygon, QString name);
bool load_from_file(QString name, QPolygon *polygon);

#endif // FILE_H
