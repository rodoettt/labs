#ifndef FILEMANAGERMODEL_H
#define FILEMANAGERMODEL_H

#include <QObject>
#include <vector>
#include <QStringList>
#include "File.h"

// Исключения
class FileException : public std::exception {
private:
    QString message;
public:
    FileException(const QString& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.toUtf8().constData();
    }
    QString getMessage() const { return message; }
};

class InvalidFilenameException : public FileException {
public:
    InvalidFilenameException(const QString& filename)
        : FileException("Некорректное имя файла: " + filename) {}
};

class InvalidDateException : public FileException {
public:
    InvalidDateException(const QString& date)
        : FileException("Некорректная дата: " + date) {}
};

class InvalidSizeException : public FileException {
public:
    InvalidSizeException(int size)
        : FileException("Некорректный размер: " + QString::number(size)) {}
};

class ParseException : public FileException {
public:
    ParseException(const QString& line, const QString& reason)
        : FileException("Ошибка парсинга строки: \"" + line + "\" - " + reason) {}
};

class FileManagerModel : public QObject {
    Q_OBJECT

private:
    std::vector<File> files;
    QStringList errorLog; // Лог ошибок

    void addToLog(const QString& error);
    bool validateAndAddFile(const File& file);

public:
    FileManagerModel(QObject* parent = nullptr);

    // Основные операции
    void addFile(const File& file);
    void addFile(const QString& filename, const QString& date, int size);
    void removeFile(int index);
    void clearAll();

    // Парсинг строки формата "filename|date|size"
    File parseLine(const QString& line) const;
    int parseAndAddFromLine(const QString& line);
    int parseAndAddFromLines(const QStringList& lines);

    // Геттеры
    const std::vector<File>& getFiles() const { return files; }
    int getFileCount() const { return files.size(); }
    long long getTotalSize() const;
    const QStringList& getErrorLog() const { return errorLog; }
    QString getErrorLogString() const;
    void clearErrorLog();

    // Поиск
    std::vector<File> findFilesBySize(int minSize, int maxSize) const;
    std::vector<File> findFilesByDate(const QString& date) const;

signals:
    void fileAdded(const File& file);
    void fileRemoved(int index);
    void dataChanged();
    void errorOccurred(const QString& error);
};

#endif // FILEMANAGERMODEL_H