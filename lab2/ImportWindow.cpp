#include "ImportWindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

ImportWindow::ImportWindow(QWidget* parent)
    : QDialog(parent) {
    setupUI();
    setWindowTitle("Импорт данных");
    setMinimumSize(500, 400);
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

    // Область для вывода содержимого файла
    contentEdit = new QTextEdit(this);
    contentEdit->setReadOnly(true);
    contentEdit->setPlaceholderText("Здесь будет отображаться содержимое файла...");

    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(contentEdit);

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
        }
        QPushButton:hover {
            background-color: #0051d5;
        }
        QTextEdit {
            border: 1px solid #d0d0d0;
            border-radius: 4px;
            background-color: white;
            font-family: monospace;
            font-size: 11px;
        }
    )");
}

void ImportWindow::onOpenFile() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Выберите файл с данными",
        QDir::homePath(),
        "Текстовые файлы (*.txt);;Все файлы (*.*)"
        );

    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл!");
        return;
    }

    QTextStream stream(&file);
    currentFileContent = stream.readAll();
    file.close();

    // Выводим содержимое файла
    contentEdit->setPlainText(currentFileContent);
    processButton->setEnabled(!currentFileContent.isEmpty());
}

void ImportWindow::onProcessData() {
    if (currentFileContent.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет данных для обработки!");
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