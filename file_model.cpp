#include "file_model.h"
#include "parser.h"
#include "command_parser.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QDir>

FileModel::FileModel() {}

void FileModel::AddFile(const FileInfo& file) {
    if (!file.IsValid()) {
        throw ParseException("Некорректная информация о файле");
    }
    files_.append(file);
}

void FileModel::AddFromString(const QString& line) {
    Parser parser(line);
    FileInfo file = parser.Parse();
    AddFile(file);
}

bool FileModel::RemoveFile(int index) {
    if (index >= 0 && index < files_.size()) {
        files_.removeAt(index);
        return true;
    }
    return false;
}

FileInfo FileModel::GetFile(int index) const {
    if (index >= 0 && index < files_.size()) {
        return files_[index];
    }
    throw ParseException("Индекс вне диапазона");
}

QList<FileInfo> FileModel::GetAllFiles() const {
    return files_;
}

QStringList FileModel::GetFilesStringList() const {
    QStringList result;
    for (const FileInfo& file : files_) {
        result << file.ToString();
    }
    return result;
}

int FileModel::Count() const {
    return files_.size();
}

void FileModel::Clear() {
    files_.clear();
}

void FileModel::LogError(const QString& errorType, const QString& context, const QString& errorMsg) const {
    QString logEntry = QString("%1 | %2 | %3 | %4")
    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
        .arg(errorType)
        .arg(context)
        .arg(errorMsg);

    qDebug() << logEntry;

    QDir dir;
    if (!dir.exists("logs")) {
        dir.mkdir("logs");
    }

    QFile logFile("logs/errors.log");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&logFile);
        stream << logEntry << "\n";
        logFile.close();
    }

    error_log_.append(logEntry);
}

void FileModel::LoadFromFile(const QString& filename) {
    QFile file(filename);

    if (!file.exists()) {
        LogError("FileNotFound", filename, "Файл не найден");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LogError("OpenError", filename, "Не удалось открыть файл");
        return;
    }

    QTextStream stream(&file);
    int lineNum = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        lineNum++;

        if (line.trimmed().isEmpty()) continue;

        try {
            AddFromString(line);
        } catch (const ParseException& e) {
            LogError(QString(typeid(e).name()),
                     QString("Строка %1: %2").arg(lineNum).arg(line),
                     e.getMessage());
        }
    }

    file.close();
}

bool FileModel::SaveToFile(const QString& filename) const {
    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LogError("SaveError", filename, "Не удалось открыть файл для записи");
        return false;
    }

    QTextStream stream(&file);
    for (const FileInfo& fileInfo : files_) {
        stream << fileInfo.GetFilename() << " "
               << fileInfo.GetCreationDate().toString("yyyy.MM.dd") << " "
               << fileInfo.GetSize() << "\n";
    }

    file.close();
    return true;
}

QStringList FileModel::GetErrorLog() const {
    return error_log_;
}

void FileModel::ClearErrorLog() {
    error_log_.clear();
}

FileModel::CommandResult FileModel::ExecuteCommand(const QString& commandLine) {
    CommandResult result;
    result.success = false;
    result.affectedRows = 0;

    try {
        CommandParser parser(commandLine);
        Command cmd = parser.Parse();

        switch (cmd.type) {
        case Command::Type::Add: {
            try {
                FileInfo file = CommandParser::ParseCsvToFileInfo(cmd.data);
                AddFile(file);
                result.affectedRows = 1;
                result.message = QString("Добавлен файл: %1").arg(file.ToString());
                result.success = true;
            } catch (const ParseException& e) {
                result.message = QString("Ошибка добавления: %1").arg(e.getMessage());
                LogError("AddError", cmd.data, e.getMessage());
            }
            break;
        }

        case Command::Type::Rem: {
            int removed = RemoveByCondition(cmd.condition);
            result.affectedRows = removed;
            result.message = QString("Удалено записей о файлах: %1").arg(removed);
            result.success = true;
            break;
        }

        case Command::Type::Save: {
            if (SaveToFile(cmd.data)) {
                result.message = QString("Данные сохранены в файл: %1").arg(cmd.data);
                result.success = true;
            } else {
                result.message = QString("Ошибка сохранения в файл: %1").arg(cmd.data);
            }
            break;
        }
        }
    } catch (const ParseException& e) {
        result.message = QString("Ошибка парсинга команды: %1").arg(e.getMessage());
        LogError("CommandParseError", commandLine, e.getMessage());
    }

    return result;
}

int FileModel::RemoveByCondition(const Condition& condition) {
    int removed = 0;

    for (int i = files_.size() - 1; i >= 0; --i) {
        if (CommandParser::FileMatchesCondition(files_[i], condition)) {
            files_.removeAt(i);
            removed++;
        }
    }

    return removed;
}