#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <QString>
#include <QDate>
#include <qglobal.h>

class FileInfo {
public:
    FileInfo();
    FileInfo(const QString& filename, const QDate& creationDate, qint64 size);

    QString GetFilename() const;
    QDate GetCreationDate() const;
    qint64 GetSize() const;

    void SetFilename(const QString& filename);
    void SetCreationDate(const QDate& date);
    void SetSize(qint64 size);

    QString ToString() const;
    bool IsValid() const;

private:
    QString filename_;
    QDate creationDate_;
    qint64 size_;
};

#endif