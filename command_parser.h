#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <QString>
#include <QStringList>
#include "file_info.h"

struct Condition {
    enum class Type {
        DateEqual,
        DateLess,
        DateGreater,
        FilenameEqual,
        FilenameContains,
        SizeEqual,
        SizeLess,
        SizeGreater
    };

    Type type;
    QString value;
};

struct Command {
    enum class Type {
        Add,
        Rem,
        Save
    };

    Type type;
    QString data;
    Condition condition;
};

class CommandParser {
public:
    explicit CommandParser(const QString& line);

    Command Parse();

    static FileInfo ParseCsvToFileInfo(const QString& csvLine);

    static bool FileMatchesCondition(const FileInfo& file, const Condition& condition);

private:
    void ParseAddCommand(const QStringList& tokens);
    void ParseRemCommand(const QStringList& tokens);
    void ParseSaveCommand(const QStringList& tokens);

    Condition ParseCondition(const QString& conditionStr);

    QString input_;
    Command command_;
};

#endif