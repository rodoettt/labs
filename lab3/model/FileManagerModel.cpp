#include "FileManagerModel.h"
#include <QRegularExpression>
#include <QTextStream>

FileManagerModel::FileManagerModel(QObject* parent) : QObject(parent) {}

void FileManagerModel::addToLog(const QString& error) {
    errorLog.append(error);
    emit errorOccurred(error);
    qDebug() << "[ERROR]" << error;
}

bool FileManagerModel::validateAndAddFile(const File& file) {
    if (!file.isValid()) {
        if (file.getFilename().isEmpty()) {
            addToLog("Пустое имя файла");
        } else if (file.getSize() < 0) {
            addToLog("Отрицательный размер файла: " + QString::number(file.getSize()));
        } else {
            addToLog("Некорректные данные файла: " + file.toString());
        }
        return false;
    }

    files.push_back(file);
    emit fileAdded(file);
    emit dataChanged();
    return true;
}

void FileManagerModel::addFile(const File& file) {
    validateAndAddFile(file);
}

void FileManagerModel::addFile(const QString& filename, const QString& date, int size) {
    File file(filename, date, size);
    validateAndAddFile(file);
}

void FileManagerModel::removeFile(int index) {
    if (index >= 0 && index < static_cast<int>(files.size())) {
        files.erase(files.begin() + index);
        emit fileRemoved(index);
        emit dataChanged();
    } else {
        addToLog("Попытка удаления несуществующего файла с индексом: " + QString::number(index));
    }
}

void FileManagerModel::clearAll() {
    files.clear();
    emit dataChanged();
}

File FileManagerModel::parseLine(const QString& line) const {
    QString trimmedLine = line.trimmed();

    if (trimmedLine.isEmpty()) {
        throw ParseException(line, "Пустая строка");
    }

    QStringList parts = trimmedLine.split('|');

    if (parts.size() != 3) {
        throw ParseException(line, "Ожидается 3 поля, получено " + QString::number(parts.size()));
    }

    QString filename = parts[0].trimmed();
    QString date = parts[1].trimmed();
    bool sizeOk;
    int size = parts[2].trimmed().toInt(&sizeOk);

    if (!sizeOk) {
        throw ParseException(line, "Размер должен быть целым числом");
    }

    File file(filename, date, size);

    if (!file.isValid()) {
        if (filename.isEmpty()) throw InvalidFilenameException(filename);
        if (size < 0) throw InvalidSizeException(size);
        // Проверка даты
        QRegularExpression dateRegex(R"(^\d{4}\.\d{2}\.\d{2}$)");
        if (!dateRegex.match(date).hasMatch()) {
            throw InvalidDateException(date);
        }
    }

    return file;
}

int FileManagerModel::parseAndAddFromLine(const QString& line) {
    try {
        File file = parseLine(line);
        if (validateAndAddFile(file)) {
            return 1;
        }
        return 0;
    } catch (const FileException& e) {
        addToLog(e.getMessage());
        return 0;
    } catch (const std::exception& e) {
        addToLog("Неизвестная ошибка: " + QString::fromUtf8(e.what()));
        return 0;
    }
}

int FileManagerModel::parseAndAddFromLines(const QStringList& lines) {
    int addedCount = 0;
    for (const QString& line : lines) {
        addedCount += parseAndAddFromLine(line);
    }
    return addedCount;
}

long long FileManagerModel::getTotalSize() const {
    long long total = 0;
    for (const auto& file : files) {
        total += file.getSize();
    }
    return total;
}

QString FileManagerModel::getErrorLogString() const {
    if (errorLog.isEmpty()) {
        return "Ошибок нет";
    }

    QString result;
    QTextStream stream(&result);
    stream << "Всего ошибок: " << errorLog.size() << "\n";
    for (int i = 0; i < errorLog.size(); ++i) {
        stream << i + 1 << ". " << errorLog[i] << "\n";
    }
    return result;
}

void FileManagerModel::clearErrorLog() {
    errorLog.clear();
}

std::vector<File> FileManagerModel::findFilesBySize(int minSize, int maxSize) const {
    std::vector<File> result;
    for (const auto& file : files) {
        if (file.getSize() >= minSize && file.getSize() <= maxSize) {
            result.push_back(file);
        }
    }
    return result;
}

std::vector<File> FileManagerModel::findFilesByDate(const QString& date) const {
    std::vector<File> result;
    for (const auto& file : files) {
        if (file.getDate() == date) {
            result.push_back(file);
        }
    }
    return result;
}