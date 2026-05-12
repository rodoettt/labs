#include <QtTest>
#include "../model/FileManagerModel.h"

class TestFileManagerModel : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Тесты для класса File
    void testFileCreation();
    void testFileValidation();
    void testFileToString();

    // Тесты для модели
    void testAddValidFile();
    void testAddInvalidFile();
    void testRemoveFile();
    void testTotalSize();

    // Тесты парсинга
    void testParseValidLine();
    void testParseInvalidLines();
    void testParseAndAddFromLines();

    // Тесты поиска
    void testFindBySize();
    void testFindByDate();

    // Тесты исключений
    void testExceptions();

private:
    FileManagerModel* model;
};

void TestFileManagerModel::initTestCase() {
    qDebug() << "Начало тестирования FileManagerModel";
}

void TestFileManagerModel::cleanupTestCase() {
    qDebug() << "Завершение тестирования";
}

void TestFileManagerModel::init() {
    model = new FileManagerModel();
}

void TestFileManagerModel::cleanup() {
    delete model;
}

// Тесты для File
void TestFileManagerModel::testFileCreation() {
    File file("test.txt", "2024.01.15", 1024);
    QCOMPARE(file.getFilename(), QString("test.txt"));
    QCOMPARE(file.getDate(), QString("2024.01.15"));
    QCOMPARE(file.getSize(), 1024);
}

void TestFileManagerModel::testFileValidation() {
    // Корректный файл
    File validFile("doc.txt", "2024.01.15", 1000);
    QVERIFY(validFile.isValid());

    // Пустое имя
    File emptyName("", "2024.01.15", 1000);
    QVERIFY(!emptyName.isValid());

    // Некорректная дата
    File invalidDate("doc.txt", "2024-01-15", 1000);
    QVERIFY(!invalidDate.isValid());

    // Отрицательный размер
    File negativeSize("doc.txt", "2024.01.15", -100);
    QVERIFY(!negativeSize.isValid());

    // Недопустимые символы в имени
    File invalidChars("doc<.txt", "2024.01.15", 1000);
    QVERIFY(!invalidChars.isValid());
}

void TestFileManagerModel::testFileToString() {
    File file("test.txt", "2024.01.15", 1024);
    QCOMPARE(file.toString(), QString("test.txt|2024.01.15|1024"));
}

// Тесты модели
void TestFileManagerModel::testAddValidFile() {
    QCOMPARE(model->getFileCount(), 0);

    model->addFile("file1.txt", "2024.01.15", 1000);
    QCOMPARE(model->getFileCount(), 1);

    model->addFile(File("file2.txt", "2024.01.16", 2000));
    QCOMPARE(model->getFileCount(), 2);
}

void TestFileManagerModel::testAddInvalidFile() {
    int initialErrorCount = model->getErrorLog().size();

    // Пустое имя
    model->addFile("", "2024.01.15", 1000);
    QCOMPARE(model->getFileCount(), 0);
    QVERIFY(model->getErrorLog().size() > initialErrorCount);

    // Некорректная дата
    model->addFile("file.txt", "2024/01/15", 1000);
    QCOMPARE(model->getFileCount(), 0);

    // Отрицательный размер
    model->addFile("file.txt", "2024.01.15", -500);
    QCOMPARE(model->getFileCount(), 0);
}

void TestFileManagerModel::testRemoveFile() {
    model->addFile("file1.txt", "2024.01.15", 1000);
    model->addFile("file2.txt", "2024.01.16", 2000);
    QCOMPARE(model->getFileCount(), 2);

    model->removeFile(0);
    QCOMPARE(model->getFileCount(), 1);
    QCOMPARE(model->getFiles()[0].getFilename(), QString("file2.txt"));

    // Попытка удалить несуществующий индекс
    int errorCount = model->getErrorLog().size();
    model->removeFile(10);
    QVERIFY(model->getErrorLog().size() > errorCount);
}

void TestFileManagerModel::testTotalSize() {
    QCOMPARE(model->getTotalSize(), 0LL);

    model->addFile("file1.txt", "2024.01.15", 1000);
    model->addFile("file2.txt", "2024.01.16", 2000);
    model->addFile("file3.txt", "2024.01.17", 3000);

    QCOMPARE(model->getTotalSize(), 6000LL);
}

