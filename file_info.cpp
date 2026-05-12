#include "file_info.h"

FileInfo::FileInfo() : filename_(""), creationDate_(QDate::currentDate()), size_(0) {}

FileInfo::FileInfo(const QString& filename, const QDate& creationDate, qint64 size)
    : filename_(filename), creationDate_(creationDate), size_(size) {}

QString FileInfo::GetFilename() const { return filename_; }
QDate FileInfo::GetCreationDate() const { return creationDate_; }
qint64 FileInfo::GetSize() const { return size_; }

void FileInfo::SetFilename(const QString& filename) { filename_ = filename; }
void FileInfo::SetCreationDate(const QDate& date) { creationDate_ = date; }
void FileInfo::SetSize(qint64 size) { size_ = size; }

QString FileInfo::ToString() const {
    return QString("Файл: %1 | Дата создания: %2 | Размер: %3 байт")
        .arg(filename_)
        .arg(creationDate_.toString("yyyy.MM.dd"))
        .arg(size_);
}

bool FileInfo::IsValid() const {
    return !filename_.isEmpty() && creationDate_.isValid() && size_ >= 0;
}