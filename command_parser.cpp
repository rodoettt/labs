#include "command_parser.h"
#include "parser_exceptions.h"
#include <QRegularExpression>
#include <QDate>

CommandParser::CommandParser(const QString& line) : input_(line.trimmed()) {}

Command CommandParser::Parse() {
    if (input_.isEmpty()) {
        throw ParseException("Пустая команда");
    }

    QStringList tokens = input_.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    if (tokens.isEmpty()) {
        throw ParseException("Не указана команда");
    }

    QString cmd = tokens[0].toUpper();

    if (cmd == "ADD") {
        ParseAddCommand(tokens);
    } else if (cmd == "REM") {
        ParseRemCommand(tokens);
    } else if (cmd == "SAVE") {
        ParseSaveCommand(tokens);
    } else {
        throw ParseException(QString("Неизвестная команда: %1").arg(cmd));
    }

    return command_;
}

void CommandParser::ParseAddCommand(const QStringList& tokens) {
    if (tokens.size() < 2) {
        throw ParseException("Команде ADD требуется аргумент (данные в формате: имя_файла;дата;размер)");
    }

    QString csvData;
    for (int i = 1; i < tokens.size(); ++i) {
        if (i > 1) csvData += " ";
        csvData += tokens[i];
    }

    command_.type = Command::Type::Add;
    command_.data = csvData;
}

void CommandParser::ParseRemCommand(const QStringList& tokens) {
    if (tokens.size() < 2) {
        throw ParseException("Команде REM требуется условие");
    }

    QString conditionStr;
    for (int i = 1; i < tokens.size(); ++i) {
        if (i > 1) conditionStr += " ";
        conditionStr += tokens[i];
    }

    command_.type = Command::Type::Rem;
    command_.condition = ParseCondition(conditionStr);
}

void CommandParser::ParseSaveCommand(const QStringList& tokens) {
    if (tokens.size() < 2) {
        throw ParseException("Команде SAVE требуется имя файла");
    }

    command_.type = Command::Type::Save;
    command_.data = tokens[1];
}

Condition CommandParser::ParseCondition(const QString& conditionStr) {
    Condition cond;

    QRegularExpression dateEqualRegex(R"(^date\s*==\s*(\d{4}\.\d{2}\.\d{2})$)");
    QRegularExpression dateLessRegex(R"(^date\s*<\s*(\d{4}\.\d{2}\.\d{2})$)");
    QRegularExpression dateGreaterRegex(R"(^date\s*>\s*(\d{4}\.\d{2}\.\d{2})$)");
    QRegularExpression filenameEqualRegex(R"(^filename\s*==\s*(.+)$)");
    QRegularExpression filenameContainsRegex(R"(^filename\s+contains\s+(.+)$)");
    QRegularExpression sizeEqualRegex(R"(^size\s*==\s*(\d+)$)");
    QRegularExpression sizeLessRegex(R"(^size\s*<\s*(\d+)$)");
    QRegularExpression sizeGreaterRegex(R"(^size\s*>\s*(\d+)$)");

    QRegularExpressionMatch match;

    if ((match = dateEqualRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::DateEqual;
        cond.value = match.captured(1);
    } else if ((match = dateLessRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::DateLess;
        cond.value = match.captured(1);
    } else if ((match = dateGreaterRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::DateGreater;
        cond.value = match.captured(1);
    } else if ((match = filenameEqualRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::FilenameEqual;
        cond.value = match.captured(1).trimmed();
    } else if ((match = filenameContainsRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::FilenameContains;
        cond.value = match.captured(1).trimmed();
    } else if ((match = sizeEqualRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::SizeEqual;
        cond.value = match.captured(1);
    } else if ((match = sizeLessRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::SizeLess;
        cond.value = match.captured(1);
    } else if ((match = sizeGreaterRegex.match(conditionStr)).hasMatch()) {
        cond.type = Condition::Type::SizeGreater;
        cond.value = match.captured(1);
    } else {
        throw ParseException(QString("Неизвестное условие: %1").arg(conditionStr));
    }

    return cond;
}

FileInfo CommandParser::ParseCsvToFileInfo(const QString& csvLine) {
    QStringList parts = csvLine.split(';');

    if (parts.size() < 3) {
        throw ParseException(QString("CSV строка должна содержать 3 поля (имя; дата; размер). Найдено: %1")
                                 .arg(parts.size()));
    }

    QString filenameStr = parts[0].trimmed();
    QString dateStr = parts[1].trimmed();
    QString sizeStr = parts[2].trimmed();

    QDate date = QDate::fromString(dateStr, "yyyy.MM.dd");
    if (!date.isValid()) {
        date = QDate::fromString(dateStr, "dd.MM.yyyy");
    }
    if (!date.isValid()) {
        throw ParseException(QString("Неверный формат даты: %1").arg(dateStr));
    }

    if (filenameStr.isEmpty()) {
        throw ParseException("Имя файла не может быть пустым");
    }

    bool ok;
    qint64 size = sizeStr.toLongLong(&ok);
    if (!ok || size < 0) {
        throw ParseException(QString("Неверный размер файла: %1").arg(sizeStr));
    }

    return FileInfo(filenameStr, date, size);
}

bool CommandParser::FileMatchesCondition(const FileInfo& file, const Condition& condition) {
    switch (condition.type) {
    case Condition::Type::DateEqual: {
        QDate condDate = QDate::fromString(condition.value, "yyyy.MM.dd");
        return file.GetCreationDate() == condDate;
    }
    case Condition::Type::DateLess: {
        QDate condDate = QDate::fromString(condition.value, "yyyy.MM.dd");
        return file.GetCreationDate() < condDate;
    }
    case Condition::Type::DateGreater: {
        QDate condDate = QDate::fromString(condition.value, "yyyy.MM.dd");
        return file.GetCreationDate() > condDate;
    }
    case Condition::Type::FilenameEqual:
        return file.GetFilename().compare(condition.value, Qt::CaseInsensitive) == 0;
    case Condition::Type::FilenameContains:
        return file.GetFilename().contains(condition.value, Qt::CaseInsensitive);
    case Condition::Type::SizeEqual: {
        qint64 condSize = condition.value.toLongLong();
        return file.GetSize() == condSize;
    }
    case Condition::Type::SizeLess: {
        qint64 condSize = condition.value.toLongLong();
        return file.GetSize() < condSize;
    }
    case Condition::Type::SizeGreater: {
        qint64 condSize = condition.value.toLongLong();
        return file.GetSize() > condSize;
    }
    }

    return false;
}