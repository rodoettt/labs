#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include "view/mainwindow.h"

bool runTests() {
    qDebug() << "========================================";
    qDebug() << "Запуск предварительного тестирования";
    qDebug() << "========================================";

    QString testProgram = "./file_tests";

    if (!QFile::exists(testProgram)) {
        qDebug() << "Файл тестов не найден, пропускаем тестирование";
        return true;
    }

    QProcess testProcess;
    testProcess.start(testProgram, QStringList() << "-silent");
    testProcess.waitForFinished(30000);

    int exitCode = testProcess.exitCode();

    if (exitCode == 0) {
        qDebug() << "========================================";
        qDebug() << "Все тесты пройдены успешно";
        qDebug() << "========================================";
        return true;
    } else {
        qDebug() << "========================================";
        qDebug() << "Тесты не пройдены (код:" << exitCode << ")";
        qDebug() << "========================================";
        return false;
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    bool skipTests = false;
    for (int i = 1; i < argc; ++i) {
        if (QString(argv[i]) == "--skip-tests") {
            skipTests = true;
            break;
        }
    }

    if (!skipTests) {
        if (!runTests()) {
            qDebug() << "Приложение не будет запущено из-за ошибок в тестах";
            return 1;
        }
    } else {
        qDebug() << "Пропускаем тесты (--skip-tests)";
    }

    MainWindow window;
    window.show();

    return app.exec();
}