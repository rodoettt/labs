#include "parser.h"
#include <QRegularExpression>
#include <QDate>

Parser::Parser(const QString& input) : input_(input.trimmed()) {}

FileInfo Parser::Parse() {
    if (input_.isEmpty()) {
        throw LineFormatException("Строка не может быть пустой");
    }

    QStringList tokens = input_.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    ValidateTokens(tokens);

    QString filenameStr = ParseFilename(tokens[0]);
    QString dateStr = ParseDate(tokens[1]);
    qint64 size = ParseSize(tokens[2]);

    QDate date = QDate::fromString(dateStr, "yyyy.MM.dd");
    if (!date.isValid()) {
        throw DateFormatException("Неверная дата: " + dateStr);
    }

    return FileInfo(filenameStr, date, size);
}

void Parser::ValidateTokens(const QStringList& tokens) {
    if (tokens.size() != 3) {
        throw LineFormatException(
            QString("Требуется ровно 3 поля (имя, дата, размер). Найдено: %1")
                .arg(tokens.size())
            );
    }
}

QString Parser::ParseFilename(const QString& token) {
    if (token.isEmpty()) {
        throw LineFormatException("Имя файла не может быть пустым");
    }
    if (!IsValidFilename(token)) {
        throw LineFormatException(
            QString("Неверное имя файла: %1").arg(token)
            );
    }
    return token;
}

QString Parser::ParseDate(const QString& token) {
    if (!IsValidDateFormat(token)) {
        throw DateFormatException(
            QString("Неверный формат даты: %1. Используйте ГГГГ.ММ.ДД")
                .arg(token)
            );
    }
    return token;
}

qint64 Parser::ParseSize(const QString& token) {
    qint64 size;
    if (!IsValidSize(token, size)) {
        throw LineFormatException(
            QString("Неверный размер файла: %1. Ожидается целое неотрицательное число")
                .arg(token)
            );
    }
    return size;
}

bool Parser::IsValidDateFormat(const QString& dateStr) {
    QRegularExpression regex(R"(^\d{4}\.\d{2}\.\d{2}$)");
    return regex.match(dateStr).hasMatch();
}

bool Parser::IsValidDate(const QString& dateStr) {
    if (!IsValidDateFormat(dateStr)) return false;
    QDate date = QDate::fromString(dateStr, "yyyy.MM.dd");
    return date.isValid();
}

bool Parser::IsValidFilename(const QString& filenameStr) {
    return !filenameStr.isEmpty() && !filenameStr.contains('/') && !filenameStr.contains('\\');
}

bool Parser::IsValidSize(const QString& sizeStr, qint64& size) {
    bool ok;
    size = sizeStr.toLongLong(&ok);
    return ok && size >= 0;
}