#include "ImportWindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

// Укажите здесь путь к вашему тестовому файлу
// Например: "/Users/username/Documents/data.txt" или "data.txt" для файла в папке с программой
static const QString DEFAULT_FILE_PATH = "/Users/rodoet/lab3/data.txt";

ImportWindow::ImportWindow(QWidget* parent)
    : QDialog(parent) {
    setupUI();
    setWindowTitle("Импорт данных");
    setMinimumSize(500, 400);
    setModal(true);
}

void ImportWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    openFileButton = new QPushButton("Открыть файл", this);
    processButton = new QPushButton("Обработать", this);

    processButton->setEnabled(false);

    buttonLayout->addWidget(openFileButton);
    buttonLayout->addWidget(processButton);
    buttonLayout->addStretch();

    // Информация о пути к файлу
    QLabel* pathLabel = new QLabel(this);
    pathLabel->setText(QString("Путь к файлу: %1").arg(DEFAULT_FILE_PATH));
    pathLabel->setStyleSheet("color: #666; font-size: 10px; padding: 5px;");

    // Область для вывода содержимого файла
    contentEdit = new QTextEdit(this);
    contentEdit->setReadOnly(true);
    contentEdit->setPlaceholderText("Нажмите 'Открыть файл' для загрузки данных...");
    contentEdit->setStyleSheet(R"(
        QTextEdit {
            font-family: monospace;
            font-size: 11px;
            background-color: #f8f8f8;
        }
    )");

    // Статусная строка
    statusLabel = new QLabel("Готов к работе", this);
    statusLabel->setStyleSheet("color: #666; padding: 5px;");

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(contentEdit);
    mainLayout->addWidget(statusLabel);

    // Подключение сигналов
    connect(openFileButton, &QPushButton::clicked, this, &ImportWindow::onOpenFile);
    connect(processButton, &QPushButton::clicked, this, &ImportWindow::onProcessData);

    // Стилизация
    setStyleSheet(R"(
        QDialog {
            background-color: #f5f5f5;
        }
        QPushButton {
            background-color: #007aff;
            border: none;
            border-radius: 5px;
            color: white;
            font-size: 12px;
            font-weight: bold;
            padding: 8px 16px;
            margin: 5px;
        }
        QPushButton:hover {
            background-color: #0051d5;
        }
        QPushButton:pressed {
            background-color: #0040a8;
        }
        QTextEdit {
            border: 1px solid #d0d0d0;
            border-radius: 4px;
        }
    )");
}

void ImportWindow::loadFile() {
    QFile file(DEFAULT_FILE_PATH);

    if (!file.exists()) {
        QString errorMsg = QString("Файл не найден: %1\nСоздайте файл с данными в формате:\nназвание|ГГГГ.ММ.ДД|размер").arg(DEFAULT_FILE_PATH);
        statusLabel->setText(errorMsg);
        statusLabel->setStyleSheet("color: #dc3545; padding: 5px;");
        contentEdit->setPlainText(errorMsg);
        processButton->setEnabled(false);
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = QString("Не удалось открыть файл: %1").arg(DEFAULT_FILE_PATH);
        statusLabel->setText(errorMsg);
        statusLabel->setStyleSheet("color: #dc3545; padding: 5px;");
        contentEdit->setPlainText(errorMsg);
        processButton->setEnabled(false);
        return;
    }

    QTextStream stream(&file);
    currentFileContent = stream.readAll();
    file.close();

    // Выводим содержимое файла
    contentEdit->setPlainText(currentFileContent);

    // Информация о файле
    QStringList lines = currentFileContent.split("\n", Qt::SkipEmptyParts);
    statusLabel->setText(QString("Загружено %1 строк из %2").arg(lines.size()).arg(DEFAULT_FILE_PATH));
    statusLabel->setStyleSheet("color: #28a745; padding: 5px;");
    processButton->setEnabled(!currentFileContent.isEmpty());

    qDebug() << "File loaded, lines:" << lines.size();
}

void ImportWindow::onOpenFile() {
    loadFile();
}

void ImportWindow::onProcessData() {
    if (currentFileContent.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет данных для обработки!\nНажмите 'Открыть файл' сначала.");
        return;
    }

    // Разбиваем содержимое на строки
    QStringList lines = currentFileContent.split("\n", Qt::SkipEmptyParts);

    if (lines.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Файл пуст!");
        return;
    }

    emit dataProcessed(lines);
    accept(); // Закрываем окно
}