// Тесты парсинга
void TestFileManagerModel::testParseValidLine() {
    File file = model->parseLine("test.txt|2024.01.15|1024");
    QCOMPARE(file.getFilename(), QString("test.txt"));
    QCOMPARE(file.getDate(), QString("2024.01.15"));
    QCOMPARE(file.getSize(), 1024);

    // С пробелами
    File file2 = model->parseLine("  test2.txt  |  2024.01.16  |  2048  ");
    QCOMPARE(file2.getFilename(), QString("test2.txt"));
    QCOMPARE(file2.getSize(), 2048);
}

void TestFileManagerModel::testParseInvalidLines() {
    // Пустая строка
    QVERIFY_THROWS_EXCEPTION(ParseException, model->parseLine(""));

    // Неверное количество полей
    QVERIFY_THROWS_EXCEPTION(ParseException, model->parseLine("test.txt|2024.01.15"));
    QVERIFY_THROWS_EXCEPTION(ParseException, model->parseLine("test.txt|2024.01.15|1024|extra"));

    // Размер не число
    QVERIFY_THROWS_EXCEPTION(ParseException, model->parseLine("test.txt|2024.01.15|abc"));

    // Некорректная дата
    QVERIFY_THROWS_EXCEPTION(InvalidDateException, model->parseLine("test.txt|2024/01/15|1024"));

    // Отрицательный размер
    QVERIFY_THROWS_EXCEPTION(InvalidSizeException, model->parseLine("test.txt|2024.01.15|-100"));
}

void TestFileManagerModel::testParseAndAddFromLines() {
    QStringList validLines = {
        "file1.txt|2024.01.15|1000",
        "file2.txt|2024.01.16|2000",
        "file3.txt|2024.01.17|3000"
    };

    int added = model->parseAndAddFromLines(validLines);
    QCOMPARE(added, 3);
    QCOMPARE(model->getFileCount(), 3);

    // Смешанные строки
    model->clearAll();
    QStringList mixedLines = {
        "valid1.txt|2024.01.15|1000",
        "invalid|date|notnumber",
        "valid2.txt|2024.01.16|2000",
        "",
        "valid3.txt|2024.01.17|3000"
    };

    added = model->parseAndAddFromLines(mixedLines);
    QCOMPARE(added, 3); // 3 корректные строки
    QCOMPARE(model->getFileCount(), 3);
    QVERIFY(model->getErrorLog().size() > 0); // Должны быть ошибки
}

// Тесты поиска
void TestFileManagerModel::testFindBySize() {
    model->addFile("small.txt", "2024.01.15", 500);
    model->addFile("medium.txt", "2024.01.16", 1500);
    model->addFile("large.txt", "2024.01.17", 2500);
    model->addFile("huge.txt", "2024.01.18", 5000);

    auto result = model->findFilesBySize(1000, 3000);
    QCOMPARE(result.size(), 2);

    result = model->findFilesBySize(0, 1000);
    QCOMPARE(result.size(), 1);
    QCOMPARE(result[0].getFilename(), QString("small.txt"));
}

void TestFileManagerModel::testFindByDate() {
    model->addFile("file1.txt", "2024.01.15", 1000);
    model->addFile("file2.txt", "2024.01.15", 2000);
    model->addFile("file3.txt", "2024.01.16", 3000);

    auto result = model->findFilesByDate("2024.01.15");
    QCOMPARE(result.size(), 2);

    result = model->findFilesByDate("2024.01.17");
    QCOMPARE(result.size(), 0);
}

// Тесты исключений
void TestFileManagerModel::testExceptions() {
    // Проверка создания исключений
    try {
        throw InvalidFilenameException("bad<>name.txt");
        QFAIL("Исключение не было выброшено");
    } catch (const InvalidFilenameException& e) {
        QVERIFY(QString(e.what()).contains("bad<>name.txt"));
    }

    try {
        throw InvalidDateException("2024/01/15");
        QFAIL("Исключение не было выброшено");
    } catch (const InvalidDateException& e) {
        QVERIFY(QString(e.what()).contains("2024/01/15"));
    }

    try {
        throw InvalidSizeException(-100);
        QFAIL("Исключение не было выброшено");
    } catch (const InvalidSizeException& e) {
        QVERIFY(QString(e.what()).contains("-100"));
    }
}

// Заглушка для QTEST_MAIN
QTEST_MAIN(TestFileManagerModel)
#include "test_model.moc"