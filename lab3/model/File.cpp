#include "File.h"
#include <QRegularExpression>

File::File(const QString& f, const QString& d, int s)
    : filename(f), date(d), size(s) {}

QString File::getFilename() const { return filename; }
QString File::getDate() const { return date; }
int File::getSize() const { return size; }

void File::setFilename(const QString& f) { filename = f; }
void File::setDate(const QString& d) { date = d; }
void File::setSize(int s) { size = s; }

bool File::isValid() const {
    if (filename.isEmpty()) return false;

    // Проверка формата даты ГГГГ.ММ.ДД
    QRegularExpression dateRegex(R"(^\d{4}\.\d{2}\.\d{2}$)");
    if (!dateRegex.match(date).hasMatch()) return false;

    // Проверка размера
    if (size < 0) return false;

    // Проверка имени файла на недопустимые символы
    QRegularExpression invalidChars(R"([<>:\"/\\|?*])");
    if (invalidChars.match(filename).hasMatch()) return false;

    return true;

}

QString File::toString() const {
    return QString("%1|%2|%3").arg(filename).arg(date).arg(size);
}