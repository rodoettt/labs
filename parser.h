#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>
#include "file_info.h"
#include "parser_exceptions.h"

class Parser {
public:
    explicit Parser(const QString& input);

    FileInfo Parse();

    static bool IsValidDateFormat(const QString& dateStr);
    static bool IsValidDate(const QString& dateStr);
    static bool IsValidFilename(const QString& filenameStr);
    static bool IsValidSize(const QString& sizeStr, qint64& size);

private:
    void ValidateTokens(const QStringList& tokens);
    QString ParseFilename(const QString& token);
    QString ParseDate(const QString& token);
    qint64 ParseSize(const QString& token);

    QString input_;
};

#endif