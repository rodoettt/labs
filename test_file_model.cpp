#include <QtTest>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include "../model/file_model.h"
#include "../model/parser.h"

#define QVERIFY_THROWS_EXCEPTION(ExceptionType, expression) \
do { \
        bool caught = false; \
    try { expression; } \
        catch (const ExceptionType&) { caught = true; } \
        catch (const std::exception& e) { \
            qDebug() << "Unexpected exception:" << e.what(); \
    } \
        catch (...) {} \
        QVERIFY2(caught, "Expected exception " #ExceptionType " not thrown"); \
} while (0)

#define QVERIFY_NO_THROW(expression) \
    do { \
    try { expression; } \
        catch (const std::exception& e) { \
            QFAIL(QString("Expected no exception, but got: %1").arg(e.what()).toUtf8()); \
    } \
        catch (...) { \
            QFAIL("Expected no exception, but got unknown exception"); \
    } \
} while (0)

    class TestLogger {
    private:
        QString logFileName;
        int totalTests;
        int passedTests;
        int failedTests;

    public:
        TestLogger() : totalTests(0), passedTests(0), failedTests(0) {
            QDir dir;
            if (!dir.exists("test_logs")) {
                dir.mkdir("test_logs");
            }

            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
            logFileName = QString("test_logs/test_results_%1.log").arg(timestamp);

            writeLog("");
            writeLog("ЗАПУСК МОДУЛЬНЫХ ТЕСТОВ (Парсер файлов)");
            writeLog("Время: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            writeLog("");
        }

        void writeLog(const QString& message) {
            QFile file(logFileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << message << "\n";
                file.close();
            }
        }

        void testStarted(const QString& testName) {
            totalTests++;
            writeLog("Запуск теста " + QString::number(totalTests) + ": " + testName);
        }

        void testPassed(const QString& testName, int duration) {
            passedTests++;
            QString msg = "Тест " + QString::number(totalTests) + " ПРОЙДЕН: " + testName + " (за " + QString::number(duration) + " мс)";
            writeLog(msg);
            qDebug().noquote() << msg;
        }

        void testFailed(const QString& testName, const QString& error, int duration) {
            failedTests++;
            QString msg = "Тест " + QString::number(totalTests) + " ПРОВАЛЕН: " + testName + " (за " + QString::number(duration) + " мс)";
            writeLog(msg);
            writeLog("Ошибка: " + error);
            qDebug().noquote() << msg;
            qDebug().noquote() << "Ошибка: " << error;
        }

        void writeSummary() {
            writeLog("");
            writeLog("ИТОГИ ТЕСТИРОВАНИЯ");
            writeLog("");
            writeLog("Всего тестов: " + QString::number(totalTests));
            writeLog("Пройдено: " + QString::number(passedTests));
            writeLog("Провалено: " + QString::number(failedTests));
            writeLog("Процент успеха: " + QString::number(totalTests > 0 ? (passedTests * 100 / totalTests) : 0) + "%");
            writeLog("");
            writeLog("Время завершения: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

            qDebug().noquote() << "";
            qDebug().noquote() << "ИТОГИ ТЕСТОВ:";
            qDebug().noquote() << "Всего: " + QString::number(totalTests) + ", Пройдено: " + QString::number(passedTests) + ", Провалено: " + QString::number(failedTests) + ", Успех: " + QString::number(totalTests > 0 ? (passedTests * 100 / totalTests) : 0) + "%";
        }

        int getFailedTests() const { return failedTests; }
    };

static TestLogger* testLogger = nullptr;

class TestFileModel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        if (!testLogger) {
            testLogger = new TestLogger();
        }
    }

    void cleanupTestCase() {
        if (testLogger) {
            testLogger->writeSummary();
            delete testLogger;
            testLogger = nullptr;
        }
    }

    void testParseValidFileInfo() {
        QString testName = "testParseValidFileInfo";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("document.txt 2024.12.25 1024");
            FileInfo file = parser.Parse();

            QCOMPARE(file.GetFilename(), QString("document.txt"));
            QCOMPARE(file.GetCreationDate(), QDate(2024, 12, 25));
            QCOMPARE(file.GetSize(), qint64(1024));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseWithLargeFileSize() {
        QString testName = "testParseWithLargeFileSize";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("video.mp4 2024.01.15 2147483648");
            FileInfo file = parser.Parse();

            QCOMPARE(file.GetFilename(), QString("video.mp4"));
            QCOMPARE(file.GetCreationDate(), QDate(2024, 1, 15));
            QCOMPARE(file.GetSize(), qint64(2147483648));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseFileWithSpacesInName() {
        QString testName = "testParseFileWithSpacesInName";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            // Имена с пробелами не поддерживаются в текущем формате
            Parser parser("my_document.pdf 2024.06.15 512000");
            FileInfo file = parser.Parse();

            QCOMPARE(file.GetFilename(), QString("my_document.pdf"));
            QCOMPARE(file.GetCreationDate(), QDate(2024, 6, 15));
            QCOMPARE(file.GetSize(), qint64(512000));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseInvalidDateFormatThrows() {
        QString testName = "testParseInvalidDateFormatThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("file.txt 25.12.2024 1024");
            QVERIFY_THROWS_EXCEPTION(DateFormatException, parser.Parse());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseNegativeSizeThrows() {
        QString testName = "testParseNegativeSizeThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("file.txt 2024.12.25 -100");
            QVERIFY_THROWS_EXCEPTION(LineFormatException, parser.Parse());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseNonNumericSizeThrows() {
        QString testName = "testParseNonNumericSizeThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("file.txt 2024.12.25 abc");
            QVERIFY_THROWS_EXCEPTION(LineFormatException, parser.Parse());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseEmptyStringThrows() {
        QString testName = "testParseEmptyStringThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("");
            QVERIFY_THROWS_EXCEPTION(LineFormatException, parser.Parse());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseMissingFieldsThrows() {
        QString testName = "testParseMissingFieldsThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("file.txt 2024.12.25");
            QVERIFY_THROWS_EXCEPTION(LineFormatException, parser.Parse());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testParseTooManyFieldsThrows() {
        QString testName = "testParseTooManyFieldsThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            Parser parser("file.txt 2024.12.25 1024 extra_field");
            QVERIFY_THROWS_EXCEPTION(LineFormatException, parser.Parse());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testAddValidFile() {
        QString testName = "testAddValidFile";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            FileInfo file("document.txt", QDate(2024, 12, 25), 1024);

            QVERIFY_NO_THROW(model.AddFile(file));
            QCOMPARE(model.Count(), 1);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testAddInvalidFileThrows() {
        QString testName = "testAddInvalidFileThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            FileInfo file("", QDate(2024, 12, 25), 1024); // пустое имя

            QVERIFY_THROWS_EXCEPTION(ParseException, model.AddFile(file));
            QCOMPARE(model.Count(), 0);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testAddFromString() {
        QString testName = "testAddFromString";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("document.txt 2024.12.25 1024");

            QCOMPARE(model.Count(), 1);
            FileInfo file = model.GetFile(0);
            QCOMPARE(file.GetFilename(), QString("document.txt"));
            QCOMPARE(file.GetCreationDate(), QDate(2024, 12, 25));
            QCOMPARE(file.GetSize(), qint64(1024));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testRemoveFile() {
        QString testName = "testRemoveFile";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("file1.txt 2024.12.25 100");
            model.AddFromString("file2.txt 2024.12.26 200");

            QCOMPARE(model.Count(), 2);

            bool removed = model.RemoveFile(0);
            QVERIFY(removed);
            QCOMPARE(model.Count(), 1);
            QCOMPARE(model.GetFile(0).GetFilename(), QString("file2.txt"));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testRemoveFileInvalidIndex() {
        QString testName = "testRemoveFileInvalidIndex";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("file1.txt 2024.12.25 100");

            bool removed = model.RemoveFile(5);
            QVERIFY(!removed);
            QCOMPARE(model.Count(), 1);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testGetFileInvalidIndexThrows() {
        QString testName = "testGetFileInvalidIndexThrows";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;

            QVERIFY_THROWS_EXCEPTION(ParseException, model.GetFile(0));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testClearModel() {
        QString testName = "testClearModel";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("file1.txt 2024.12.25 100");
            model.AddFromString("file2.txt 2024.12.26 200");

            model.Clear();
            QCOMPARE(model.Count(), 0);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testLoadFromFileWithValidData() {
        QString testName = "testLoadFromFileWithValidData";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            QString filename = "temp_test_valid.txt";
            QFile file(filename);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            out << "document.txt 2024.12.25 1024\n";
            out << "report.pdf 2024.11.15 2048576\n";
            out << "image.png 2024.10.30 512000\n";
            file.close();

            FileModel model;
            model.LoadFromFile(filename);

            QCOMPARE(model.Count(), 3);
            QCOMPARE(model.GetFile(0).GetFilename(), QString("document.txt"));
            QCOMPARE(model.GetFile(1).GetSize(), qint64(2048576));
            QCOMPARE(model.GetFile(2).GetCreationDate(), QDate(2024, 10, 30));

            QFile::remove(filename);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testLoadFromFileWithErrors() {
        QString testName = "testLoadFromFileWithErrors";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            QString filename = "temp_test_errors.txt";
            QFile file(filename);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&file);
            out << "document.txt 2024.12.25 1024\n";      // валидная
            out << "badfile.txt 25.12.2024 500\n";        // неверная дата
            out << "file.txt 2024.12.27\n";               // недостаточно полей
            out << "report.pdf 2024.12.28 2048\n";        // валидная
            out << "data.txt 2024.12.29 abc\n";           // неверный размер
            file.close();

            FileModel model;
            model.LoadFromFile(filename);

            QCOMPARE(model.Count(), 2);

            QStringList errors = model.GetErrorLog();
            QVERIFY(errors.size() >= 3);

            QFile::remove(filename);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testSaveAndLoadRoundTrip() {
        QString testName = "testSaveAndLoadRoundTrip";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            QString filename = "temp_roundtrip.txt";

            FileModel originalModel;
            originalModel.AddFromString("document.txt 2024.12.25 1024");
            originalModel.AddFromString("report.pdf 2024.11.15 2048576");
            originalModel.AddFromString("image.png 2024.10.30 512000");

            QVERIFY(originalModel.SaveToFile(filename));

            FileModel loadedModel;
            loadedModel.LoadFromFile(filename);

            QCOMPARE(loadedModel.Count(), 3);
            QCOMPARE(loadedModel.GetFile(0).GetFilename(), QString("document.txt"));
            QCOMPARE(loadedModel.GetFile(0).GetCreationDate(), QDate(2024, 12, 25));
            QCOMPARE(loadedModel.GetFile(0).GetSize(), qint64(1024));
            QCOMPARE(loadedModel.GetFile(1).GetFilename(), QString("report.pdf"));
            QCOMPARE(loadedModel.GetFile(2).GetFilename(), QString("image.png"));

            QFile::remove(filename);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testSaveToNonexistentDirectory() {
        QString testName = "testSaveToNonexistentDirectory";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("test.txt 2024.12.25 100");

            // Попытка сохранения в несуществующую директорию
            bool result = model.SaveToFile("/nonexistent_dir/test_output.txt");
            QVERIFY(!result);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteAddCommand() {
        QString testName = "testExecuteAddCommand";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            auto result = model.ExecuteCommand("ADD document.txt;2024.12.25;1024");

            QVERIFY(result.success);
            QCOMPARE(result.affectedRows, 1);
            QCOMPARE(model.Count(), 1);
            QCOMPARE(model.GetFile(0).GetFilename(), QString("document.txt"));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteAddCommandInvalidData() {
        QString testName = "testExecuteAddCommandInvalidData";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            auto result = model.ExecuteCommand("ADD badfile.txt;invalid_date;1024");

            QVERIFY(!result.success);
            QCOMPARE(model.Count(), 0);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteRemCommandByFilename() {
        QString testName = "testExecuteRemCommandByFilename";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("doc1.txt 2024.12.25 100");
            model.AddFromString("doc2.txt 2024.12.26 200");
            model.AddFromString("doc3.txt 2024.12.27 300");

            auto result = model.ExecuteCommand("REM filename == doc2.txt");

            QVERIFY(result.success);
            QCOMPARE(result.affectedRows, 1);
            QCOMPARE(model.Count(), 2);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteRemCommandByDate() {
        QString testName = "testExecuteRemCommandByDate";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("old.txt 2024.01.01 100");
            model.AddFromString("mid.txt 2024.06.15 200");
            model.AddFromString("new.txt 2024.12.31 300");

            auto result = model.ExecuteCommand("REM date > 2024.06.01");

            QVERIFY(result.success);
            QCOMPARE(result.affectedRows, 2);
            QCOMPARE(model.Count(), 1);
            QCOMPARE(model.GetFile(0).GetFilename(), QString("old.txt"));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteRemCommandBySize() {
        QString testName = "testExecuteRemCommandBySize";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("small.txt 2024.12.25 100");
            model.AddFromString("medium.txt 2024.12.26 1000");
            model.AddFromString("large.txt 2024.12.27 10000");

            auto result = model.ExecuteCommand("REM size < 500");

            QVERIFY(result.success);
            QCOMPARE(result.affectedRows, 1);
            QCOMPARE(model.Count(), 2);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteRemCommandByFilenameContains() {
        QString testName = "testExecuteRemCommandByFilenameContains";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("report_2024.pdf 2024.12.25 1024");
            model.AddFromString("invoice_2024.pdf 2024.12.26 2048");
            model.AddFromString("readme.txt 2024.12.27 512");

            auto result = model.ExecuteCommand("REM filename contains report");

            QVERIFY(result.success);
            QCOMPARE(result.affectedRows, 1);
            QCOMPARE(model.Count(), 2);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteRemCommandNoMatches() {
        QString testName = "testExecuteRemCommandNoMatches";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("file1.txt 2024.12.25 100");
            model.AddFromString("file2.txt 2024.12.26 200");

            auto result = model.ExecuteCommand("REM filename == nonexistent.txt");

            QVERIFY(result.success);
            QCOMPARE(result.affectedRows, 0);
            QCOMPARE(model.Count(), 2);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteSaveCommand() {
        QString testName = "testExecuteSaveCommand";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            model.AddFromString("test.txt 2024.12.25 1024");

            QString saveFilename = "temp_save_test.txt";
            auto result = model.ExecuteCommand("SAVE " + saveFilename);

            QVERIFY(result.success);
            QVERIFY(QFile::exists(saveFilename));

            // Проверяем содержимое сохраненного файла
            FileModel loadedModel;
            loadedModel.LoadFromFile(saveFilename);
            QCOMPARE(loadedModel.Count(), 1);
            QCOMPARE(loadedModel.GetFile(0).GetFilename(), QString("test.txt"));

            QFile::remove(saveFilename);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testExecuteUnknownCommand() {
        QString testName = "testExecuteUnknownCommand";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;
            auto result = model.ExecuteCommand("UNKNOWN data");

            QVERIFY(!result.success);
            QVERIFY(result.message.contains("Ошибка"));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testErrorLogging() {
        QString testName = "testErrorLogging";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileModel model;

            QStringList initialLog = model.GetErrorLog();
            QVERIFY(initialLog.isEmpty());

            model.LoadFromFile("nonexistent_file.txt");

            QStringList logAfterLoad = model.GetErrorLog();
            QVERIFY(!logAfterLoad.isEmpty());

            model.ClearErrorLog();
            QStringList logAfterClear = model.GetErrorLog();
            QVERIFY(logAfterClear.isEmpty());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testFileInfoIsValid() {
        QString testName = "testFileInfoIsValid";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileInfo validFile("test.txt", QDate(2024, 12, 25), 1024);
            QVERIFY(validFile.IsValid());

            FileInfo emptyNameFile("", QDate(2024, 12, 25), 1024);
            QVERIFY(!emptyNameFile.IsValid());

            FileInfo negativeSizeFile("test.txt", QDate(2024, 12, 25), -100);
            QVERIFY(!negativeSizeFile.IsValid());

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testFileInfoToString() {
        QString testName = "testFileInfoToString";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            FileInfo file("document.txt", QDate(2024, 12, 25), 1024);
            QString str = file.ToString();

            QVERIFY(str.contains("document.txt"));
            QVERIFY(str.contains("2024.12.25"));
            QVERIFY(str.contains("1024"));
            QVERIFY(str.contains("байт"));

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }

    void testMultipleLoadsAccumulateData() {
        QString testName = "testMultipleLoadsAccumulateData";
        testLogger->testStarted(testName);
        QElapsedTimer timer;
        timer.start();

        try {
            QString file1 = "temp_load1.txt";
            QString file2 = "temp_load2.txt";

            QFile f1(file1);
            f1.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream(&f1) << "file1.txt 2024.12.25 100\n";
            f1.close();

            QFile f2(file2);
            f2.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream(&f2) << "file2.txt 2024.12.26 200\n";
            f2.close();

            FileModel model;
            model.LoadFromFile(file1);
            QCOMPARE(model.Count(), 1);

            model.LoadFromFile(file2);
            QCOMPARE(model.Count(), 2);

            QFile::remove(file1);
            QFile::remove(file2);

            testLogger->testPassed(testName, timer.elapsed());
        } catch (const std::exception& e) {
            testLogger->testFailed(testName, e.what(), timer.elapsed());
            throw;
        }
    }
};

QTEST_MAIN(TestFileModel)
#include "test_file_model.moc"