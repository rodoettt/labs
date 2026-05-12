#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QDebug>

class File {
private:
    QString filename;
    QString date;
    int size;

public:
    File(const QString& f = "", const QString& d = "", int s = 0);

    QString getFilename() const;
    QString getDate() const;
    int getSize() const;

    void setFilename(const QString& f);
    void setDate(const QString& d);
    void setSize(int s);

    bool isValid() const;
    QString toString() const;
};

#endif // FILE_H