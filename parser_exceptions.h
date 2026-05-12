#ifndef PARSER_EXCEPTIONS_H
#define PARSER_EXCEPTIONS_H

#include <QString>
#include <exception>

class ParseException : public std::exception {
protected:
    QString message_;
public:
    explicit ParseException(const QString& msg) : message_(msg) {}
    virtual ~ParseException() = default;

    const char* what() const noexcept override {
        return message_.toUtf8().constData();
    }

    QString getMessage() const { return message_; }
};

class DateFormatException : public ParseException {
public:
    explicit DateFormatException(const QString& msg) : ParseException(msg) {}
};

class FilenameFormatException : public ParseException {
public:
    explicit FilenameFormatException(const QString& msg) : ParseException(msg) {}
};

class SizeFormatException : public ParseException {
public:
    explicit SizeFormatException(const QString& msg) : ParseException(msg) {}
};

class LineFormatException : public ParseException {
public:
    explicit LineFormatException(const QString& msg) : ParseException(msg) {}
};

#endif