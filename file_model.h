#ifndef FILE_MODEL_H
#define FILE_MODEL_H

#include <QString>
#include <QList>
#include <QStringList>
#include "file_info.h"

struct Condition;

class FileModel {
public:
    FileModel();

    void AddFile(const FileInfo& file);
    void AddFromString(const QString& line);

    bool RemoveFile(int index);

    FileInfo GetFile(int index) const;
    QList<FileInfo> GetAllFiles() const;
    QStringList GetFilesStringList() const;
    int Count() const;
    void Clear();

    void LoadFromFile(const QString& filename);
    bool SaveToFile(const QString& filename) const;

    QStringList GetErrorLog() const;
    void ClearErrorLog();

    struct CommandResult {
        bool success;
        QString message;
        int affectedRows;

        CommandResult() : success(false), affectedRows(0) {}
    };

    CommandResult ExecuteCommand(const QString& commandLine);

    int RemoveByCondition(const Condition& condition);

private:
    QList<FileInfo> files_;
    mutable QStringList error_log_;

    void LogError(const QString& errorType, const QString& context, const QString& errorMsg) const;
};

#endif